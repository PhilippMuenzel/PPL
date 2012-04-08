// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, Inc.

#include "XPStandardWidgets.h"
#include "XPWidgets.h"

#include "onscreendisplay.h"

using namespace PPLNAMESPACE;

OnScreenDisplay::OnScreenDisplay(int width, int height, const std::string& title):
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

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

OnScreenDisplay::~OnScreenDisplay()
{
    XPSetWidgetProperty(widget_id_, xpProperty_Object, 0);
    XPDestroyWidget(widget_id_,1);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int OnScreenDisplay::processMessages(XPWidgetMessage inMessage, intptr_t, intptr_t)
{
    if (inMessage == xpMessage_CloseButtonPushed)
    {
        XPDestroyWidget(widget_id_, 1);
        return 1;
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int OnScreenDisplay::widgetCallback(XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t param1, intptr_t param2)
{
    OnScreenDisplay* display = reinterpret_cast<OnScreenDisplay*>(XPGetWidgetProperty(inWidget, xpProperty_Object, 0));
    if (display)
    {
        return display->processMessages(inMessage, param1, param2);
    }
    return 0;
}
