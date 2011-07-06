// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, Inc.

#include "overlaygauge.h"
#include "basics.h"
#include <cstdio>
#include <cstring>

using namespace PPL;

OverlayGauge::OverlayGauge(int left2d, int top2d, int width2d, int height2d, int left3d, int top3d, int width3d, int height3d, int textureId3d, bool is_visible2d):
    m_visible_2d(is_visible2d),
    m_screen_width("sim/graphics/view/window_width"),
    m_screen_height("sim/graphics/view/window_height"),
    m_view_type("sim/graphics/view/view_type"),
    m_click_3d_x("sim/graphics/view/click_3d_x"),
    m_click_3d_y("sim/graphics/view/click_3d_y"),
    m_panel_coord_l("sim/graphics/view/panel_total_pnl_l"),
    m_panel_coord_t("sim/graphics/view/panel_total_pnl_t"),
    m_panel_region_id_3d(textureId3d),
    m_call_counter(0)
{
    XPLMRegisterDrawCallback(draw2dCallback, xplm_Phase_LastCockpit, 0, this);
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

    win.left = left3d;
    win.top = top3d;
    win.right = left3d+width3d;
    win.bottom = top3d-height3d;
    win.visible = true;
    win.drawWindowFunc = draw3dWindowCallback;
    win.handleKeyFunc = handle3dKeyCallback;
    win.handleMouseClickFunc = handle3dClickCallback;
    win.handleCursorFunc = handle3dCursorCallback;
    m_window3d_id = XPLMCreateWindowEx(&win);
}

OverlayGauge::~OverlayGauge()
{
    XPLMUnregisterDrawCallback(draw2dCallback, xplm_Phase_LastCockpit, 0, this);
    XPLMUnregisterDrawCallback(draw3dCallback, xplm_Phase_Gauges, 0, this);
    XPLMDestroyWindow(m_window2d_id);
    XPLMDestroyWindow(m_window3d_id);
}

void OverlayGauge::set3d(int left3d, int top3d, int width3d, int height3d, int texture_id)
{
    m_panel_region_id_3d = texture_id;
    if (m_window3d_id != 0)
        XPLMDestroyWindow(m_window3d_id);
    m_window3d_id = XPLMCreateWindow(left3d, top3d, left3d+width3d, top3d-height3d, true, draw3dWindowCallback, handle3dKeyCallback, handle3dClickCallback, this);
}

void OverlayGauge::disable3d()
{
    XPLMDestroyWindow(m_window3d_id);
    m_window3d_id = 0;
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
    if (m_visible_2d)
    {
        int left, top, right, bottom;
        XPLMGetWindowGeometry(m_window2d_id, &left, &top, &right, &bottom);
        draw(left, top, right, bottom);
    }
    return 1;
}

void OverlayGauge::frame()
{
    m_call_counter = 0;
}

int OverlayGauge::draw3dCallback(XPLMDrawingPhase, int)
{
    if (m_view_type == 1026)
    {
        /*float l = m_panel_coord_l;
        float t = m_panel_coord_t;*/
        m_call_counter++;
        if (m_window3d_id && (m_panel_region_id_3d == -1 || m_call_counter == static_cast<unsigned int>(m_panel_region_id_3d)))
        {
            int left, top, right, bottom;
            XPLMGetWindowGeometry(m_window3d_id, &left, &top, &right, &bottom);
            draw(left, top, right, bottom);
        }
    }
    return 1;
}

void OverlayGauge::draw2dWindowCallback(XPLMWindowID)
{
}

void OverlayGauge::draw3dWindowCallback(XPLMWindowID)
{
}

void OverlayGauge::handle2dKeyCallback(XPLMWindowID, char, XPLMKeyFlags, char, int)
{
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

    static int gDragging = 0;

    if (!m_visible_2d)
        return 0;

    /// Get the windows current position
    XPLMGetWindowGeometry(window_id, &Left, &Top, &Right, &Bottom);
    int x_rel = (x-Left);
    int y_rel = (y-Bottom);
    switch(mouse) {
    case xplm_MouseDown:
        /// Test for the mouse in the window
        if (coordInRect(x, y, Left+50, Top, Right-50, Top-50))
        {
            dX = (x - Left);
            dY = (y - Top);
            Weight = Right - Left;
            Height = Bottom - Top;
            gDragging = 1;
        }
        if (coordInRect(x, y, Right-50, Top, Right, Top-50))
        {
            setVisible(!m_visible_2d);
        }
        if (coordInRect(x, y, Left, Top-50, Right, Bottom))
        {
            handleNonDragClick(x_rel, y_rel);
        }
        break;
    case xplm_MouseDrag:
        /// We are dragging so update the window position
        if (gDragging)
        {
            Left = (x - dX);
            Right = Left + Weight;
            Top = (y - dY);
            Bottom = Top + Height;
            XPLMSetWindowGeometry(window_id, Left, Top, Right, Bottom);
        }
        break;
    case xplm_MouseUp:
        gDragging = 0;
        break;
    }
    return 1;
}

int OverlayGauge::handle3dClickCallback(XPLMWindowID, int, int, XPLMMouseStatus)
{
    return 0;
}

XPLMCursorStatus OverlayGauge::handle2dCursorCallback(XPLMWindowID, int, int)
{
    return xplm_CursorArrow;
}

XPLMCursorStatus OverlayGauge::handle3dCursorCallback(XPLMWindowID, int, int)
{
    return xplm_CursorDefault;
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

bool OverlayGauge::coordInRect(float x, float y, float l, float t, float r, float b)
{
    return ((x >= l) && (x < r) && (y < t) && (y >= b));
}
