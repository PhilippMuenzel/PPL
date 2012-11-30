// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, Inc.

#include <string>
#include <cstring>

#if APL
#if defined(__MACH__)
#include <CoreFoundation/CoreFoundation.h>
#endif
#endif

#include "XPLMUtilities.h"
#include "XPLMPlanes.h"

#include "pluginpath.h"

using namespace PPLNAMESPACE;

std::string PluginPath::plugin_directory = "";

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

std::string PluginPath::prependXPlanePath(const std::string& file)
{
#ifdef BUILD_FOR_STANDALONE
    return file;
#else
    char path[512];
    XPLMGetSystemPath(path);
    std::string absolute_path(path);
#if APL && __MACH__
    int result = ConvertPath(absolute_path.c_str(), path, 512);
    if (result == 0)
        absolute_path = std::string(path);
    else
        throw PathSetupError("Critical error - cannot convert Mac-HFS-format path to unix-format path");
#endif
    absolute_path.append(file);
    return absolute_path;
#endif
}

std::string PluginPath::prependPluginPath(const std::string& file)
{
#ifdef BUILD_FOR_STANDALONE
    return file;
#else
    std::string path = "/Resources/plugins/";
    path.append(plugin_directory).append("/").append(file);
    return prependXPlanePath(path);
#endif
}

std::string PluginPath::prependPluginResourcesPath(const std::string& file)
{
    std::string res_path("Resources/");
    res_path.append(file);
#ifdef BUILD_FOR_STANDALONE
    return res_path;
#else
    return prependPluginPath(res_path);
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

std::string PluginPath::prependPlanePath(const std::string& file)
{
#ifdef BUILD_FOR_STANDALONE
    return file;
#else
    char name[512];
    char path[512];
    XPLMGetNthAircraftModel(0, name, path);
    std::string absolute_path(path);
    std::size_t pos = absolute_path.find(name);
    absolute_path = absolute_path.substr(0, pos);
#if APL && __MACH__
    int result = ConvertPath(absolute_path.c_str(), path, 512);
    if (result == 0)
        absolute_path = std::string(path);
    else
        throw PathSetupError("Critical error - cannot convert Mac-HFS-format path to unix-format path");
#endif
    absolute_path.append(file);
    return absolute_path;
#endif
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

