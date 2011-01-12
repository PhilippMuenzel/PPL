// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU General Public License version 2 or later
// as published by the Free Software Foundation, Inc.

#ifndef PLUGINPATH_H
#define PLUGINPATH_H

#include <string>
#include <stdexcept>
#if APL
#if defined(__MACH__)
#include <CoreFoundation/CoreFoundation.h>
#endif
#endif

namespace PPL {

/**
  * PluginPath provides a platform-independant absolute path to directory
  * the plugin currently runs in.
  * @author (c) 2009-2011 by Philipp Muenzel
  * @version 0.3
  */
class PluginPath
{
public:

    class PathSetupError : public std::runtime_error{
    public:
        PathSetupError(const std::string& msg):
            std::runtime_error(msg)
        {}
    };

    /**
      * prepend the absolute path to the directory of the currently running plugin
      * to the path
      * @param path to which absolute path should be prepended
      * @return the absolute path
      * @exception PathSetupError is thrown if path conversion fails
      */
    static std::string prependPath(const std::string&);

    /**
      * set the name of the directory where fat plugin resides
      * @param name
      */
    static void setPluginDirectoryName(const std::string& name);

private:

    static std::string plugin_directory;

#if APL && __MACH__
    static int ConvertPath(const char * inPath, char * outPath, int outPathMaxLen);
#endif
};

}

#endif // PLUGINPATH_H
