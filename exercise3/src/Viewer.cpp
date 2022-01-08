// This source code is property of the Computer Graphics and Visualization 
// chair of the TU Dresden. Do not distribute! 
// Copyright (C) CGV TU Dresden - All Rights Reserved

#include "Viewer.h"

#include <nanogui/window.h>
#include <nanogui/layout.h>
#include <nanogui/checkbox.h>

#include <gui/SliderHelper.h>

#include <iostream>

#include <stb_image.h>

#include "glsl.h"
#include "textures.h"

const uint32_t PATCH_SIZE = 256; //number of vertices along one side of the terrain patch

Viewer::Viewer()
        : AbstractViewer("CG1 Exercise 3"),
          terrainPositions(nse::gui::VertexBuffer), terrainIndices(nse::gui::IndexBuffer),
          offsetBuffer(nse::gui::VertexBuffer) {
    LoadShaders();
    CreateGeometry();

    //Create a texture and framebuffer for the background
    glGenFramebuffers(1, &backgroundFBO);
    glGenTextures(1, &backgroundTexture);

    //Align camera to view a reasonable part of the terrain
    camera().SetSceneExtent(nse::math::BoundingBox<float, 3>(Eigen::Vector3f(0, 0, 0),
                                                             Eigen::Vector3f(PATCH_SIZE - 1, 0, PATCH_SIZE - 1)));
    camera().FocusOnPoint(0.5f * Eigen::Vector3f(PATCH_SIZE - 1, 15, PATCH_SIZE - 1));
    camera().Zoom(-30);
    camera().RotateAroundFocusPointLocal(
            Eigen::AngleAxisf(-0.5f, Eigen::Vector3f::UnitY()) * Eigen::AngleAxisf(-0.05f, Eigen::Vector3f::UnitX()));
    camera().FixClippingPlanes(0.1f, 1000.f);
}

bool Viewer::resizeEvent(const Eigen::Vector2i &) {
    //Re-generate the texture and FBO for the background
    glBindFramebuffer(GL_FRAMEBUFFER, backgroundFBO);
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width(), height(), 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, backgroundTexture, 0);
    auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Warning: Background framebuffer is not complete: " << fboStatus << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return false;
}

void Viewer::LoadShaders() {
    skyShader.init("Sky Shader", std::string((const char *) sky_vert, sky_vert_size),
                   std::string((const char *) sky_frag, sky_frag_size));
    terrainShader.init("Terrain Shader", std::string((const char *) terrain_vert, terrain_vert_size),
                       std::string((const char *) terrain_frag, terrain_frag_size));
}

GLuint CreateTexture(const unsigned char *fileData, size_t fileLength, bool repeat = true) {
    GLuint textureName;
    GLuint wrapMode = GL_MIRRORED_REPEAT;
    if (!repeat) {
        wrapMode = GL_CLAMP_TO_BORDER;
    }
    glGenTextures(1, &textureName);
    glBindTexture(GL_TEXTURE_2D, textureName);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); //minification Filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //magnification Filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode); //WrapMode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrapMode);
    int textureWidth, textureHeight, textureChannels;
    unsigned char *pixelData = stbi_load_from_memory(fileData, (int) fileLength, &textureWidth, &textureHeight,
                                                     &textureChannels, 3);
    if (pixelData) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE,
                     pixelData); //create Texture
        glGenerateMipmap(GL_TEXTURE_2D); //Genrate MipMap
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(pixelData);
    return textureName;
}

void Viewer::CreateGeometry() {
    //empty VAO for sky
    emptyVAO.generate();

    //terrain VAO
    terrainVAO.generate();
    terrainVAO.bind();

    std::vector<Eigen::Vector4f> positions;
    std::vector<uint32_t> indices;

    /*Generate positions and indices for a terrain patch with a
      single triangle strip */

    uint32_t currentIndex = 0;
    for (float x = 0.0; x < PATCH_SIZE; x += 1.0f) {
        for (float z = 0.0; z < PATCH_SIZE; z += 1.0f) {
            if (currentIndex >= PATCH_SIZE) {
                indices.push_back(currentIndex - PATCH_SIZE);
            }
            indices.push_back(currentIndex);
            currentIndex++;
            positions.emplace_back(x, 0.0f, z, 1.0f);
        }
        indices.push_back(UINT32_MAX); //UINT32_MAX is our restart index
    }
    /* enable primitive restart and set the restart index*/
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(UINT32_MAX);

    terrainShader.bind();
    terrainPositions.uploadData(positions).bindToAttribute("position");
    terrainIndices.uploadData((uint32_t) indices.size() * sizeof(uint32_t), indices.data());

    offsetBuffer.bind();
    std::vector<Eigen::Vector2f> offset;
    offset.emplace_back(0,0);
    offsetBuffer.uploadData(offset).bindToAttribute("offset");
    glVertexAttribDivisor(terrainShader.attrib("offset"), offset.size());


    //textures
    grassTexture = CreateTexture((unsigned char *) grass_jpg, grass_jpg_size);
    rockTexture = CreateTexture((unsigned char *) rock_jpg, rock_jpg_size);
    roadColorTexture = CreateTexture((unsigned char *) roadcolor_jpg, roadcolor_jpg_size);
    roadNormalMap = CreateTexture((unsigned char *) roadnormals_jpg, roadnormals_jpg_size);
    roadSpecularMap = CreateTexture((unsigned char *) roadspecular_jpg, roadspecular_jpg_size);
    alphaMap = CreateTexture((unsigned char *) alpha_jpg, alpha_jpg_size, false);
}

void Viewer::RenderSky() {
    Eigen::Matrix4f skyView = view;
    for (int i = 0; i < 3; ++i)
        skyView.col(i).normalize();
    skyView.col(3).head<3>().setZero();
    Eigen::Matrix4f skyMvp = proj * skyView;
    glDepthMask(GL_FALSE);
    glEnable(GL_DEPTH_CLAMP);
    emptyVAO.bind();
    skyShader.bind();
    skyShader.setUniform("mvp", skyMvp);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
    glDisable(GL_DEPTH_CLAMP);
    glDepthMask(GL_TRUE);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, backgroundFBO);
    glBlitFramebuffer(0, 0, width(), height(), 0, 0, width(), height(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void CalculateViewFrustum(const Eigen::Matrix4f &mvp, Eigen::Vector4f *frustumPlanes,
                          nse::math::BoundingBox<float, 3> &bbox) {
    std::cout << "calculate Frustum planes!" << std::endl;
    frustumPlanes[0] = (mvp.row(3) + mvp.row(0)).transpose();
    frustumPlanes[1] = (mvp.row(3) - mvp.row(0)).transpose();
    frustumPlanes[2] = (mvp.row(3) + mvp.row(1)).transpose();
    frustumPlanes[3] = (mvp.row(3) - mvp.row(1)).transpose();
    frustumPlanes[4] = (mvp.row(3) + mvp.row(2)).transpose();
    frustumPlanes[5] = (mvp.row(3) - mvp.row(2)).transpose();
    std::cout << "Frustum planes calculated!" << std::endl;
    Eigen::Matrix4f invMvp = mvp.inverse();
    bbox.reset();
    for (int x = -1; x <= 1; x += 2)
        for (int y = -1; y <= 1; y += 2)
            for (int z = -1; z <= 1; z += 2) {
                Eigen::Vector4f corner = invMvp * Eigen::Vector4f((float) x, (float) y, (float) z, 1);
                corner /= corner.w();
                bbox.expand(corner.head<3>());
            }

    std::cout << "finished with planes!" << std::endl;
}

bool
IsBoxCompletelyBehindPlane(const Eigen::Vector3f &boxMin, const Eigen::Vector3f &boxMax, const Eigen::Vector4f &plane) {
    return
            plane.dot(Eigen::Vector4f(boxMin.x(), boxMin.y(), boxMin.z(), 1)) < 0 &&
            plane.dot(Eigen::Vector4f(boxMin.x(), boxMin.y(), boxMax.z(), 1)) < 0 &&
            plane.dot(Eigen::Vector4f(boxMin.x(), boxMax.y(), boxMin.z(), 1)) < 0 &&
            plane.dot(Eigen::Vector4f(boxMin.x(), boxMax.y(), boxMin.z(), 1)) < 0 &&
            plane.dot(Eigen::Vector4f(boxMax.x(), boxMin.y(), boxMin.z(), 1)) < 0 &&
            plane.dot(Eigen::Vector4f(boxMax.x(), boxMin.y(), boxMax.z(), 1)) < 0 &&
            plane.dot(Eigen::Vector4f(boxMax.x(), boxMax.y(), boxMin.z(), 1)) < 0 &&
            plane.dot(Eigen::Vector4f(boxMax.x(), boxMax.y(), boxMin.z(), 1)) < 0;
}

void Viewer::drawContents() {
    camera().ComputeCameraMatrices(view, proj);

    Eigen::Matrix4f mvp = proj * view;



    int visiblePatches = 0;

    /* offset buffer */

    Eigen::Vector4f frustumPlanes;
    nse::math::BoundingBox<float, 3> bbox;
    CalculateViewFrustum(proj * view, &frustumPlanes, bbox);

    /*unsigned int bboxMinX = (int) ceil(bbox.min.x()) - 1;
    unsigned int bboxMinZ = (int) ceil(bbox.min.z()) - 1;
    unsigned int bboxMaxX = (int) ceil(bbox.max.x()) + 1;
    unsigned int bboxMaxZ = (int) ceil(bbox.max.z()) + 1;
    unsigned int minX = (bboxMinX % PATCH_SIZE - 1) * PATCH_SIZE;
    unsigned int minZ = (bboxMinZ % PATCH_SIZE - 1) * PATCH_SIZE;
    unsigned int maxX = (bboxMaxX % PATCH_SIZE - 1) * PATCH_SIZE;
    unsigned int maxZ = (bboxMaxZ % PATCH_SIZE - 1) * PATCH_SIZE;

   /* for (unsigned int xOffset = minX; xOffset <= maxX; xOffset += PATCH_SIZE) {
        for (unsigned int zOffset = minZ; zOffset <= maxZ; zOffset += PATCH_SIZE) {
            Eigen::Vector3f min = {(float) xOffset, 0, (float) zOffset};
            Eigen::Vector3f max = {(float) xOffset + PATCH_SIZE, 15, (float) zOffset + PATCH_SIZE};
            std::cout << "xOffset: " << xOffset << " zOffset: " << zOffset << std::endl;
            //    for (int i = 0; i < frustumPlanes.size(); i++) {
            //       Eigen::Vector4f plane;
            //       plane = frustumPlanes[i]
            //         if (IsBoxCompletelyBehindPlane(min, max, plane)) {
            //visiblePatches++;
            // offset.emplace_back(xOffset, zOffset);

            //      }
            //  }
        }
    }*/

    Eigen::Vector3f cameraPosition = view.inverse().col(3).head<3>();

    RenderSky();

    //render terrain
    glEnable(GL_DEPTH_TEST);
    terrainVAO.bind();
    terrainShader.bind();

    terrainShader.setUniform("screenSize", Eigen::Vector2f(width(), height()), false);
    terrainShader.setUniform("mvp", mvp);
    terrainShader.setUniform("cameraPos", cameraPosition, false);
    /* Task: Render the terrain */


/* Bind Textures */
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, grassTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, rockTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, alphaMap);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, roadColorTexture);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, roadSpecularMap);
    terrainShader.setUniform("grassSampler", 0);
    terrainShader.setUniform("rockSampler", 1);
    terrainShader.setUniform("alphaSampler", 2);
    terrainShader.setUniform("roadSampler", 3);
    terrainShader.setUniform("roadSpecularSampler", 4);
/* Draw Triangle Strips*/
    glDrawElementsInstanced(GL_TRIANGLE_STRIP, terrainIndices.bufferSize(), GL_UNSIGNED_INT, (void *) nullptr,
                            offsetBuffer.bufferSize());

//Render text
    nvgBeginFrame(mNVGContext, (float) width(), (float) height(), mPixelRatio

    );
    std::string text = "Patches visible: " + std::to_string(visiblePatches);
    nvgText(mNVGContext, 10, 20, text.c_str(), nullptr);
    nvgEndFrame(mNVGContext);
}