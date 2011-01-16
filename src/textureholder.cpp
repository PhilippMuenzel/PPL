// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, Inc.

#include <stdexcept>

#include "XPLMGraphics.h"
#include "XPLMUtilities.h"
#include "bitmaploader.h"
#include "textureholder.h"
#include "log.h"

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

TextureHolder::TextureHolder()
{
}

int TextureHolder::loadTexture(const std::string &filename)
{

    void *pImageData = 0;
    int sWidth, sHeight;

    int texture_number;

    /// Get the bitmap from the file
    try {
        BitmapLoader loader(filename);
        BitmapLoader::IMAGEDATA sImageData = loader.getImageData();

        BitmapLoader::swapRedBlue(sImageData);
        pImageData = &sImageData.pData[0];

        /// Do the opengl stuff using XPLM functions for a friendly Xplane existence.
        sWidth=sImageData.Width;
        sHeight=sImageData.Height;
        XPLMGenerateTextureNumbers(&texture_number, 1);
        m_textures.push_back(texture_number);
        XPLMBindTexture2d(texture_number, 0);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, sWidth, sHeight, GL_RGB, GL_UNSIGNED_BYTE, pImageData);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    } catch (std::runtime_error& ex) {
        std::string fn(filename);
        fn.append(ex.what()).append("\n");
        XPLMDebugString(fn.c_str());
    }

    return texture_number;
}
