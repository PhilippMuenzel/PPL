// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, Inc.

#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <vector>

namespace PPL {

class Texture
{
public:

#pragma pack(push, ident, 2)

    struct BMPFILEHEADER
    {
        short   bfType;
        long    bfSize;
        short   bfReserved1;
        short   bfReserved2;
        long    bfOffBits;
    };

    struct BMPINFOHEADER
    {
        long    biSize;
        long    biWidth;
        long    biHeight;
        short   biPlanes;
        short   biBitCount;
        long    biCompression;
        long    biSizeImage;
        long    biXPelsPerMeter;
        long    biYPelsPerMeter;
        long    biClrUsed;
        long    biClrImportant;
    };

    struct IMAGEDATA
    {
        std::vector<unsigned char> pData;
        long    Width;
        long    Height;
        long    Padding;
        short   Channels;
        unsigned int bpp;
    };

#pragma pack(pop, ident)

    Texture(const std::string& file_name);
    ~Texture();
    int id() const;
    int width() const;
    int height() const;

private:

    void swapRedBlue();
    IMAGEDATA m_imagedata;
    int m_id;

};

}

#endif // TEXTURE_H
