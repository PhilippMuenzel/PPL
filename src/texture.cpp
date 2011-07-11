// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, Inc.

#include <fstream>
#include <stdexcept>
#include <cstring>
#include "texture.h"
#ifndef BUILD_FOR_STANDALONE
#include "XPLMUtilities.h"
#include "XPLMGraphics.h"
#endif

#if IBM
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
#include <gl\gl.h>
#include <gl\glu.h>
#elif LIN
#include <GL/gl.h>
#include <GL/glu.h>
#else
#if __GNUC__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <gl.h>
#include <glu.h>
#endif
#endif

using namespace PPL;

#if APL && defined(__ppc__)
short Endian(short Data)
{
    unsigned char *pBuffer = (unsigned char *)&Data;
    short Result = (short)(pBuffer[0] & 0xff) + ( (short)(pBuffer[1] & 0xff) << 8) ;
    return(Result);
}

long Endian(long Data)
{
    unsigned char *pBuffer = (unsigned char *)&Data;

    long Result =     (long)(pBuffer[0] & 0xff)
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
/// Only the ppc mac needs these so dummy functions for x86.
void SwapEndian(short *){}
void SwapEndian(long *){}
#endif


Texture::Texture(const std::string& file_name)
{
    if (file_name.rfind(".bmp") != std::string::npos)
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
            m_imagedata.pData.resize(bytes);

            fs.read(reinterpret_cast<char*>(&m_imagedata.pData[0]), bytes);


        } catch (std::bad_alloc&)
        {
            throw std::runtime_error("Could not allocate memory for bitmap data");
        }

        swapRedBlue();


#ifdef BUILD_FOR_STANDALONE
        glGenTextures(1, (GLuint*)&m_id);
        glBindTexture(GL_TEXTURE_2D, m_id);
#else
        /// Do the opengl stuff using XPLM functions for a friendly Xplane existence.
        XPLMGenerateTextureNumbers(&m_id, 1);
        XPLMBindTexture2d(m_id, 0);
#endif


        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, m_imagedata.Width, m_imagedata.Height, GL_RGB, GL_UNSIGNED_BYTE, &m_imagedata.pData[0]);

        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    } else if (file_name.rfind(".tga") != std::string::npos)
    {
        GLubyte TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};    // Uncompressed TGA Header
        GLubyte TGAcompare[12];                             // Used To Compare TGA Header
        GLubyte header[6];                                  // First 6 Useful Bytes From The Header
        GLuint  bytesPerPixel;                              // Holds Number Of Bytes Per Pixel Used In The TGA File
        GLuint  imageSize;                                  // Used To Store The Image Size When Setting Aside Ram
        GLuint  temp;                                       // Temporary Variable
        GLuint        type=GL_RGBA;                         // Set The Default GL Mode To RBGA (32 BPP)


        FILE *file = fopen(file_name.c_str(), "rb");        // Open The TGA File

        if(    file==NULL ||                                                        // Does File Even Exist?
                fread(TGAcompare,1,sizeof(TGAcompare),file)!=sizeof(TGAcompare) ||  // Are There 12 Bytes To Read?
                memcmp(TGAheader,TGAcompare,sizeof(TGAheader))!=0                || // Does The Header Match What We Want?
                fread(header,1,sizeof(header),file)!=sizeof(header))                // If So Read Next 6 Header Bytes
        {
            if (file == NULL)                               // Did The File Even Exist? *Added Jim Strong*
                throw std::runtime_error("File could not be openend");
            else                                            // Otherwise
            {
                fclose(file);                               // If Anything Failed, Close The File
                throw std::runtime_error("Images is not a valid TGA file");
            }
        }

        m_imagedata.Width  = header[1] * 256 + header[0];   // Determine The TGA Width    (highbyte*256+lowbyte)
        m_imagedata.Height = header[3] * 256 + header[2];   // Determine The TGA Height    (highbyte*256+lowbyte)

        if(    m_imagedata.Width    <=0    ||               // Is The Width Less Than Or Equal To Zero
                m_imagedata.Height    <=0    ||             // Is The Height Less Than Or Equal To Zero
                (header[4]!=24 && header[4]!=32))           // Is The TGA 24 or 32 Bit?
        {
            fclose(file);                                   // If Anything Failed, Close The File
            throw std::runtime_error("File is not a vaid TGA file");
        }

        m_imagedata.bpp = header[4];                        // Grab The TGA's Bits Per Pixel (24 or 32)
        bytesPerPixel   = m_imagedata.bpp/8;                // Divide By 8 To Get The Bytes Per Pixel
        imageSize       = m_imagedata.Width*m_imagedata.Height*bytesPerPixel;   // Calculate The Memory Required For The TGA Data

        try {
            unsigned char* buffer = new unsigned char[imageSize];
            if (fread(reinterpret_cast<char*>(buffer), 1, imageSize, file) == imageSize)
            {
                m_imagedata.pData.assign(buffer, buffer+imageSize);
                delete[] buffer;
            } else {
                throw std::runtime_error("Image size of tga doesn't match");
            }
        } catch (std::bad_alloc&)
        {
            throw std::runtime_error("Could not allocate memory for tga data");
        }

        for(GLuint i=0; i<(imageSize); i+=bytesPerPixel)        // Loop Through The Image Data
        {                                                       // Swaps The 1st And 3rd Bytes ('R'ed and 'B'lue)
            temp = m_imagedata.pData[i];                        // Temporarily Store The Value At Image Data 'i'
            m_imagedata.pData[i] = m_imagedata.pData[i + 2];    // Set The 1st Byte To The Value Of The 3rd Byte
            m_imagedata.pData[i + 2] = temp;                    // Set The 3rd Byte To The Value In 'temp' (1st Byte Value)
        }

        fclose (file);
        if (m_imagedata.bpp==24)                                // Was The TGA 24 Bits
        {
            type=GL_RGB;                                        // If So Set The 'type' To GL_RGB
        }
#ifdef BUILD_FOR_STANDALONE
        glGenTextures(1, (GLuint*)&m_id);
        glBindTexture(GL_TEXTURE_2D, m_id);
#else
        /// Do the opengl stuff using XPLM functions for a friendly Xplane existence.
        XPLMGenerateTextureNumbers(&m_id, 1);
        XPLMBindTexture2d(m_id, 0);
#endif
        glTexImage2D(GL_TEXTURE_2D, 0, type, m_imagedata.Width, m_imagedata.Height, 0, type, GL_UNSIGNED_BYTE, &m_imagedata.pData[0]);

        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    } else {
        throw std::runtime_error("The texture file is neither a BMP nor a TGA. Other fileformats are not supported.");
    }
}

Texture::~Texture()
{
    glDeleteTextures(1, (GLuint*)&m_id);
}

int Texture::id() const
{
    return m_id;
}

int Texture::width() const
{
    return m_imagedata.Width;
}

int Texture::height() const
{
    return m_imagedata.Height;
}

void Texture::swapRedBlue()
{
    long x,y;

    /// Does not support 4 channels.
    if (m_imagedata.Channels == 4)
        return;

    /// Do the swap
    std::size_t i = 0;
    for (y = 0; y < m_imagedata.Height; ++y)
    {
        for (x = 0; x < m_imagedata.Width; ++x)
        {
            std::swap(m_imagedata.pData[i], m_imagedata.pData[i+2]);
            i += 3;
            if (x == (m_imagedata.Width - 1))
                i += m_imagedata.Padding;
        }
    }
}

