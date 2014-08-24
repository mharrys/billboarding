#ifndef GL_HPP_INCLUDED
#define GL_HPP_INCLUDED

#include "GL/glew.h"

#include <iostream>
#include <string>

enum VertexAttribIndex {
    VERTEX_POSITION_INDEX = 0,
    VERTEX_NORMAL_INDEX,
    VERTEX_COLOR_INDEX,
    VERTEX_TEX_COORD_INDEX
};

struct GLVertexArray {
    GLuint name;
    GLuint indices_size;
    GLuint positions_size;
    GLuint positions_buffer;
    GLuint normals_buffer;
    GLuint colors_buffer;
    GLuint tex_coords_buffer;
    GLuint indices_buffer;
};

struct GLShader {
    GLenum type;
    GLuint shader;
};

struct GLProgram {
    GLuint program;
};

struct GLTexture {
    GLenum target;
    GLuint texture;
};

static inline void gl_print_error()
{
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        std::string error_str;
        switch (error) {
            case GL_INVALID_ENUM:
                error_str =
                    "GL_INVALID_ENUM. An unacceptable value is specified for an "
                    "enumerated argument. The offending command is ignored and "
                    "has no other side effect than to set the error flag.";
                break;
            case GL_INVALID_VALUE:
                error_str =
                    "GL_INVALID_VALUE. A numeric argument is out of range. The "
                    "offending command is ignored and has no other side effect "
                    "than to set the error flag.";
                break;
            case GL_INVALID_OPERATION:
                error_str =
                    "GL_INVALID_OPERATION. The specified operation is not "
                    "allowed in the current state. The offending command "
                    "is ignored and has no other side effect than to set "
                    "the error flag.";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error_str =
                    "GL_INVALID_FRAMEBUFFER_OPERATION. The framebuffer object "
                    "is not complete. The offending command is ignored and "
                    "has no other side effect than to set the error flag.";
                break;
            case GL_OUT_OF_MEMORY:
                error_str =
                    "GL_OUT_OF_MEMORY. There is not enough memory left to "
                    "execute the command. The state of the GL is undefined, "
                    "except for the state of the error flags, after this error "
                    "is recorded.";
                break;
            default:
                error_str = "Uknown error code.";
                break;
        }
        std::cerr << "OpenGL error (" << error << "): " << error_str << std::endl;
    }
}

#endif
