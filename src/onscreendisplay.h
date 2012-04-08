// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, Inc.

#ifndef ONSCREENDISPLAY_H
#define ONSCREENDISPLAY_H

#include "XPWidgetDefs.h"
#include "dataref.h"
#include "namespaces.h"

namespace PPLNAMESPACE {

class OnScreenDisplay {
public:
    OnScreenDisplay(int width, int height, const std::string& title);

    ~OnScreenDisplay();

    int processMessages(XPWidgetMessage inMessage, intptr_t, intptr_t);

    static int widgetCallback(XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t param1, intptr_t param2);

private:
    DataRef<int> screen_w_;
    DataRef<int> screen_h_;
    XPWidgetID widget_id_;
    int top_, left_, bottom_, right_;
    std::string title_;
};

}

#endif // ONSCREENDISPLAY_H
