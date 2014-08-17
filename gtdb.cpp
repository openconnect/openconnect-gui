#include "gtdb.h"
#include <gnutls/crypto.h>

#define HASH GNUTLS_DIG_SHA1
#define HASH_LEN 20
#define MAX_HASH_LEN 64

static int
store_cb(const char* db_name, const char* host, const char* service,
              time_t expiration,
              const gnutls_datum_t* pubkey)
{
    const gtdb *tdb = reinterpret_cast<const gtdb*>(db_name);
    char output[MAX_HASH_LEN];
    QByteArray ahash;
    int ret;

    ret = gnutls_hash_fast(HASH, pubkey->data, pubkey->size, output);
    if (ret < 0) {
        return -1;
    }

    ahash.append(output, HASH_LEN);
    tdb->ss->set_server_hash(HASH, ahash);

    return 0;
}

static int
verify_cb(const char* db_name, const char* host, const char* service,
                       const gnutls_datum_t* pubkey)
{
    const gtdb *tdb = reinterpret_cast<const gtdb*>(db_name);
    QByteArray ahash;
    unsigned algo;
    int len;
    int ret;
    char output[MAX_HASH_LEN];

    algo = tdb->ss->get_server_hash(ahash);
    len = gnutls_hash_get_len((gnutls_digest_algorithm_t)algo);

    if (algo == 0 || len > (int)sizeof(output))
        return -1;

    if (ahash.size() != len)
        return -1;

    ret = gnutls_hash_fast((gnutls_digest_algorithm_t)algo, pubkey->data, pubkey->size, output);
    if (ret < 0) {
        return -1;
    }

    if (memcmp(ahash.constData(), output, len) == 0)
        return 0;
    return GNUTLS_E_CERTIFICATE_KEY_MISMATCH;
}

gtdb::gtdb(StoredServer * ss)
{
    this->ss = ss;
    gnutls_tdb_init(&this->tdb);
    gnutls_tdb_set_verify_func(tdb, verify_cb);
    gnutls_tdb_set_store_func(tdb, store_cb);
}

gtdb::~gtdb()
{
    gnutls_tdb_deinit(this->tdb);
}
