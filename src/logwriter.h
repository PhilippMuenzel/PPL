// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU General Public License version 2 or later
// as published by the Free Software Foundation, Inc.

#ifndef LOGWRITER_H
#define LOGWRITER_H

#include <fstream>
#include "log.h"

namespace PPL {

/**
  * @brief Logger implementation that logs to file.
  *
  * By default, this is implemented by ConsoleLogger. If you want to log
  * to a file or GUI instead, override the LogWriter::writeString function.
  * @author (c) 2009-2011 by Philipp Münzel
  * @version 1.3
  */
class LogWriter
{
public:
    LogWriter();

    ~LogWriter();

    /**
      * Set a file to log all outputs to. If a log happens before
      * this funtion was called (e.g. static initializations), output is logged to stdout
      */
    void setLogFile(const std::string& filename);

    /**
      * Post a log entry to the log queue.
      * Can be called from any thread.
      */
    void postEntry(const LogEntry& entry);

    static LogWriter& getLogger();

private:
    LogWriter(const LogWriter&);
    LogWriter& operator=(const LogWriter&);
    std::ofstream m_logfile;
};

}

#endif // LOGWRITER_H
