// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, Inc.

#ifndef ACTION_H
#define ACTION_H

#include <string>
#include "namespaces.h"

namespace PPLNAMESPACE {

class Action {
public:
    virtual ~Action() {};
    virtual const std::string name() const = 0;
    virtual void doAction() = 0;
};

}

#endif // ACTION_H
