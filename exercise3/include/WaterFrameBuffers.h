#pragma once


#include <glad/glad.h>

class WaterFrameBuffers {
public:
    WaterFrameBuffers();
    void cleanUp();

    void bindRefractionFrameBuffer();

    void bindReflectionFrameBuffer();

    void unbindCurrentFrameBuffer();

    GLuint getReflectionTexture();

    GLuint getRefractionTexture();
    GLuint getRefractionDepthTexture();

private:
    GLuint reflectionFrameBuffer;
    GLuint reflectionTexture;
    GLuint reflectionDepthBuffer;

    GLuint refractionFrameBuffer;
    GLuint refractionTexture;
    GLuint refractionDepthTexture;

    void initialiseReflectionFrameBuffer();
    void initialiseRefractionFrameBuffer();
    void bindFrameBuffer(int frameBuffer, int width, int height);
    GLuint createFrameBuffer();
    GLuint createTextureAttachment( int width, int height);
    GLuint createDepthTextureAttachment(int width, int height);
    GLuint createDepthBufferAttachment(int width, int height);
};