

#include <nanogui/widget.h>
#include <iostream>
#include "WaterFrameBuffers.h"

const int REFLECTION_WIDTH = 320;
const int REFLECTION_HEIGHT = 180;

const int REFRACTION_WIDTH = 1280;
const int REFRACTION_HEIGHT = 720;

WaterFrameBuffers::WaterFrameBuffers() {//call when loading the game
    initialiseReflectionFrameBuffer();
    initialiseRefractionFrameBuffer();
}

void WaterFrameBuffers::cleanUp() {//call when closing the game
    glDeleteFramebuffers(1, &reflectionFrameBuffer);
    glDeleteTextures(1, &reflectionTexture);
    glDeleteRenderbuffers(1, &reflectionDepthBuffer);
    glDeleteFramebuffers(1, &refractionFrameBuffer);
    glDeleteTextures(1, &refractionTexture);
    glDeleteTextures(1, &refractionDepthTexture);
}


void WaterFrameBuffers::bindReflectionFrameBuffer() {//call before rendering to this FBO
    bindFrameBuffer(reflectionFrameBuffer, REFLECTION_WIDTH, REFLECTION_HEIGHT);
}


void WaterFrameBuffers::bindRefractionFrameBuffer() {//call before rendering to this FBO
    bindFrameBuffer(refractionFrameBuffer, REFRACTION_WIDTH, REFRACTION_HEIGHT);
}


void WaterFrameBuffers::unbindCurrentFrameBuffer() {//call to switch to default frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


GLuint WaterFrameBuffers::getReflectionTexture() {//get the resulting texture
    return reflectionTexture;
}


GLuint WaterFrameBuffers::getRefractionTexture() {//get the resulting texture
    return refractionTexture;
}


GLuint WaterFrameBuffers::getRefractionDepthTexture() {//get the resulting depth texture
    return refractionDepthTexture;
}

GLuint WaterFrameBuffers::getRefractionFrameBuffer() {//get the resulting depth texture
    return refractionDepthTexture;
}

GLuint WaterFrameBuffers::getReflectionFrameBuffer() {//get the resulting depth texture
    return refractionDepthTexture;
}


void WaterFrameBuffers::initialiseReflectionFrameBuffer() {
    reflectionFrameBuffer = createFrameBuffer();
    reflectionTexture = createTextureAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
    reflectionDepthBuffer = createDepthBufferAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
    auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Warning: Background framebuffer is not complete: " << fboStatus << std::endl;
    unbindCurrentFrameBuffer();
}


void WaterFrameBuffers::initialiseRefractionFrameBuffer() {
    refractionFrameBuffer = createFrameBuffer();
    refractionTexture = createTextureAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
    refractionDepthTexture = createDepthTextureAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
    auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Warning: Background framebuffer is not complete: " << fboStatus << std::endl;
    unbindCurrentFrameBuffer();
}


void WaterFrameBuffers::bindFrameBuffer(int frameBuffer, int width, int height) {
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glBindTexture(GL_TEXTURE_2D, 0);//To make sure the texture isn't bound
    glViewport(0, 0, width, height);
}


GLuint WaterFrameBuffers::createFrameBuffer() {
    GLuint frameBuffer;
    glGenFramebuffers(1, &frameBuffer);
    //generate name for frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    //create the framebuffer
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    //indicate that we will always render to color attachment 0

    return frameBuffer;
}

GLuint WaterFrameBuffers::createTextureAttachment(int width, int height) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    return texture;
}

GLuint WaterFrameBuffers::createDepthTextureAttachment(int width, int height) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0,
                 GL_DEPTH_COMPONENT32, width, height,
                 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D, texture, 0);
    return texture;
}

GLuint WaterFrameBuffers::createDepthBufferAttachment(int width, int height) {
    GLuint depthBuffer;
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width,
                          height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, depthBuffer);
    return depthBuffer;
}
