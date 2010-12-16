#include "overlaywindow.h"
#include "basics.h"
#include <cstdio>

using namespace PPL;

OverlayWindow::OverlayWindow(int left, int top, int width, int height, bool is_visible):
    m_visible(is_visible),
    m_screen_width("sim/graphics/view/window_width"),
    m_screen_height("sim/graphics/view/window_height")
{
    XPLMRegisterDrawCallback(drawCallback, xplm_Phase_LastCockpit, 0, this);
    m_window_id = XPLMCreateWindow(left, top, left+width, top-height, is_visible, drawWindowCallback, handleKeyCallback, handleClickCallback, this);
}

OverlayWindow::~OverlayWindow()
{
    XPLMUnregisterDrawCallback(drawCallback, xplm_Phase_LastCockpit, 0, this);
    XPLMDestroyWindow(m_window_id);
}

void OverlayWindow::setVisible(bool b)
{
    m_visible = b;
}

int OverlayWindow::drawCallback(XPLMDrawingPhase, int)
{
    if (m_visible)
    {
        int left, top, right, bottom;
        XPLMGetWindowGeometry(m_window_id, &left, &top, &right, &bottom);
        draw(left, top, right, bottom);
    }
    return 1;
}

void OverlayWindow::drawWindowCallback(XPLMWindowID)
{
}

void OverlayWindow::handleKeyCallback(XPLMWindowID, char, XPLMKeyFlags, char, int)
{
}

int OverlayWindow::handleClickCallback(XPLMWindowID window_id, int x, int y, XPLMMouseStatus mouse)
{
    printf("mouse (%d,%d)\n",x,y);
    static	int	dX = 0, dY = 0;
    static	int	Weight = 0, Height = 0;
    int	Left, Top, Right, Bottom;

    static	int	gDragging = 0;

    if (!m_visible)
        return 0;

    /// Get the windows current position
    XPLMGetWindowGeometry(window_id, &Left, &Top, &Right, &Bottom);
    printf("window (%d,%d)(%d,%d)\n", Left, Top, Right, Bottom);
    //x = static_cast<int>(round(x * 1024.0 / static_cast<double>(m_screen_width)));
    //y = static_cast<int>(round(y *  768.0 / static_cast<double>(m_screen_height)));
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
        if (coordInRect(x, y, Left+50, Top-50, Right-50, Bottom+50))
        {
            handleNonDragClick(3);  // cheat
        }
        if (coordInRect(x, y, Left, Bottom+50, Left+80, Bottom)) {
            handleNonDragClick(0); // step
        }
        if (coordInRect(x, y, Left+85, Bottom+50, Right-85, Bottom)) {
            handleNonDragClick(2); // both
        }
        if (coordInRect(x, y, Right-80, Bottom+50, Right, Bottom)) {
            handleNonDragClick(1); // lean find
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


int OverlayWindow::drawCallback(XPLMDrawingPhase phase, int is_before, void* refcon)
{
    OverlayWindow* window = static_cast<OverlayWindow*>(refcon);
    return window->drawCallback(phase, is_before);
}

void OverlayWindow::drawWindowCallback(XPLMWindowID window_id, void* refcon)
{
    OverlayWindow* window = static_cast<OverlayWindow*>(refcon);
    window->drawWindowCallback(window_id);
}

void OverlayWindow::handleKeyCallback(XPLMWindowID window_id, char key, XPLMKeyFlags flags, char virtual_key, void* refcon, int losing_focus)
{
    OverlayWindow* window = static_cast<OverlayWindow*>(refcon);
    window->handleKeyCallback(window_id, key, flags, virtual_key, losing_focus);
}

int OverlayWindow::handleClickCallback(XPLMWindowID window_id, int x, int y, XPLMMouseStatus mouse, void* refcon)
{
    OverlayWindow* window = static_cast<OverlayWindow*>(refcon);
    return window->handleClickCallback(window_id, x, y, mouse);
}

bool OverlayWindow::coordInRect(float x, float y, float l, float t, float r, float b)
{
    return ((x >= l) && (x < r) && (y < t) && (y >= b));
}
