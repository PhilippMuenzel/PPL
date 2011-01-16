// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, Inc.

#include "menuitem.h"
#include "action.h"
#include <iostream>

using namespace PPL;

MenuItem::MenuItem(const std::string& title)
{
    m_item_id = XPLMAppendMenuItem(XPLMFindPluginsMenu(), title.c_str(), NULL, 1);
    m_menu_id = XPLMCreateMenu(title.c_str(), XPLMFindPluginsMenu(), m_item_id, menuHandler, this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

MenuItem::~MenuItem()
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void MenuItem::menuHandler(void *iRef)
{
    int menu_item = reinterpret_cast<int>(iRef);
    m_actions[menu_item]->doAction();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void MenuItem::addSubItem(Action* action)
{
    m_actions.push_back(action);
    XPLMAppendMenuItem(m_menu_id, action->name().c_str(), reinterpret_cast<void*>(m_actions.size()-1), 1);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void MenuItem::menuHandler(void *mRef, void *iRef)
{
    MenuItem* menu = static_cast<MenuItem*>(mRef);
    menu->menuHandler(iRef);
}
