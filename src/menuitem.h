// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, Inc.

#ifndef MENUITEM_H
#define MENUITEM_H

#include <string>
#include <vector>

#include "XPLMDisplay.h"
#include "XPLMMenus.h"
#include "XPWidgets.h"
#include "XPStandardWidgets.h"
#include "namespaces.h"

namespace PPLNAMESPACE {

class Action;

class MenuItem
{
public:
    MenuItem(const std::string& title);
    ~MenuItem();
    void addSubItem(Action* action);
    void menuHandler(void* iRef);
private:
    static void menuHandler(void* mRef, void* iRef);
    int m_item_id;
    std::vector<Action*> m_actions;
    XPLMMenuID m_menu_id;
};


}


#endif // MENUITEM_H
