// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, Inc.

#include "overlaygauge.h"
#include "basics.h"
#include <cstdio>
#include <cstring>
#include "XPLMGraphics.h"
#include "XPLMProcessing.h"


using namespace PPL;

OverlayGauge::OverlayGauge(int left2d, int top2d, int width2d, int height2d, int left3d, int top3d, int width3d, int height3d,
                           int frameOffX, int frameOffY, int textureId3d, bool is_visible3d, bool is_visible2d, bool always_draw_3d):
    left_3d_(left3d),
    top_3d_(top3d),
    width_2d_(width2d),
    height_2d_(height2d),
    width_3d_(width3d),
    height_3d_(height3d),
    frame_off_x_(frameOffX),
    frame_off_y_(frameOffY),
    visible_2d_(is_visible2d),
    visible_3d_(is_visible3d),
    always_draw_3d_(always_draw_3d),
    screen_width_("sim/graphics/view/window_width"),
    screen_height_("sim/graphics/view/window_height"),
    view_type_("sim/graphics/view/view_type"),
    click_3d_x_("sim/graphics/view/click_3d_x_pixels"),
    click_3d_y_("sim/graphics/view/click_3d_y_pixels"),
    panel_region_id_3d_(textureId3d),
    region_draw_counter_(0),
    window_is_dragging_(false),
    window_has_keyboard_focus_(false)
{
    XPLMRegisterDrawCallback(draw3dCallback, xplm_Phase_Gauges, 0, this);
    XPLMRegisterFlightLoopCallback(frameCallback, -1, this);

    XPLMCreateWindow_t win;
    memset(&win, 0, sizeof(win));

    win.structSize = sizeof(win);
    win.left = left2d;
    win.top = top2d;
    win.right = left2d+width2d;
    win.bottom = top2d-height2d;
    win.visible = is_visible2d;
    win.drawWindowFunc = draw2dWindowCallback;
    win.handleKeyFunc = handle2dKeyCallback;
    win.handleMouseClickFunc = handle2dClickCallback;
    win.handleCursorFunc = handle2dCursorCallback;
    win.refcon = this;
    window2d_id_ = XPLMCreateWindowEx(&win);

    memset(&win, 0, sizeof(win));
    win.structSize = sizeof(win);
    win.left = 0;
    win.top = screen_height_;
    win.right = screen_width_;
    win.bottom = 0;
    win.visible = true;
    win.drawWindowFunc = draw3dWindowCallback;
    win.handleKeyFunc = handle3dKeyCallback;
    win.handleMouseClickFunc = handle3dClickCallback;
    win.handleCursorFunc = handle3dCursorCallback;
    win.refcon = this;
    window3d_click_harcevester_id_ = XPLMCreateWindowEx(&win);

    generateTex((int*)(&gauge_texture_), 1);
    bindTex(gauge_texture_, 0);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // automatic mipmap
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width3d, height3d, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, 0);

    // create a renderbuffer object to store depth info
    glGenRenderbuffersEXT(1, &rbo_);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, rbo_);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT,
                             width3d, height3d);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

    // create a framebuffer object
    glGenFramebuffersEXT(1, &fbo_);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_);

    // attach the texture to FBO color attachment point
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                              GL_TEXTURE_2D, gauge_texture_, 0);

    // attach the renderbuffer to depth attachment point
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
                                 GL_RENDERBUFFER_EXT, rbo_);

    // check FBO status
    status_ = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    if(status_ != GL_FRAMEBUFFER_COMPLETE_EXT)
        fbo_used_ = false;

    // switch back to window-system-provided framebuffer
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

}

OverlayGauge::~OverlayGauge()
{
    XPLMUnregisterDrawCallback(draw3dCallback, xplm_Phase_Gauges, 0, this);
    XPLMDestroyWindow(window2d_id_);
    XPLMDestroyWindow(window3d_click_harcevester_id_);
    XPLMUnregisterFlightLoopCallback(frameCallback, this);
}

void OverlayGauge::set3d(int left3d, int top3d, int width3d, int height3d, int texture_id, bool always_draw_3d)
{
    panel_region_id_3d_ = texture_id;
    always_draw_3d_ = always_draw_3d;

    left_3d_ = left3d;
    top_3d_ = top3d;
    width_3d_ = width3d;
    height_3d_ = height3d;

    visible_3d_ = true;
}

void OverlayGauge::disable3d()
{
    visible_3d_ = false;
}

void OverlayGauge::setVisible(bool b)
{
    visible_2d_ = b;
}

bool OverlayGauge::isVisible() const
{
    return visible_2d_;
}

int OverlayGauge::draw2dCallback(XPLMDrawingPhase, int)
{
    return 1;
}

void OverlayGauge::frame()
{
    region_draw_counter_ = 0;
}

int OverlayGauge::draw3dCallback(XPLMDrawingPhase, int)
{
    if (view_type_ == 1026 || always_draw_3d_)
    {
        region_draw_counter_++;
        if (visible_3d_ && (panel_region_id_3d_ == -1 || region_draw_counter_ == static_cast<unsigned int>(panel_region_id_3d_)))
        {
            bindTex(gauge_texture_, 0);

            setDrawState(0/*Fog*/, 1/*TexUnits*/, 0/*Lighting*/, 0/*AlphaTesting*/, 1/*AlphaBlending*/, 0/*DepthTesting*/, 0/*DepthWriting*/);
            glColor4f(1,1,1,1);

            glBegin(GL_QUADS);
            glTexCoord2f(0, 1);  glVertex2f(left_3d_,           top_3d_);
            glTexCoord2f(1, 1);  glVertex2f(left_3d_+width_3d_, top_3d_);
            glTexCoord2f(1, 0);  glVertex2f(left_3d_+width_3d_, top_3d_ - height_3d_);
            glTexCoord2f(0, 0);  glVertex2f(left_3d_,           top_3d_ - height_3d_);
            glEnd();
        }
    }
    return 1;
}

void OverlayGauge::draw2dWindowCallback(XPLMWindowID)
{
    // set rendering destination to FBO
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_);

    // clear buffers
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw a scene to a texture directly
    draw(0, height_3d_, width_3d_, 0);

    // unbind FBO
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    // trigger mipmaps generation explicitly
    // NOTE: If GL_GENERATE_MIPMAP is set to GL_TRUE, then glCopyTexSubImage2D()
    // triggers mipmap generation automatically. However, the texture attached
    // onto a FBO should generate mipmaps manually via glGenerateMipmapEXT().
    bindTex(gauge_texture_,0);
    glGenerateMipmapEXT(GL_TEXTURE_2D);
    if (visible_2d_)
    {
        int left, top, right, bottom;
        XPLMGetWindowGeometry(window2d_id_, &left, &top, &right, &bottom);

        bindTex(gauge_texture_, 0);
        setDrawState(0/*Fog*/, 1/*TexUnits*/, 0/*Lighting*/, 0/*AlphaTesting*/, 1/*AlphaBlending*/, 0/*DepthTesting*/, 0/*DepthWriting*/);
        glColor4f(1,1,1,1);

        glBegin(GL_QUADS);
        glTexCoord2f(0, 1);  glVertex2f(left+frame_off_x_,           top-frame_off_y_);
        glTexCoord2f(1, 1);  glVertex2f(left+frame_off_x_+width_3d_, top-frame_off_y_);
        glTexCoord2f(1, 0);  glVertex2f(left+frame_off_x_+width_3d_, top-frame_off_y_-height_3d_);
        glTexCoord2f(0, 0);  glVertex2f(left+frame_off_x_,           top-frame_off_y_-height_3d_);
        glEnd();

        drawFrameTexture(left, top, right, bottom);

        if (window_has_keyboard_focus_)
        {
            static float color[] = { 1.f, 0.5f, 0.f};
            static char str[] = "K";
            XPLMDrawString(color, left + 20, top - 25, str, 0, xplmFont_Basic);
        }
    }
}

void OverlayGauge::draw3dWindowCallback(XPLMWindowID)
{
}

void OverlayGauge::handle2dKeyCallback(XPLMWindowID, char key, XPLMKeyFlags flags, char virtual_key, int losing_focus)
{
    if (losing_focus)
        window_has_keyboard_focus_ = false;
    printf("2d key %c, flags %d, virtual_key %c, losing focus %d\n", key, flags, virtual_key, losing_focus);
    handleKeyPress(key, flags, virtual_key);
}

void OverlayGauge::handle3dKeyCallback(XPLMWindowID, char, XPLMKeyFlags, char, int)
{
}

int OverlayGauge::handle2dClickCallback(XPLMWindowID window_id, int x, int y, XPLMMouseStatus mouse)
{
    //printf("mouse (%d,%d)\n",x,y);
    static int dX = 0, dY = 0;
    static int Weight = 0, Height = 0;
    int Left, Top, Right, Bottom;

    if (!visible_2d_)
        return 0;

    /// Get the windows current position
    XPLMGetWindowGeometry(window_id, &Left, &Top, &Right, &Bottom);
    int x_rel = x - Left;
    int y_rel = y - Bottom;
    switch(mouse) {
    case xplm_MouseDown:
        /// Test for the mouse in the window
        if (coordInRect(x, y, Left, Top, Left+50, Top-50))
        {
            if (window_has_keyboard_focus_)
            {
                XPLMTakeKeyboardFocus(0);
                window_has_keyboard_focus_ = false;
            } else
            {
                XPLMTakeKeyboardFocus(window_id);
                window_has_keyboard_focus_ = true;
            }
        }
        if (coordInRect(x, y, Left+50, Top, Right-50, Top-50))
        {
            dX = x - Left;
            dY = y - Top;
            Weight = Right - Left;
            Height = Bottom - Top;
            window_is_dragging_ = true;
        }
        if (coordInRect(x, y, Right-50, Top, Right, Top-50))
        {
            XPLMTakeKeyboardFocus(0);
            window_has_keyboard_focus_ = false;
            setVisible(false);
        }
        if (coordInRect(x, y, Left, Top-50, Right, Bottom))
        {
            handleNonDragClick(x_rel, y_rel);
        }
        break;
    case xplm_MouseDrag:
        /// We are dragging so update the window position
        if (window_is_dragging_)
        {
            Left = (x - dX);
            Right = Left + Weight;
            Top = (y - dY);
            Bottom = Top + Height;
            XPLMSetWindowGeometry(window_id, Left, Top, Right, Bottom);
        }
        break;
    case xplm_MouseUp:
        window_is_dragging_ = false;
        break;
    }
    return 1;
}

int OverlayGauge::handle3dClickCallback(XPLMWindowID, int, int, XPLMMouseStatus)
{
    if (panel_region_id_3d_ != -1)
        return 0;
    float x = click_3d_x_;
    float y = click_3d_y_;
    if (coordInRect(x, y, left_3d_, top_3d_, left_3d_+width_3d_, top_3d_-height_3d_))
    {
        visible_2d_ = !visible_2d_;
        return 1;
    }
    return 0;
}

XPLMCursorStatus OverlayGauge::handle2dCursorCallback(XPLMWindowID, int, int)
{
    if (visible_2d_)
        return xplm_CursorArrow;
    else
        return xplm_CursorDefault;
}

XPLMCursorStatus OverlayGauge::handle3dCursorCallback(XPLMWindowID, int, int)
{
    return xplm_CursorDefault;
}

int OverlayGauge::frameTextureId() const
{
    return 0;
}

void OverlayGauge::drawFrameTexture(int left, int top, int right, int bottom)
{
    int tex_id = frameTextureId();
    if (tex_id > 0)
    {
        setDrawState(0/*Fog*/, 1/*TexUnits*/, 0/*Lighting*/, 0/*AlphaTesting*/, 1/*AlphaBlending*/, 0/*DepthTesting*/, 0/*DepthWriting*/);
        glColor4f(1,1,1,1);

        bindTex(tex_id, 0);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 1);  glVertex2f(left, top);
        glTexCoord2f(1, 1);  glVertex2f(right, top);
        glTexCoord2f(1, 0);  glVertex2f(right, bottom);
        glTexCoord2f(0, 0);  glVertex2f(left, bottom);
        glEnd();
    }
}

int OverlayGauge::draw2dCallback(XPLMDrawingPhase phase, int is_before, void* refcon)
{
    OverlayGauge* window = static_cast<OverlayGauge*>(refcon);
    return window->draw2dCallback(phase, is_before);
}

int OverlayGauge::draw3dCallback(XPLMDrawingPhase phase, int is_before, void* refcon)
{
    OverlayGauge* window = static_cast<OverlayGauge*>(refcon);
    return window->draw3dCallback(phase, is_before);
}

void OverlayGauge::draw2dWindowCallback(XPLMWindowID window_id, void* refcon)
{
    OverlayGauge* window = static_cast<OverlayGauge*>(refcon);
    window->draw2dWindowCallback(window_id);
}

void OverlayGauge::draw3dWindowCallback(XPLMWindowID window_id, void* refcon)
{
    OverlayGauge* window = static_cast<OverlayGauge*>(refcon);
    window->draw3dWindowCallback(window_id);
}

void OverlayGauge::handle2dKeyCallback(XPLMWindowID window_id, char key, XPLMKeyFlags flags, char virtual_key, void* refcon, int losing_focus)
{
    OverlayGauge* window = static_cast<OverlayGauge*>(refcon);
    window->handle2dKeyCallback(window_id, key, flags, virtual_key, losing_focus);
}

void OverlayGauge::handle3dKeyCallback(XPLMWindowID window_id, char key, XPLMKeyFlags flags, char virtual_key, void* refcon, int losing_focus)
{
    OverlayGauge* window = static_cast<OverlayGauge*>(refcon);
    window->handle3dKeyCallback(window_id, key, flags, virtual_key, losing_focus);
}

int OverlayGauge::handle2dClickCallback(XPLMWindowID window_id, int x, int y, XPLMMouseStatus mouse, void* refcon)
{
    OverlayGauge* window = static_cast<OverlayGauge*>(refcon);
    return window->handle2dClickCallback(window_id, x, y, mouse);
}

int OverlayGauge::handle3dClickCallback(XPLMWindowID window_id, int x, int y, XPLMMouseStatus mouse, void* refcon)
{
    OverlayGauge* window = static_cast<OverlayGauge*>(refcon);
    return window->handle3dClickCallback(window_id, x, y, mouse);
}

XPLMCursorStatus OverlayGauge::handle2dCursorCallback(XPLMWindowID window_id, int x, int y, void* refcon)
{
    OverlayGauge* window = static_cast<OverlayGauge*>(refcon);
    return window->handle2dCursorCallback(window_id, x, y);
}

XPLMCursorStatus OverlayGauge::handle3dCursorCallback(XPLMWindowID window_id, int x, int y, void* refcon)
{
    OverlayGauge* window = static_cast<OverlayGauge*>(refcon);
    return window->handle3dCursorCallback(window_id, x, y);
}

float OverlayGauge::frameCallback(float, float, int, void* refcon)
{
    OverlayGauge* window = static_cast<OverlayGauge*>(refcon);
    window->frame();
    return -1;
}

void OverlayGauge::setDrawState(int inEnableFog, int inNumberTexUnits, int inEnableLighting, int inEnableAlphaTesting, int inEnableAlphaBlending, int inEnableDepthTesting, int inEnableDepthWriting)
{
    XPLMSetGraphicsState(inEnableFog, inNumberTexUnits, inEnableLighting, inEnableAlphaTesting, inEnableAlphaBlending, inEnableDepthTesting, inEnableDepthWriting);
}

void OverlayGauge::bindTex(int tex_id, int texture_unit)
{
    XPLMBindTexture2d(tex_id, texture_unit);
}

void OverlayGauge::generateTex(int* tex_ids, int number_of_textures)
{
    XPLMGenerateTextureNumbers(tex_ids, number_of_textures);
}

bool OverlayGauge::coordInRect(float x, float y, float l, float t, float r, float b)
{
    return ((x >= l) && (x < r) && (y < t) && (y >= b));
}
