#include "pngloader.hpp"

#include "image.hpp"

#include "lib/lodepng.hpp"

#include <iostream>

std::unique_ptr<Image> PNGLoader::load(std::string filename)
{
    std::unique_ptr<Image> image(new Image());

    unsigned int error = lodepng::decode(
        image->data,
        image->width,
        image->height,
        filename.c_str()
    );

    if (error != 0) {
        std::cerr << "PNGLoader::load: error loading \"" << filename << "\": "
                  << lodepng_error_text(error) << "." << std::endl;
        return nullptr;
    }

    return image;
}
