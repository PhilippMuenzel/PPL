// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU General Public License version 2 or later
// as published by the Free Software Foundation, Inc.

#include <string>
#include <cstring>

#include "XPLMUtilities.h"

#include "pluginpath.h"

using namespace PPL;

std::string PluginPath::plugin_directory = "";

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

std::string PluginPath::prependPath(const std::string& file)
{
    if (plugin_directory == "")
        throw PathSetupError("Critical error - no plugin name set - unable to create file in the right directory");
    char path[512];
    XPLMGetSystemPath(path);
    std::string absolute_path(path);
    absolute_path.
            append("Resources").append(XPLMGetDirectorySeparator()).
            append("plugins").append(XPLMGetDirectorySeparator()).
            append(plugin_directory).append(XPLMGetDirectorySeparator()).
            append(file);
#if APL && __MACH__
    int result = ConvertPath(absolute_path.c_str(), path, 512);
    if (result == 0)
        return std::string(path);
    else
        throw PathSetupError("Critical error - cannot convert Mac-HFS-format path to unix-format path");
#endif
    return absolute_path;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void PluginPath::setPluginDirectoryName(const std::string& name)
{
    PluginPath::plugin_directory = name;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if APL && __MACH__
int PluginPath::ConvertPath(const char * inPath, char * outPath, int outPathMaxLen)
{
    CFStringRef inStr = CFStringCreateWithCString(kCFAllocatorDefault, inPath ,kCFStringEncodingMacRoman);
    if (inStr == NULL)
        return -1;
    CFURLRef url = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, inStr, kCFURLHFSPathStyle,0);
    CFStringRef outStr = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
    if (!CFStringGetCString(outStr, outPath, outPathMaxLen, kCFURLPOSIXPathStyle))
        return -1;
    CFRelease(outStr);
    CFRelease(url);
    CFRelease(inStr);
    return 0;
}
#endif

