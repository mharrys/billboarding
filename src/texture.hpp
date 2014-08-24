#ifndef TEXTURE_HPP_INCLUDED
#define TEXTURE_HPP_INCLUDED

#include <memory>
#include <vector>

struct Image;

struct GLTexture;

enum class TextureTarget {
    TEXTURE_2D
};

enum class TextureFormat {
    RGB,
    RGBA
};

enum class FilterParameter {
    MIN_FILTER,
    MAG_FILTER
};

enum class FilterMode {
    NEAREST,
    LINEAR
};

typedef std::vector<std::pair<FilterParameter, FilterMode>> FilterParameters;

enum class WrapParameter {
    WRAP_S,
    WRAP_T,
    WRAP_R
};

enum class WrapMode {
    CLAMP_TO_EDGE,
    MIRRORED_REPEAT,
    REPEAT
};

typedef std::vector<std::pair<WrapParameter, WrapMode>> WrapParameters;

struct TextureSetting {
    TextureFormat internal_format;
    TextureFormat format;
    FilterParameters filter_param;
    WrapParameters wrap_param;
};

class Texture {
public:
    Texture(TextureTarget target);
    ~Texture();
    void bind(int unit);
    void make(Image & image, TextureSetting & setting);
private:
    void set_parameters(FilterParameters & filter_param);
    void set_parameters(WrapParameters & wrap_param);

    std::unique_ptr<GLTexture> gl;
};

#endif
