// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, Inc.

#ifndef FONT_H
#define FONT_H

#include <vector>
#include <string>
#include <stdexcept>

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

#if APL == 1
#include <OpenGL/OpenGL.h>
#include <OpenGL/glu.h>
#elif IBM == 1
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#elif LIN == 1
#include <GL/gl.h>
#include <GL/glu.h>
#endif

namespace PPL {

class Font {
public:
    Font(const std::string& fname, unsigned int height);
    ~Font();

private:
    friend void glPrint(const Font &font, float x, float y, const std::string &text);

    void make_dlist ( FT_Face face, char ch, GLuint list_base, GLuint * tex_base );
    int next_p2 ( int a );

    float h;            //!< Holds the height of the font.
    GLuint* textures;   //!< Holds the texture ids
    GLuint  list_base;  //!< Holds the first display list id

};


void glPrint(const Font& font, float x, float y, const std::string& text);

}

#endif // FONT_H
