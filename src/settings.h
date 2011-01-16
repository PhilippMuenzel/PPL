// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, Inc.

#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

#define SI_CONVERT_GENERIC
#include "SimpleIni.h"

namespace PPL {

/**
  * Convenient access to ini-Files.
  * This is a small wrapper accessing the most frequently used basic functions of SimpleIni
  * @version 0.3
  * @author (c) 2009-2011 by Philipp Muenzel
  */
class Settings : private CSimpleIniA
{
public:
    /**
      * init with the path to an ini.-file. If the file does not exist, it is created
      * and everything set via setValue is stored
      * @param filename path to ini-file
      * @param create_if_not_exists whether to create a new .ini-file with default settings if it wasn't there before
      * @param write_new_file whether to rewrite the file with updated settings on exit
      */
    Settings(const std::string& filename, bool create_if_not_exists = false, bool write_new_file = false);

    /**
      * if file should be created
      */
    ~Settings();

    /**
      * load settings from the file an overwrite everything that was set before
      */
    bool loadFromFile();

    /**
      * get a string value
      */
    std::string get(const std::string& section, const std::string& key);

    /**
      * get a long value
      */
    long getLong(const std::string& section, const std::string& key);

    /**
      * set a long value
      */
    void set(const std::string& section, const std::string& key, const std::string& value);

    /**
      * set a string value
      */
    void setLong(const std::string& section, const std::string& key, long value);


private:

    std::string m_config_file_name;

    bool m_file_did_exist_before;

    bool m_create_file_if_not_exists;

    bool m_write_new_file;
};

}
#endif // SETTINGS_H
