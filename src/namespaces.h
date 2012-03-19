// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, Inc.

#ifndef NAMESPACES_H
#define NAMESPACES_H

#ifndef PRIVATENAMESPACE
#error you didnt define your private name. Please read the forum.
#endif

#define PASTER(x,y) x ## y
#define EVALUATOR(x,y)  PASTER(x,y)
#define PPLNAMESPACE EVALUATOR(PPL, PRIVATENAMESPACE)


namespace PPLNAMESPACE{} namespace PPL = PPLNAMESPACE;


#endif // NAMESPACES_H
