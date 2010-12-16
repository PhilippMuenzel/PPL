// Copyright (C) 2008,2009,2010 by Philipp Münzel. All rights reserved.
// Released under the terms of the license described in license.txt

#include <cassert>

#include "log.h"
#include "logwriter.h"

using namespace PPL;

Log::Log():
        m_severity(L_UNSPEC),
        m_writer(LogWriter::getLogger())
{}

Log& Log::Info(Log& log)
{
    assert(log.m_stream.str() == "");
    log.m_severity = L_INFO;
    return log;
}

Log& Log::Warn(Log& log)
{
    assert(log.m_stream.str() == "");
    log.m_severity = L_WARN;
    return log;
}

Log& Log::Error(Log& log)
{
    assert(log.m_stream.str() == "");
    log.m_severity = L_ERROR;
    return log;
}

Log& Log::Fail(Log& log)
{
    assert(log.m_stream.str() == "");
    log.m_severity = L_FAIL;
    return log;
}

Log& Log::endl(Log& log)
{
    assert(log.m_stream.str() != "");
    assert(log.m_severity != L_UNSPEC);
    time_t now;
    LogEntry entry;
    if ( time(&now) != (time_t)(-1) )
        entry.time = now;
    else
        entry.time = 0;
    entry.lvl = log.m_severity;
    entry.txt = log.m_stream.str();
    log.m_writer.postEntry(entry);
    log.m_stream.str("");
    log.m_severity=L_UNSPEC;
    return log;
}

Log& Log::operator<<(long i)
{
    m_stream << i;
    return *this;
}

Log& Log::operator<<(double d)
{
    m_stream << d;
    return *this;
}

Log& Log::operator<<(char c)
{
    m_stream << c;
    return *this;
}

Log& Log::operator<<(const std::string& s)
{
    m_stream << s;
    return *this;
}

Log& Log::operator<<(Log& (*f)(Log&))
{
    (*f)(*this);
    return *this;
}
