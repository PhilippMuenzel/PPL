// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, Inc.

#ifndef FONT_H
#define FONT_H

#include <vector>
#include <string>
#include <stdexcept>

#include "ft2build.h"
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"
#include "freetype/ftoutln.h"
#include "freetype/fttrigon.h"

#if APL == 1
#include <OpenGL/OpenGL.h>
#include <OpenGL/glu.h>
#elif IBM == 1
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/glew.h>
#elif LIN == 1
#include <GL/glew.h>
#endif

#include "namespaces.h"

namespace PPLNAMESPACE {

class Font {
public:
    Font(const std::string& fname, unsigned int height, int try_unicode_to = 62475);
    ~Font();

    template<typename T>
    void glPrint(float x, float y, const std::basic_string<T>& text);

private:

    template<typename T>
    void callList(const std::basic_string<T>& text);

    void make_dlist ( FT_Face face, wchar_t ch, GLuint list_base, GLuint * tex_base );
    int next_p2 ( int a );

    float h;            //!< Holds the height of the font.
    GLuint* textures;   //!< Holds the texture ids
    GLuint  list_base;  //!< Holds the first display list id
    int num_glyphs_;
};

template<>
void Font::callList<char>(const std::string& text);

template<>
void Font::callList<wchar_t>(const std::wstring& text);

template<typename T>
void Font::glPrint(float x, float y, const std::basic_string<T>& text)
{
    glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT  | GL_ENABLE_BIT | GL_TRANSFORM_BIT);

    glListBase(list_base);

    //float modelview_matrix[16];
    //glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);

    glPushMatrix();
    //glLoadIdentity();
    //    glColor4fv(color);
    glTranslatef(x,y,0);
    //glMultMatrixf(modelview_matrix);
    callList(text);
    glPopMatrix();

    glPopAttrib();
}

}

#endif // FONT_H
