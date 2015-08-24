// Copyright (c) 2013, Philipp Muenzel mail@philippmuenzel.de
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// The views and conclusions contained in the software and documentation are those
// of the authors and should not be interpreted as representing official policies,
// either expressed or implied, of the FreeBSD Project.

#include "overlaygauge.h"
#include "basics.h"
#include <cstdio>
#include <cstring>
#include "XPLMGraphics.h"
#include "XPLMProcessing.h"
#include "XPLMUtilities.h"

using namespace PPLNAMESPACE;

OverlayGauge::OverlayGauge(int left2d, int top2d, int width2d, int height2d, int left3d, int top3d, int width3d, int height3d,
                           int frameOffX, int frameOffY, int textureId3d, bool allow_keyboard, bool is_visible3d, bool is_visible2d, bool always_draw_3d, bool allow_3d_click, float scale_3d, bool double_size):
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
    allow_keyboard_grab_(allow_keyboard),
    allow_3d_click_(allow_3d_click),
    scale_3d_(scale_3d),
    doubled_size_(double_size),
    width_view_3d_(width3d),
    height_view_3d_(height3d),
    screen_width_("sim/graphics/view/window_width"),
    screen_height_("sim/graphics/view/window_height"),
    view_type_("sim/graphics/view/view_type"),
    click_3d_x_("sim/graphics/view/click_3d_x_pixels"),
    click_3d_y_("sim/graphics/view/click_3d_y_pixels"),
    instrument_brightness_("sim/cockpit2/switches/instrument_brightness_ratio"),
    alpha_(1),
    panel_region_id_3d_(textureId3d),
    region_draw_counter_(0),
    window_is_dragging_(false),
    window_has_keyboard_focus_(false),
    copy_left_3d_(-1),
    copy_top_3d_(-1)
{
    // sim/graphics/view/panel_render_type
    // debug/texture_browser
    // panel texture
    // 0 - 0 = Attr_cockpit
    // atr_cockpit_Region albedo
    //  attr_cockpit_Region emissive
    int xplm_version;
    XPLMHostApplicationID host_app;
    XPLMGetVersions(&xplane_version_, &xplm_version, &host_app);

    if (double_size)
    {
        width_3d_ *= 2;
        height_3d_ *= 2;
    }
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
    win.handleMouseWheelFunc = handle2dWheelCallback;
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
    win.handleMouseWheelFunc = handle3dWheelCallback;
    win.refcon = this;
    window3d_click_harcevester_id_ = XPLMCreateWindowEx(&win);

    generateTex((int*)(&gauge_texture_), 1);
    bindTex(gauge_texture_, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_3d_, height_3d_, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // store whatever rbo and fbo X-Plane is using right now
    GLint xp_rbo, xp_fbo;
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &xp_rbo);
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &xp_fbo);

    // create a renderbuffer object to store depth info
    glGenRenderbuffersEXT(1, &rbo_);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, rbo_);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_STENCIL,
                             width_3d_, height_3d_);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, xp_rbo);

    // create a framebuffer object
    glGenFramebuffersEXT(1, &fbo_);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_);

    // attach the texture to FBO color attachment point
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                              GL_TEXTURE_2D, gauge_texture_, 0);

    // attach the renderbuffer to depth attachment point
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_STENCIL_ATTACHMENT,
                                 GL_RENDERBUFFER_EXT, rbo_);

    // check FBO status
    GLenum status_ = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    if(status_ != GL_FRAMEBUFFER_COMPLETE_EXT)
        throw std::runtime_error("No possible FBO, sorry");

    // switch back to window-system-provided framebuffer
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, xp_fbo);

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

    width_3d_ = (doubled_size_ ? 2*width3d : width3d);
    height_3d_ = (doubled_size_ ? 2*height3d : height3d);
    width_view_3d_ = width3d;
    height_view_3d_ = height3d;

    visible_3d_ = true;
}

void OverlayGauge::setCopy3d(int left3dcopy, int top3dcopy)
{
    copy_left_3d_ = left3dcopy;
    copy_top_3d_ = top3dcopy;
}

void OverlayGauge::disable3d()
{
    visible_3d_ = false;
}

void OverlayGauge::setVisible(bool b)
{
    visible_2d_ = b;
    if (!b)
    {
        XPLMTakeKeyboardFocus(0);
        window_has_keyboard_focus_ = false;
    }
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
    XPLMSetWindowIsVisible(window2d_id_,1);
    region_draw_counter_ = 0;
}

void OverlayGauge::drawTexture(int tex_id, int left, int top, int right, int bottom, float alpha, int blend, bool vflip)
{
    setDrawState(0/*Fog*/, 1/*TexUnits*/, 0/*Lighting*/, 0/*AlphaTesting*/, blend/*AlphaBlending*/, 0/*DepthTesting*/, 0/*DepthWriting*/);
    bindTex(tex_id, 0);
    GLfloat vertices[] = { left, top,
                           right, top,
                           right, bottom,
                           left, bottom };
    GLfloat colors[] = { 1,1,1,alpha,
                         1,1,1,alpha,
                         1,1,1,alpha,
                         1,1,1,alpha };
    GLfloat tex_coords[] = { 0, !vflip,
                             1, !vflip,
                             1, vflip,
                             0, vflip };

    if (xplane_version_ < 10000)
        glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, vertices);
    glTexCoordPointer(2, GL_FLOAT, 0, tex_coords);
    glColorPointer(4, GL_FLOAT, 0, colors);

    glDrawArrays(GL_QUADS, 0, 4);
    if (xplane_version_ < 10000)
        glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

int OverlayGauge::draw3dCallback(XPLMDrawingPhase, int)
{
    if (view_type_ == 1026 || always_draw_3d_)
    {
        region_draw_counter_++;
        if (visible_3d_ && (panel_region_id_3d_ == -1 || region_draw_counter_ == static_cast<unsigned int>(panel_region_id_3d_)))
        {
            drawTexture(gauge_texture_, left_3d_, top_3d_, left_3d_+width_view_3d_ * scale_3d_, top_3d_-height_view_3d_*scale_3d_, alpha_,1,wantVFlip());

            if (copy_top_3d_ > -1 && copy_left_3d_ > -1)
            {
                drawTexture(gauge_texture_, copy_left_3d_, copy_top_3d_, copy_left_3d_+width_view_3d_ * scale_3d_, copy_top_3d_-height_view_3d_*scale_3d_, alpha_,1,wantVFlip());
            }
        }
    }
    return 1;
}

float OverlayGauge::instrumentBrightness()
{
    return instrument_brightness_[0];
}

void OverlayGauge::draw2dWindowCallback(XPLMWindowID)
{
    alpha_ = instrumentBrightness();
    if (wantRedraw())
    {
        GLint xp_fbo;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &xp_fbo);
        // set rendering destination to FBO
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_);
        glPushAttrib(GL_VIEWPORT_BIT);
        glViewport(0,0,width_3d_, height_3d_);
        glMatrixMode (GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0,width_3d_, 0, height_3d_, 0, 1);
        glMatrixMode (GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        // clear buffers
        if (wantClearTexture())
            glClearColor(0,0,0,0);
        else
            glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw a scene to a texture directly
        if (xplane_version_ > 10000)
            glDisableClientState(GL_VERTEX_ARRAY);
        draw(0, height_3d_, width_3d_, 0);
        if (xplane_version_ > 10000)
            glEnableClientState(GL_VERTEX_ARRAY);

        // unbind FBO

        glPopMatrix();
        glMatrixMode (GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopAttrib();
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, xp_fbo);
    }

    if (visible_2d_)
    {
        int left, top, right, bottom;
        XPLMGetWindowGeometry(window2d_id_, &left, &top, &right, &bottom);

        if (frameIsBackground())
            drawFrameTexture(left, top, right, bottom);
        drawTexture(gauge_texture_, left+frame_off_x_, top-frame_off_y_, left+frame_off_x_+width_view_3d_, top -frame_off_y_-height_view_3d_, alpha_, (frameIsBackground() || wantClearTexture()), wantVFlip());
        if (!frameIsBackground())
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
    if (losing_focus) {
        window_has_keyboard_focus_ = false;
    } else
    {
        handleKeyPress(key, flags, virtual_key);
    }

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
            if (allow_keyboard_grab_)
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
        if (coordInRect(x, y, Left, Top-50, Right, Bottom))
        {
            handleNonDragClickRelease(x_rel, y_rel);
        }
        break;
    }
    return 1;
}

void OverlayGauge::handleNonDragClickRelease(int, int)
{
}

int OverlayGauge::handle3dClickCallback(XPLMWindowID, int, int, XPLMMouseStatus mouse)
{
    if (!allow_3d_click_)
        return 0;
    if (panel_region_id_3d_ != -1)
        return 0;
    if (!visible_3d_)
        return 0;
    if(mouse == xplm_MouseDown)
    {
        float x = click_3d_x_;
        float y = click_3d_y_;
        if (coordInRect(x, y, left_3d_, top_3d_, left_3d_+width_3d_, top_3d_-height_3d_))
        {
            visible_2d_ = !visible_2d_;
            return 1;
        }
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

int OverlayGauge::handle2dWheelCallback(XPLMWindowID inWindowID, int x, int y, int wheel, int clicks)
{
    if (visible_2d_)
    {
        int left, right, top, bottom;
        XPLMGetWindowGeometry(inWindowID, &left, &top, &right, &bottom);
        return handleMouseWheel(x-left, y-bottom, wheel, clicks);
    }
    return 0;
}

int OverlayGauge::handle3dWheelCallback(XPLMWindowID inWindowID, int x, int y, int wheel, int clicks)
{
    return 0;
}

bool OverlayGauge::wantRedraw()
{
    return true;
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
        drawTexture(tex_id, left, top, right, bottom);
    }
}

bool OverlayGauge::wantClearTexture()
{
    return false;
}

bool OverlayGauge::frameIsBackground()
{
    return false;
}

bool OverlayGauge::wantVFlip()
{
    return false;
}

int OverlayGauge::handleMouseWheel(int, int, int, int)
{
    return 1;
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

int OverlayGauge::handle2dWheelCallback(XPLMWindowID window_id, int x, int y, int wheel, int clicks, void *refcon)
{
    OverlayGauge* window = static_cast<OverlayGauge*>(refcon);
    return window->handle2dWheelCallback(window_id, x, y, wheel, clicks);
}

int OverlayGauge::handle3dWheelCallback(XPLMWindowID window_id, int x, int y, int wheel, int clicks, void *refcon)
{
    OverlayGauge* window = static_cast<OverlayGauge*>(refcon);
    return window->handle3dWheelCallback(window_id, x, y, wheel, clicks);
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
