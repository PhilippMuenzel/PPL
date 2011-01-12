// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU General Public License version 2 or later
// as published by the Free Software Foundation, Inc.

#ifndef OVERLAYWINDOW_H
#define OVERLAYWINDOW_H

#include "XPLMDisplay.h"
#include "dataref.h"

namespace PPL {

class OverlayWindow
{
public:
    OverlayWindow(int left, int top, int width, int height, bool is_visible = false);
    virtual ~OverlayWindow();
    void setVisible(bool b);
    int drawCallback(XPLMDrawingPhase phase, int is_before);
    void drawWindowCallback(XPLMWindowID window_id);
    void handleKeyCallback(XPLMWindowID window_id, char key, XPLMKeyFlags flags, char virtual_key, int losing_focus);
    int handleClickCallback(XPLMWindowID window_id, int x, int y, XPLMMouseStatus mouse);
    virtual void draw(int left, int top, int right, int bottom) = 0;
    virtual void handleNonDragClick(int no) = 0;

    static int drawCallback(XPLMDrawingPhase phase, int is_before, void* refcon);
    static void drawWindowCallback(XPLMWindowID window_id, void* refcon);
    static void handleKeyCallback(XPLMWindowID window_id, char key, XPLMKeyFlags flags, char virtual_key, void* refcon, int losing_focus);
    static int handleClickCallback(XPLMWindowID window_id, int x, int y, XPLMMouseStatus mouse, void* refcon);

private:
    bool coordInRect(float x, float y, float l, float t, float r, float b);

private:
    XPLMWindowID m_window_id;
    bool m_visible;
    DataRef<int> m_screen_width;
    DataRef<int> m_screen_height;
};

}

#endif // OVERLAYWINDOW_H
