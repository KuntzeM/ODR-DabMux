/*
   Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012
   Her Majesty the Queen in Right of Canada (Communications Research
   Center Canada)

   Copyright (C) 2018
   Matthias P. Braendli, matthias.braendli@mpb.li

    http://www.opendigitalradio.org
 */
/*
   This file is part of ODR-DabMux.

   ODR-DabMux is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   ODR-DabMux is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with ODR-DabMux.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

#include <syslog.h>
#include <cstdarg>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <iostream>
#include <list>
#include <stdexcept>
#include <string>
#include <mutex>
#include <memory>

#define SYSLOG_IDENT "ODR-DabMux"
#define SYSLOG_FACILITY LOG_LOCAL0

enum log_level_t {debug = 0, info, warn, error, alert, emerg, discard};

static const std::string levels_as_str[] =
    { "     ", "     ", "WARN ", "ERROR", "ALERT", "EMERG", "-----"} ;

/** Abstract class all backends must inherit from */
class LogBackend {
    public:
        virtual ~LogBackend() {};
        virtual void log(log_level_t level, const std::string& message) = 0;
        virtual std::string get_name() const = 0;
};

/** A Logging backend for Syslog */
class LogToSyslog : public LogBackend {
    public:
        LogToSyslog() : name("SYSLOG") {
            openlog(SYSLOG_IDENT, LOG_PID, SYSLOG_FACILITY);
        }

        virtual ~LogToSyslog() {
            closelog();
        }

        void log(log_level_t level, const std::string& message);

        std::string get_name() const { return name; }

    private:
        const std::string name;

        LogToSyslog(const LogToSyslog& other) = delete;
        const LogToSyslog& operator=(const LogToSyslog& other) = delete;
};

class LogToFile : public LogBackend {
    public:
        LogToFile(const std::string& filename);
        void log(log_level_t level, const std::string& message);
        std::string get_name() const { return name; }

    private:
        const std::string name;

        struct FILEDeleter{ void operator()(FILE* fd){ if(fd) fclose(fd);}};
        std::unique_ptr<FILE, FILEDeleter> log_file;

        LogToFile(const LogToFile& other) = delete;
        const LogToFile& operator=(const LogToFile& other) = delete;
};

class LogLine;

class Logger {
    public:
        void register_backend(std::shared_ptr<LogBackend> backend);

        /* Log the message to all backends */
        void log(log_level_t level, const char* fmt, ...);

        void logstr(log_level_t level, std::string&& message);

        /* Return a LogLine for the given level
         * so that you can write etiLog.level(info) << "stuff = " << 21 */
        LogLine level(log_level_t level);

    private:
        std::list<std::shared_ptr<LogBackend> > backends;

        std::mutex m_cerr_mutex;
};

extern Logger etiLog;

// Accumulate a line of logs, using same syntax as stringstream
// The line is logged when the LogLine gets destroyed
class LogLine {
    public:
        LogLine(const LogLine& logline);
        const LogLine& operator=(const LogLine& other) = delete;
        LogLine(Logger* logger, log_level_t level) :
            logger_(logger)
        {
            level_ = level;
        }

        // Push the new element into the stringstream
        template <typename T>
        LogLine& operator<<(T s) {
            if (level_ != discard) {
                os << s;
            }
            return *this;
        }

        ~LogLine()
        {
            if (level_ != discard) {
                logger_->logstr(level_, os.str());
            }
        }

    private:
        std::ostringstream os;
        log_level_t level_;
        Logger* logger_;
};


