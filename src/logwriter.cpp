// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU General Public License version 2 or later
// as published by the Free Software Foundation, Inc.

#include "logwriter.h"
#include <iostream>
#include <cstdlib>

using namespace PPL;

LogWriter::LogWriter()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

LogWriter::~LogWriter()
{
    if (m_logfile)
        m_logfile.close();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void LogWriter::setLogFile(const std::string& filename)
{
    m_logfile.open(filename.c_str(), std::ios_base::out | std::ios_base::trunc);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void LogWriter::postEntry(const LogEntry& entry)
{
    switch (entry.lvl) {
    case L_INFO:
        if (m_logfile)
            m_logfile << "[INFO] " << entry.txt << std::endl;
        else
            std::cout << "[INFO] " << entry.txt << std::endl;
        break;
    case L_WARN:
        if (m_logfile)
            m_logfile << "[WARN] " << entry.txt << std::endl;
        else
            std::cout << "[WARN] " << entry.txt << std::endl;
        break;
    case L_ERROR:
        if (m_logfile)
            m_logfile << "[ERROR] " << entry.txt << std::endl;
        else
            std::cerr << "[ERROR] " << entry.txt << std::endl;
        break;
    case L_FAIL:
        if (m_logfile)
            m_logfile << "[FAIL] " << entry.txt << std::endl;
        else
            std::cerr << "[FAIL] " << entry.txt << std::endl;
        std::exit(EXIT_FAILURE);
        break;
    default:
        std::cerr << "[FAIL-undefined] " << entry.txt << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

LogWriter& LogWriter::getLogger()
{
    // Meyers Singleton. Threadsafe in gcc, but not in MSVC !!!
    static LogWriter writer;
    return writer;
}
