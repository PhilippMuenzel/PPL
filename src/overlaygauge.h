// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, Inc.

#ifndef OVERLAYWINDOW_H
#define OVERLAYWINDOW_H

#include "XPLMDisplay.h"
#include "dataref.h"

#if APL == 1
#include <OpenGL/OpenGL.h>
#include <OpenGL/glu.h>
#elif IBM == 1
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "GL/glew.h"
#include <gl/gl.h>
#include <gl/glu.h>
#elif LIN == 1
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include "namespaces.h"

namespace PPLNAMESPACE {

class OverlayGauge
{
public:
    OverlayGauge(int left2d, int top2d, int width2d, int height2d, int left3d, int top3d, int width3d, int height3d,
                 int frameOffX, int frameOffY, int textureId3d, bool allow_keyboard = true, bool is_visible3d = true, bool is_visible2d = false, bool always_draw_3d = false, bool allow_3d_click = true, float scale_3d = 1.0f, bool double_size = false);
    virtual ~OverlayGauge();

    void set3d(int left3d, int top3d, int width3d, int height3d, int texture_id, bool always_draw_3d);
    void setCopy3d(int left3dcopy, int top3dcopy);
    void disable3d();
    bool isVisible() const;
    void setVisible(bool b);
    void frame();
    int draw2dCallback(XPLMDrawingPhase phase, int is_before);
    int draw3dCallback(XPLMDrawingPhase phase, int is_before);
    void draw2dWindowCallback(XPLMWindowID window_id);
    void draw3dWindowCallback(XPLMWindowID window_id);

    void handle2dKeyCallback(XPLMWindowID window_id, char key, XPLMKeyFlags flags, char virtual_key, int losing_focus);
    void handle3dKeyCallback(XPLMWindowID window_id, char key, XPLMKeyFlags flags, char virtual_key, int losing_focus);

    int handle2dClickCallback(XPLMWindowID window_id, int x, int y, XPLMMouseStatus mouse);
    int handle3dClickCallback(XPLMWindowID window_id, int x, int y, XPLMMouseStatus mouse);

    XPLMCursorStatus handle2dCursorCallback(XPLMWindowID window_id, int x, int y);
    XPLMCursorStatus handle3dCursorCallback(XPLMWindowID window_id, int x, int y);

    virtual bool wantRedraw();
    virtual void draw(int left, int top, int right, int bottom) = 0;
    virtual void handleNonDragClick(int x_rel, int y_rel) = 0;
    virtual void handleNonDragClickRelease(int x_rel, int y_rel);
    virtual void handleKeyPress(char key, XPLMKeyFlags flags, char virtual_key) = 0;
    virtual int  frameTextureId() const;
    virtual void drawFrameTexture(int, int, int, int);
    virtual bool wantClearTexture();
    virtual bool frameIsBackground();

    static int draw2dCallback(XPLMDrawingPhase phase, int is_before, void* refcon);
    static int draw3dCallback(XPLMDrawingPhase phase, int is_before, void* refcon);
    static void draw2dWindowCallback(XPLMWindowID window_id, void* refcon);
    static void draw3dWindowCallback(XPLMWindowID window_id, void* refcon);

    static void handle2dKeyCallback(XPLMWindowID window_id, char key, XPLMKeyFlags flags, char virtual_key, void* refcon, int losing_focus);
    static void handle3dKeyCallback(XPLMWindowID window_id, char key, XPLMKeyFlags flags, char virtual_key, void* refcon, int losing_focus);

    static int handle2dClickCallback(XPLMWindowID window_id, int x, int y, XPLMMouseStatus mouse, void* refcon);
    static int handle3dClickCallback(XPLMWindowID window_id, int x, int y, XPLMMouseStatus mouse, void* refcon);

    static XPLMCursorStatus handle2dCursorCallback(XPLMWindowID window_id, int x, int y, void* refcon);
    static XPLMCursorStatus handle3dCursorCallback(XPLMWindowID window_id, int x, int y, void* refcon);

    static float frameCallback(float, float, int, void* refcon);

    void setDrawState(int inEnableFog,
                      int inNumberTexUnits,
                      int inEnableLighting,
                      int inEnableAlphaTesting,
                      int inEnableAlphaBlending,
                      int inEnableDepthTesting,
                      int inEnableDepthWriting);
    void bindTex(int tex_id, int texture_unit);
    void generateTex(int* tex_id, int number_of_textures);

    static bool coordInRect(float x, float y, float l, float t, float r, float b);
    void drawTexture(int tex_id, int left, int top, int right, int bottom);

private:
    XPLMWindowID window2d_id_;
    XPLMWindowID window3d_click_harcevester_id_;
    int left_3d_;
    int top_3d_;
    int width_2d_;
    int height_2d_;
    int width_3d_;
    int height_3d_;
    int frame_off_x_;
    int frame_off_y_;
    bool visible_2d_;
    bool visible_3d_;
    bool always_draw_3d_;
    bool allow_keyboard_grab_;
    bool allow_3d_click_;
    float scale_3d_;
    bool doubled_size_;
    int width_view_3d_;
    int height_view_3d_;
    DataRef<int> screen_width_;
    DataRef<int> screen_height_;
    DataRef<int> view_type_;
    DataRef<float> click_3d_x_;
    DataRef<float> click_3d_y_;
    int panel_region_id_3d_;
    unsigned int region_draw_counter_;
    bool window_is_dragging_;
    bool window_has_keyboard_focus_;
    GLuint gauge_texture_;
    GLuint rbo_;
    GLuint fbo_;
    int copy_left_3d_;
    int copy_top_3d_;
};

}

#endif // OVERLAYWINDOW_H
