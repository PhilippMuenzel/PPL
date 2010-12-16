#include <fstream>
#include <stdexcept>
#include "bitmaploader.h"
#include "XPLMUtilities.h"

#if APL && defined(__ppc__)
    short Endian(short Data)
    {
        unsigned char *pBuffer = (unsigned char *)&Data;
        short Result = (short)(pBuffer[0] & 0xff)	+ ( (short)(pBuffer[1] & 0xff) << 8) ;
        return(Result);
    }

    long Endian(long Data)
    {
        unsigned char *pBuffer = (unsigned char *)&Data;

        long Result = 		(long)(pBuffer[0] & 0xff)
                        + ( (long)(pBuffer[1] & 0xff) << 8)
                        + ( (long)(pBuffer[2] & 0xff) << 16)
                        + ( (long)(pBuffer[3] & 0xff) << 24);

        return(Result);
    }

    void SwapEndian(short *Data)
    {
        *Data = Endian(*Data);
    }

    void SwapEndian(long *Data)
    {
        *Data = Endian(*Data);
    }
#else
    /// Only the mac needs these so dummy functions for windows and linux.
    void SwapEndian(short *){}
    void SwapEndian(long *){}
#endif


BitmapLoader::BitmapLoader(const std::string& file_name)
{
    BMPFILEHEADER header;
    BMPINFOHEADER image_info;
    long padding;

    std::ifstream fs(file_name.c_str(), std::ios_base::in | std::ios_base::binary);
    if (!fs)
        throw std::runtime_error("File could not be openend");
    fs.read(reinterpret_cast<char*>(&header), sizeof(header));
    if (!fs)
        throw std::runtime_error("Bitmap header could not be read");
    fs.read(reinterpret_cast<char*>(&image_info), sizeof(image_info));
    if (!fs)
        throw std::runtime_error("Image info could not be read");
#if APL && defined(__ppc__)
    SwapEndian(&header.bfSize);
    SwapEndian(&header.bfOffBits);

    SwapEndian(&image_info.biWidth);
    SwapEndian(&image_info.biHeight);
    SwapEndian(&image_info.biBitCount);

    if (!(((header.bfType & 0xff) == 'M') &&
            (((header.bfType >> 8) & 0xff) == 'B') &&
#else
    if (!(((header.bfType & 0xff) == 'B') &&
            (((header.bfType >> 8) & 0xff) == 'M') &&
#endif
            (image_info.biBitCount == 24) &&
            (image_info.biWidth > 0) &&
            (image_info.biHeight > 0)))
        throw std::runtime_error("Image is not a bitmap");
    if (!((header.bfSize + image_info.biSize - header.bfOffBits) >= (image_info.biWidth * image_info.biHeight * 3)))
        throw std::runtime_error("Image size mismatch");
    padding = (image_info.biWidth * 3 + 3) & ~3;
    padding -= image_info.biWidth * 3;

    m_imagedata.Width = image_info.biWidth;
    m_imagedata.Height = image_info.biHeight;
    m_imagedata.Padding = padding;

    /// Allocate memory for the actual image.
    m_imagedata.Channels = 3;
    std::size_t bytes = image_info.biWidth * image_info.biHeight * m_imagedata.Channels + image_info.biHeight * padding;
    try {
        unsigned char* buffer = new unsigned char[bytes];
        fs.read(reinterpret_cast<char*>(buffer), bytes);
        m_imagedata.pData.assign(buffer, buffer+bytes);
        delete[] buffer;
    } catch (std::bad_alloc&)
    {
        throw std::runtime_error("Could not allocate memory for bitmap data");
    }
}

BitmapLoader::~BitmapLoader()
{
}

BitmapLoader::IMAGEDATA BitmapLoader::getImageData()
{
    return m_imagedata;
}

void BitmapLoader::swapRedBlue(IMAGEDATA &ImageData)
{
    long x,y;

    /// Does not support 4 channels.
    if (ImageData.Channels == 4)
        return;

    /// Do the swap
    std::size_t i = 0;
    for (y = 0; y < ImageData.Height; ++y)
    {
        for (x = 0; x < ImageData.Width; ++x)
        {
            std::swap(ImageData.pData[i], ImageData.pData[i+2]);
            i += 3;
            if (x == (ImageData.Width - 1))
                i += ImageData.Padding;
        }
    }
}



