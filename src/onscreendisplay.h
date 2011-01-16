// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, Inc.

#ifndef ONSCREENDISPLAY_H
#define ONSCREENDISPLAY_H

#include "XPWidgetDefs.h"
#include "dataref.h"

namespace PPL {

class OnScreenDisplay {
public:
    OnScreenDisplay(int width, int height, const std::string& title);

    ~OnScreenDisplay();

    int processMessages(XPWidgetMessage inMessage, long, long);

    static int widgetCallback(XPWidgetMessage inMessage, XPWidgetID inWidget, long param1, long param2);

private:
    DataRef<int> screen_w_;
    DataRef<int> screen_h_;
    XPWidgetID widget_id_;
    int top_, left_, bottom_, right_;
    std::string title_;
};

}

#endif // ONSCREENDISPLAY_H
