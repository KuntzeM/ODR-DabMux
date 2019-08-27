/*
   Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009 Her Majesty the Queen in
   Right of Canada (Communications Research Center Canada)

   Copyright (C) 2019
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
#include "Log.h"
#include "RemoteControl.h"
#include <string>

namespace Inputs {

/* New input object base */
class InputBase {
    public:
        /* Throws runtime_error or invalid_argument on failure */
        virtual void open(const std::string& name) = 0;

        virtual int readFrame(uint8_t* buffer, size_t size) = 0;

        /* Returns the effectively used bitrate, or throws invalid_argument on invalid bitrate */
        virtual int setBitrate(int bitrate) = 0;
        virtual void close() = 0;

        virtual ~InputBase() {}
    protected:
        InputBase() {}
};

};

