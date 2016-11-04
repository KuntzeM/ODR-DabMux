/*
   Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009 Her Majesty the Queen in
   Right of Canada (Communications Research Center Canada)
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

#ifndef DAB_INPUT_H
#define DAB_INPUT_H

#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif
#include "Log.h"
#include "RemoteControl.h"
#include <string>

// TODO replace usage of dabInputOperations by a
// class hierarchy
struct dabInputOperations {
    int (*init)(void** args);
    int (*open)(void* args, const char* name);
    int (*setbuf)(void* args, int size);
    int (*read)(void* args, void* buffer, int size);
    int (*lock)(void* args);
    int (*unlock)(void* args);
    int (*readFrame)(dabInputOperations* ops, void* args, void* buffer, int size);
    int (*setBitrate)(dabInputOperations* ops, void* args, int bitrate);
    int (*close)(void* args);
    int (*clean)(void** args);
    int (*rewind)(void* args);
    bool operator==(const dabInputOperations&);
};

/* Wrapper class for old-style dabInputOperations inputs */
class DabInputCompatible : public DabInputBase {
    public:
        DabInputCompatible(dabInputOperations ops)
            : m_ops(ops)
        { m_ops.init(&args); }

        virtual ~DabInputCompatible()
        { m_ops.clean(&args); }

        virtual int open(const std::string& name)
        { return m_ops.open(args, name.c_str()); }

        virtual int setbuf(int size)
        { return m_ops.setbuf(args, size); }

        virtual int readFrame(uint8_t* buffer, size_t size)
        {
            if (m_ops.lock) {
                m_ops.lock(args);
            }
            int result = m_ops.readFrame(&m_ops, args, buffer, size);
            if (m_ops.unlock) {
                m_ops.unlock(args);
            }
            return result;
        }

        virtual int setBitrate(int bitrate)
        { return m_ops.setBitrate(&m_ops, args, bitrate); }

        virtual int close()
        { return m_ops.close(args); }

        virtual int rewind()
        { return m_ops.rewind(args); }

        virtual int read(void* buffer, int size)
        { return m_ops.read(args, buffer, size); }

        virtual dabInputOperations getOpts() { return m_ops; }

    private:
        DabInputCompatible& operator=(const DabInputCompatible& other);
        DabInputCompatible(const DabInputCompatible& other);

        dabInputOperations m_ops;
        void* args;
};

int dabInputSetbuf(void* args, int size);
int dabInputSetbitrate(dabInputOperations* ops, void* args, int bitrate);


#endif // DAB_INPUT_H
