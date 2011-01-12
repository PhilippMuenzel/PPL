// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU General Public License version 2 or later
// as published by the Free Software Foundation, Inc.

#ifndef ACTION_H
#define ACTION_H

#include <string>

namespace PPL {

class Action {
public:
    virtual ~Action() {};
    virtual const std::string name() const = 0;
    virtual void doAction() = 0;
};

}

#endif // ACTION_H
