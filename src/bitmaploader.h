// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, Inc.

#ifndef BITMAPLOADER_H
#define BITMAPLOADER_H

#include <string>
#include <vector>

namespace PPL {

class BitmapLoader
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
        long     biSize;
        long     biWidth;
        long     biHeight;
        short    biPlanes;
        short    biBitCount;
        long     biCompression;
        long     biSizeImage;
        long     biXPelsPerMeter;
        long     biYPelsPerMeter;
        long     biClrUsed;
        long     biClrImportant;
    };

    struct IMAGEDATA
    {
        std::vector<unsigned char> pData;
        long			Width;
        long			Height;
        long			Padding;
        short			Channels;
    };

#pragma pack(pop, ident)

    BitmapLoader(const std::string& file_name);
    ~BitmapLoader();
    IMAGEDATA getImageData();

    static void swapRedBlue(IMAGEDATA&);

private:
    IMAGEDATA m_imagedata;

};

}

#endif // BITMAPLOADER_H
