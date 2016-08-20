#include "Texture2D.h"

using namespace aa::gfx;

Texture2D::Texture2D()
{
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
}
bool Texture2D::bmp(const char* filename)
{
    unsigned char header[54];
    unsigned int dataPos;
    unsigned int width, height;
    unsigned int imageSize;
    char* data = 0;

    // Open the file
    FILE * file = fopen(filename, "rb");
    if (!file)
        return false;
    if (fread(header, 1, 54, file) != 54)
        return false;

    if (header[0] != 'B' || header[1] != 'M')
        return false;

    glBindTexture(GL_TEXTURE_2D, tex);
    // Read ints from the byte array
    dataPos = *(int*)&(header[0x0A]);
    imageSize = *(int*)&(header[0x22]);
    width = *(int*)&(header[0x12]);
    height = *(int*)&(header[0x16]);

    if (imageSize == 0)    imageSize = width*height * 3;    // 3 : one byte for each Red, Green and Blue component
    if (dataPos == 0)      dataPos = 54;                    // The BMP header is done that way
    data = new char[imageSize];
    if (!data)
    {
        glBindTexture(GL_TEXTURE_2D, 0);
        return false;
    }
    fread(data, 1, imageSize, file);
    fclose(file);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
    delete[] data;

    return true;
}
void Texture2D::bind()
{
    glBindTexture(GL_TEXTURE_2D, tex);
}
Texture2D::~Texture2D()
{
    glDeleteTextures(1, &tex);
    tex = 0;
}
