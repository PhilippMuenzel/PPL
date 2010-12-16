// Copyright (C) 2008,2009,2010 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU General Public License version 2 or later
// as published by the Free Software Foundation, Inc.

#include "XPStandardWidgets.h"
#include "XPWidgets.h"

#include "xposd.h"

using namespace PPL;

OverlayDisplay::OverlayDisplay(int width, int height, const std::string& title):
    screen_w_("sim/graphics/view/window_width"),
    screen_h_("sim/graphics/view/window_height"),
    title_(title)
{
    left_ = (screen_w_ - width)/2;
    right_ = (screen_w_ + width)/2;
    top_ = (screen_h_ + height)/2 + 250;
    bottom_ = (screen_h_ - height)/2 + 250;

    widget_id_ = XPCreateWidget(left_,
                                top_,
                                right_,
                                bottom_,
                                1,
                                title_.c_str(),
                                1,
                                0,
                                xpWidgetClass_MainWindow);
    XPSetWidgetProperty(widget_id_, xpProperty_MainWindowType, xpMainWindowStyle_Translucent);
    XPSetWidgetProperty(widget_id_, xpProperty_Object, reinterpret_cast<long>(this));
    XPAddWidgetCallback(widget_id_, widgetCallback);
}

OverlayDisplay::~OverlayDisplay()
{
    XPSetWidgetProperty(widget_id_, xpProperty_Object, 0);
    XPDestroyWidget(widget_id_,1);
}

int OverlayDisplay::processMessages(XPWidgetMessage inMessage, long, long)
{
    if (inMessage == xpMessage_CloseButtonPushed)
    {
        XPDestroyWidget(widget_id_, 1);
        return 1;
    }
    return 0;
}

int OverlayDisplay::widgetCallback(XPWidgetMessage inMessage, XPWidgetID inWidget, long param1, long param2)
{
    OverlayDisplay* display = reinterpret_cast<OverlayDisplay*>(XPGetWidgetProperty(inWidget, xpProperty_Object, 0));
    if (display)
    {
        return display->processMessages(inMessage, param1, param2);
    }
    return 0;
}
