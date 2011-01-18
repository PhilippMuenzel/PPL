// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, Inc.

#ifndef OVERLAYWINDOW_H
#define OVERLAYWINDOW_H

#include "XPLMDisplay.h"
#include "dataref.h"

namespace PPL {

class OverlayGauge
{
public:
    OverlayGauge(int left2d, int top2d, int width2d, int height2d, int left3d, int top3d, int width3d, int height3d, int textureId3d, bool is_visible2d = false);
    virtual ~OverlayGauge();

    void set3d(int left3d, int top3d, int width3d, int height3d, int texture_id);
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

    virtual void draw(int left, int top, int right, int bottom) = 0;
    virtual void handleNonDragClick(int no) = 0;

    static int draw2dCallback(XPLMDrawingPhase phase, int is_before, void* refcon);
    static int draw3dCallback(XPLMDrawingPhase phase, int is_before, void* refcon);
    static void draw2dWindowCallback(XPLMWindowID window_id, void* refcon);
    static void draw3dWindowCallback(XPLMWindowID window_id, void* refcon);

    static void handle2dKeyCallback(XPLMWindowID window_id, char key, XPLMKeyFlags flags, char virtual_key, void* refcon, int losing_focus);
    static void handle3dKeyCallback(XPLMWindowID window_id, char key, XPLMKeyFlags flags, char virtual_key, void* refcon, int losing_focus);

    static int handle2dClickCallback(XPLMWindowID window_id, int x, int y, XPLMMouseStatus mouse, void* refcon);
    static int handle3dClickCallback(XPLMWindowID window_id, int x, int y, XPLMMouseStatus mouse, void* refcon);


private:
    bool coordInRect(float x, float y, float l, float t, float r, float b);

private:
    XPLMWindowID m_window2d_id;
    XPLMWindowID m_window3d_id;
    bool m_visible_2d;
    DataRef<int> m_screen_width;
    DataRef<int> m_screen_height;
    DataRef<int> m_panel_render_mode;
    DataRef<float> m_click_3d_x;
    DataRef<float> m_click_3d_y;
    DataRef<float> m_panel_coord_l;
    DataRef<float> m_panel_coord_t;
    int m_texture_id_3d;
    unsigned int m_call_counter;
};

}

#endif // OVERLAYWINDOW_H
