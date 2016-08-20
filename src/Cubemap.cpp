#include "Cubemap.h"

using namespace aa::gfx;

Cubemap::Cubemap(unsigned dim)
{
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    if (dim)
    {
        for (GLuint i = 0; i < 6; i++)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, dim, dim, 0, GL_BGRA, GL_UNSIGNED_BYTE, 0);
        }
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
bool Cubemap::bmp(const char* filename[])
{
    unsigned char header[54];
    unsigned int dataPos;
    unsigned int width, height;
    unsigned int imageSize;
    char* data = 0;
    FILE * file;
    int cntSuccess = 0;

    glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
    for (int i = 0; i < 6; i++)
    {
        // Open the file
        file = fopen(filename[i], "rb");
        if (!file)
            continue;
        if (fread(header, 1, 54, file) != 54)
            continue;
        if (header[0] != 'B' || header[1] != 'M')
            continue;

        // Read ints from the byte array
        dataPos = *(int*)&(header[0x0A]);
        imageSize = *(int*)&(header[0x22]);
        width = *(int*)&(header[0x12]);
        height = *(int*)&(header[0x16]);

        if (imageSize == 0)    imageSize = width*height * 3;    // 3 : one byte for each Red, Green and Blue component
        if (dataPos == 0)      dataPos = 54;                    // The BMP header is done that way
        data = new char[imageSize];
        if (!data)
            continue;

        fread(data, 1, imageSize, file);
        fclose(file);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
        delete[] data;
        cntSuccess++;
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    if (cntSuccess < 6)
        return false;

    return true;
}
void Cubemap::bind()
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
}
Cubemap::~Cubemap()
{
    glDeleteTextures(1, &tex);
    tex = 0;
}
