#ifndef GTDB_H
#define GTDB_H

#include <gnutls/gnutls.h>
#include "storage.h"

class gtdb
{
public:
    gtdb(StoredServer * ss);
    ~gtdb();
    gnutls_tdb_t get_tdb() {
        return tdb;
    };

    StoredServer *ss;
    gnutls_tdb_t tdb;
};

#endif // GTDB_H
