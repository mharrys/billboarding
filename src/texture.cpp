#include "texture.hpp"

#include "image.hpp"

#include "lib/gl.hpp"

#include <iostream>

static GLenum translate_texture_target(TextureTarget target)
{
    switch (target) {
    case TextureTarget::TEXTURE_2D:
        return GL_TEXTURE_2D;
    default:
        return 0;
    }
}

Texture::Texture(TextureTarget target)
    : gl(new GLTexture())
{
    gl->target = translate_texture_target(target);
    glGenBuffers(1, &gl->texture);
}

Texture::~Texture()
{
    glDeleteBuffers(1, &gl->texture);
}

void Texture::bind(int unit)
{
    GLenum texture_unit = GL_TEXTURE0 + unit;

    if (texture_unit >= (GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS - 1)) {
        std::cerr << "Texture::bind: texture unit is out of range." << std::endl;
        return;
    }

    glActiveTexture(texture_unit);
    glBindTexture(gl->target, gl->texture);
}

static GLint translate_texture_format(TextureFormat format)
{
    switch (format) {
    case TextureFormat::RGB:
        return GL_RGB;
    case TextureFormat::RGBA:
        return GL_RGBA;
    default:
        return 0;
    }
}

void Texture::make(Image & image, TextureSetting & setting)
{
    glBindTexture(gl->target, gl->texture);

    set_parameters(setting.filter_param);
    set_parameters(setting.wrap_param);

    glTexImage2D(
        gl->target,
        0,
        translate_texture_format(setting.internal_format),
        image.width,
        image.height,
        0,
        translate_texture_format(setting.format),
        GL_UNSIGNED_BYTE,
        &image.data[0]
    );
}

static GLenum translate_filter_parameter(FilterParameter parameter)
{
    switch (parameter) {
    case FilterParameter::MIN_FILTER:
        return GL_TEXTURE_MIN_FILTER;
    case FilterParameter::MAG_FILTER:
        return GL_TEXTURE_MAG_FILTER;
    default:
        return 0;
    }
}

static GLint translate_filter_mode(FilterMode mode)
{
    switch (mode) {
    case FilterMode::NEAREST:
        return GL_NEAREST;
    case FilterMode::LINEAR:
        return GL_LINEAR;
    default:
        return 0;
    }
}

void Texture::set_parameters(FilterParameters & filter_param)
{
    for (auto parameter : filter_param) {
        glTexParameteri(
            gl->target,
            translate_filter_parameter(parameter.first),
            translate_filter_mode(parameter.second)
        );
    }
}

static GLenum translate_wrap_parameter(WrapParameter parameter)
{
    switch (parameter) {
    case WrapParameter::WRAP_S:
        return GL_TEXTURE_WRAP_S;
    case WrapParameter::WRAP_T:
        return GL_TEXTURE_WRAP_T;
    case WrapParameter::WRAP_R:
        return GL_TEXTURE_WRAP_R;
    default:
        return 0;
    }
}

static GLint translate_wrap_mode(WrapMode mode)
{
    switch(mode) {
    case WrapMode::CLAMP_TO_EDGE:
         return GL_CLAMP_TO_EDGE;
    case WrapMode::MIRRORED_REPEAT:
         return GL_MIRRORED_REPEAT;
    case WrapMode::REPEAT:
         return GL_REPEAT;
    default:
         return 0;
    }
}

void Texture::set_parameters(WrapParameters & wrap_param)
{
    for (auto parameter : wrap_param) {
        glTexParameteri(
            gl->target,
            translate_wrap_parameter(parameter.first),
            translate_wrap_mode(parameter.second)
        );
    }
}
