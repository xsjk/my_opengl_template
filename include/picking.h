#ifndef _PICKING_H_
#define _PICKING_H_

#include <defines.h>

struct Pixel { GLubyte red, green, blue; };

class PickingTexture
{

public:

    ~PickingTexture() {
        if (m_fbo != 0) {
            glDeleteFramebuffers(1, &m_fbo);
        }
        if (m_pickingTexture != 0) {
            glDeleteTextures(1, &m_pickingTexture);
        }
        if (m_depthTexture != 0) {
            glDeleteTextures(1, &m_depthTexture);
        }
    }

    bool init(unsigned int WindowWidth, unsigned int WindowHeight)
    {
        // Create the FBO
        glGenFramebuffers(1, &m_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

        // Create the texture object for the primitive information buffer
        glGenTextures(1, &m_pickingTexture);
        glBindTexture(GL_TEXTURE_2D, m_pickingTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32UI, WindowWidth, WindowHeight, 0, GL_RGB_INTEGER, GL_UNSIGNED_INT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pickingTexture, 0);

        // Create the texture object for the depth buffer
        glGenTextures(1, &m_depthTexture);
        glBindTexture(GL_TEXTURE_2D, m_depthTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);

        // Verify that the FBO is correct
        GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

        if (Status != GL_FRAMEBUFFER_COMPLETE) {
            printf("FB error, status: 0x%x\n", Status);
            exit(1);
        }

        // Restore the default framebuffer
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return true;
    }


    void activate() 
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    }

    void set_color(int id) {
        float r = (id & 0x000000FF) >>  0;
        float g = (id & 0x0000FF00) >>  8;
        float b = (id & 0x00FF0000) >> 16;
        float a = (id & 0xFF000000) >> 24;
        glClearColor(r/255.0f, g/255.0f, b/255.0f, a/255.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void deactivate()
    {
        // Bind back the default framebuffer
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }


    vec3 ReadPixel(unsigned int x, unsigned int y)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);

        glReadBuffer(GL_COLOR_ATTACHMENT0);

        vec3 Pixel;
        glReadPixels(x, y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &Pixel);

        glReadBuffer(GL_NONE);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

        return Pixel;
    }

    vec3 read(unsigned x, unsigned y) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        vec3 color;
        glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &color);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        return color;
    }


private:
    GLuint m_fbo = 0, m_pickingTexture = 0, m_depthTexture;
};


#endif