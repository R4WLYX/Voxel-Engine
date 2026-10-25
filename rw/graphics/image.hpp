#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

class Image {
  public:
    Image(const std::string& path);

  private:
    unsigned char* _buffer;
    int _width;
    int _height;
    int _channels;
};

inline Image::Image(const std::string& path) {
    _buffer = stbi_load(path.c_str(), &_width, &_height, &_channels, 4);
}