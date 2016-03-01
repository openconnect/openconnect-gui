/*
 * Copyright (C) 2014 Red Hat
 *
 * This file is part of openconnect-gui.
 *
 * openconnect-gui is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GTDB_H
#define GTDB_H

class StoredServer;
extern "C" {
#include <gnutls/gnutls.h>
}

class gtdb {
public:
    gtdb(StoredServer* ss);
    ~gtdb();

    gnutls_tdb_t get_tdb() const;

    StoredServer* ss;
    gnutls_tdb_t tdb;
};

#endif // GTDB_H
