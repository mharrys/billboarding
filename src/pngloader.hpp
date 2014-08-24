#ifndef PNGLOADER_HPP_INCLUDED
#define PNGLOADER_HPP_INCLUDED

#include <memory>
#include <string>

struct Image;

class PNGLoader {
public:
    static std::unique_ptr<Image> load(std::string filename);
};

#endif
