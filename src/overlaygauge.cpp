// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, Inc.

#include "overlaygauge.h"
#include "basics.h"
#include <cstdio>
#include <cstring>
#include "XPLMGraphics.h"


using namespace PPL;

OverlayGauge::OverlayGauge(int left2d, int top2d, int width2d, int height2d, int left3d, int top3d, int width3d, int height3d, int textureId3d, bool is_visible2d, bool always_draw_3d):
    left_3d_(left3d),
    top_3d_(top3d),
    width_3d_(width3d),
    height_3d_(height3d),
    m_visible_2d(is_visible2d),
    m_always_draw_3d(always_draw_3d),
    m_screen_width("sim/graphics/view/window_width"),
    m_screen_height("sim/graphics/view/window_height"),
    m_view_type("sim/graphics/view/view_type"),
    m_click_3d_x("sim/graphics/view/click_3d_x_pixels"),// click_3d_x_pixels
    m_click_3d_y("sim/graphics/view/click_3d_y_pixels"),// click_3d_y_pixels
    m_panel_coord_l("sim/graphics/view/panel_total_pnl_l"),
    m_panel_coord_t("sim/graphics/view/panel_total_pnl_t"),
    m_panel_region_id_3d(textureId3d),
    m_call_counter(0),
    window_is_dragging_(false),
    window_has_keyboard_focus_(false)
{
    //XPLMRegisterDrawCallback(draw2dCallback, xplm_Phase_LastCockpit, 0, this);
    XPLMRegisterDrawCallback(draw3dCallback, xplm_Phase_Gauges, 0, this);
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
    m_window2d_id = XPLMCreateWindowEx(&win);

    win.left = 0;
    win.top = m_screen_height;
    win.right = m_screen_width;
    win.bottom = 0;
    win.visible = true;
    win.drawWindowFunc = draw3dWindowCallback;
    win.handleKeyFunc = handle3dKeyCallback;
    win.handleMouseClickFunc = handle3dClickCallback;
    win.handleCursorFunc = handle3dCursorCallback;
    m_window3d_click_harcevester_id = XPLMCreateWindowEx(&win);

    XPLMGenerateTextureNumbers((int*)(&textureId), 1);
    XPLMBindTexture2d(textureId, 0);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // automatic mipmap
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width3d, height3d, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, 0);

    XPLMBindTexture2d(0,0);

    // create a renderbuffer object to store depth info
    glGenRenderbuffersEXT(1, &rboId);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, rboId);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT,
                             width3d, height3d);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

    // create a framebuffer object
    glGenFramebuffersEXT(1, &fboId);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);

    // attach the texture to FBO color attachment point
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                              GL_TEXTURE_2D, textureId, 0);

    // attach the renderbuffer to depth attachment point
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
                                 GL_RENDERBUFFER_EXT, rboId);

    // check FBO status
    status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    if(status != GL_FRAMEBUFFER_COMPLETE_EXT)
        fboUsed = false;

    // switch back to window-system-provided framebuffer
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

}

OverlayGauge::~OverlayGauge()
{
    XPLMUnregisterDrawCallback(draw2dCallback, xplm_Phase_LastCockpit, 0, this);
    XPLMUnregisterDrawCallback(draw3dCallback, xplm_Phase_Gauges, 0, this);
    XPLMDestroyWindow(m_window2d_id);
    //    XPLMDestroyWindow(m_window3d_id);
}

void OverlayGauge::set3d(int left3d, int top3d, int width3d, int height3d, int texture_id, bool always_draw_3d)
{
    m_panel_region_id_3d = texture_id;
    m_always_draw_3d = always_draw_3d;

    left_3d_ = left3d;
    top_3d_ = top3d;
    width_3d_ = width3d;
    height_3d_ = height3d;

    //    if (m_window3d_id != 0)
    //        XPLMDestroyWindow(m_window3d_id);

    //    XPLMCreateWindow_t win;
    //    memset(&win, 0, sizeof(win));

    //    win.structSize = sizeof(win);

    //    win.left = left3d;
    //    win.top = top3d;
    //    win.right = left3d+width3d;
    //    win.bottom = top3d-height3d;
    //    win.visible = true;
    //    win.drawWindowFunc = draw3dWindowCallback;
    //    win.handleKeyFunc = handle3dKeyCallback;
    //    win.handleMouseClickFunc = handle3dClickCallback;
    //    win.handleCursorFunc = handle3dCursorCallback;
    //    m_window3d_id = XPLMCreateWindowEx(&win);
}

void OverlayGauge::disable3d()
{
    //    XPLMDestroyWindow(m_window3d_id);
    //    m_window3d_id = 0;
}

void OverlayGauge::setVisible(bool b)
{
    m_visible_2d = b;
}

bool OverlayGauge::isVisible() const
{
    return m_visible_2d;
}

int OverlayGauge::draw2dCallback(XPLMDrawingPhase, int)
{
    return 1;
}

void OverlayGauge::frame()
{
    m_call_counter = 0;
}

int OverlayGauge::draw3dCallback(XPLMDrawingPhase, int)
{
    if (m_view_type == 1026 || m_always_draw_3d)
    {
        /*float l = m_panel_coord_l;
        float t = m_panel_coord_t;*/
        m_call_counter++;
        if (/*m_window3d_id && */(m_panel_region_id_3d == -1 || m_call_counter == static_cast<unsigned int>(m_panel_region_id_3d)))
        {
            //int left, top, right, bottom;
            //XPLMGetWindowGeometry(m_window3d_id, &left, &top, &right, &bottom);
            draw(left_3d_, top_3d_, left_3d_+width_3d_, top_3d_ - height_3d_);
        }
    }
    return 1;
}

void OverlayGauge::draw2dWindowCallback(XPLMWindowID)
{
    if (m_visible_2d)
    {
        int left, top, right, bottom;
        XPLMGetWindowGeometry(m_window2d_id, &left, &top, &right, &bottom);
        // set rendering destination to FBO
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);

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
        bindTex(textureId,0);
        glGenerateMipmapEXT(GL_TEXTURE_2D);

        setDrawState(0/*Fog*/, 1/*TexUnits*/, 0/*Lighting*/, 0/*AlphaTesting*/, 1/*AlphaBlending*/, 0/*DepthTesting*/, 0/*DepthWriting*/);
        glColor4f(1,1,1,1);

        glBegin(GL_QUADS);
        glTexCoord2f(0, 1);  glVertex2f(left, top);
        glTexCoord2f(1, 1);  glVertex2f(right, top);
        glTexCoord2f(1, 0);  glVertex2f(right, bottom);
        glTexCoord2f(0, 0);  glVertex2f(left, bottom);
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
    printf("2d key %c, flags %d, virtual_key %c, losing focus %d\n", key, flags, virtual_key, losing_focus);
}

void OverlayGauge::handle3dKeyCallback(XPLMWindowID, char key, XPLMKeyFlags flags, char virtual_key, int losing_focus)
{
    printf("3d key %c, flags %d, virtual_key %c, losing focus %d\n", key, flags, virtual_key, losing_focus);
}

int OverlayGauge::handle2dClickCallback(XPLMWindowID window_id, int x, int y, XPLMMouseStatus mouse)
{
    //printf("mouse (%d,%d)\n",x,y);
    static int dX = 0, dY = 0;
    static int Weight = 0, Height = 0;
    int Left, Top, Right, Bottom;

    if (!m_visible_2d)
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
                XPLMTakeKeyboardFocus(0);
            else
                XPLMTakeKeyboardFocus(window_id);
            window_has_keyboard_focus_ = !window_has_keyboard_focus_;
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
    printf("%f, %f\n", (float)m_click_3d_x, (float)m_click_3d_y);
    return 0;
}

XPLMCursorStatus OverlayGauge::handle2dCursorCallback(XPLMWindowID, int, int)
{
    if (m_visible_2d)
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

void OverlayGauge::setDrawState(int inEnableFog, int inNumberTexUnits, int inEnableLighting, int inEnableAlphaTesting, int inEnableAlphaBlending, int inEnableDepthTesting, int inEnableDepthWriting)
{
    XPLMSetGraphicsState(inEnableFog, inNumberTexUnits, inEnableLighting, inEnableAlphaTesting, inEnableAlphaBlending, inEnableDepthTesting, inEnableDepthWriting);
}

void OverlayGauge::bindTex(int tex_id, int texture_unit)
{
    XPLMBindTexture2d(tex_id, texture_unit);
}

bool OverlayGauge::coordInRect(float x, float y, float l, float t, float r, float b)
{
    return ((x >= l) && (x < r) && (y < t) && (y >= b));
}
