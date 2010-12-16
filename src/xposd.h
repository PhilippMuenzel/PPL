// Copyright (C) 2008,2009,2010 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU General Public License version 2 or later
// as published by the Free Software Foundation, Inc.

#ifndef XPOSD_H
#define XPOSD_H

#include "XPWidgetDefs.h"
#include "dataref.h"

namespace PPL {


class OverlayDisplay {
public:
    OverlayDisplay(int width, int height, const std::string& title);

    ~OverlayDisplay();

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

#endif // XPOSD_H
