// Copyright (C) 2008,2009,2010 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU General Public License version 2 or later
// as published by the Free Software Foundation, Inc.

#include "settings.h"
#include "log.h"

using namespace PPL;

Settings::Settings(const std::string& filename, bool create_if_not_exists, bool write_new_file):
        m_config_file_name(filename),
        m_file_did_exist_before(false),
        m_create_file_if_not_exists(create_if_not_exists),
        m_write_new_file(write_new_file)
{
    FILE* f = fopen(m_config_file_name.c_str(), "rb");
    if (f) {
        m_file_did_exist_before = true;
        fclose(f);
        Log() << Log::Info << "Using config file." << Log::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


Settings::~Settings()
{
    if (m_write_new_file) {
        if (m_file_did_exist_before || m_create_file_if_not_exists)
        {
            if (SI_OK != this->SaveFile(m_config_file_name.c_str()))
            {
                Log() << Log::Info << "Could not save config file" << Log::endl;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool Settings::loadFromFile()
{
    if (m_file_did_exist_before)
    {
        bool success = (SI_OK == this->LoadFile(m_config_file_name.c_str()));
        return success;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

std::string Settings::get(const std::string &section, const std::string &key)
{
    return std::string(GetValue(section.c_str(), key.c_str()));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

long Settings::getLong(const std::string& section, const std::string& key)
{
    return GetLongValue(section.c_str(), key.c_str());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Settings::set(const std::string& section, const std::string& key, const std::string& value)
{
    SetValue(section.c_str(), key.c_str(), value.c_str());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Settings::setLong(const std::string& section, const std::string& key, long value)
{
    SetLongValue(section.c_str(), key.c_str(), value);
}
