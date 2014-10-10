/*
 * OpenConnect (SSL + DTLS) VPN client
 *
 * Copyright © 2008-2014 Intel Corporation.
 * Copyright © 2008 Nick Andrew <nick@nick-andrew.net>
 * Copyright © 2013 John Morrissey <jwm@horde.net>
 *
 * Author: David Woodhouse <dwmw2@infradead.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 */

#ifndef __OPENCONNECT_H__
#define __OPENCONNECT_H__

#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef _WIN32
#define uid_t unsigned
#endif

#define OPENCONNECT_API_VERSION_MAJOR 4
#define OPENCONNECT_API_VERSION_MINOR 0

/*
 * API version 4.0:
 *  - Change string handling to never transfer ownership of allocations.
 *  - Add openconnect_set_option_value()
 *
 * API version 3.4:
 *  - Add openconnect_set_token_callbacks()
 *
 * API version 3.3:
 *  - Add openconnect_set_pfs(), openconnect_set_dpd(),
 *    openconnect_set_proxy_auth()
 *
 * API version 3.2:
 *  - Add OC_TOKEN_MODE_HOTP and allow openconnect_has_oath_support() to
 *    return 2 to indicate that it is present.
 *
 * API version 3.1:
 *  - Add openconnect_setup_cmd_pipe(), openconnect_mainloop(),
 *    openconnect_setup_tun_device(), openconnect_setup_tun_script(),
 *    openconnect_setup_tun_fd(), openconnect_setup_dtls(),
 *    openconnect_make_cstp_connection(), openconnect_set_server_cert_sha1(),
 *    openconnect_get_ifname(), openconnect_set_reqmtu(),
 *    openconnect_get_ip_info(), openconnect_set_protect_socket_handler(),
 *    openconnect_set_mobile_info(), openconnect_set_xmlpost(),
 *    openconnect_set_stats_handler()
 *
 * API version 3.0:
 *  - Change oc_form_opt_select->choices to an array of pointers
 *  - Add oc_form_opt->flags
 *  - Add OC_FORM_RESULT_* and oc_auth_form->authgroup_*
 *
 * API version 2.2:
 *  - Add openconnect_set_token_mode(), openconnect_has_oath_support()
 *  - Deprecate openconnect_set_stoken_mode()
 *
 * API version 2.1:
 *  - Add openconnect_set_reported_os()
 *  - Add openconnect_set_stoken_mode(), openconnect_has_stoken_support()
 *
 * API version 2.0:
 *  - OPENCONNECT_X509 is now an opaque type.
 *  - Add openconnect_has_pkcs11_support(), openconnect_has_tss_blob_support()
 *  - Rename openconnect_init_openssl() -> openconnect_init_ssl()
 *  - Rename openconnect_vpninfo_new_with_cbdata() -> openconnect_vpninfo_new()
 *    and kill the old openconnect_vpninfo_new() and its callback types.
 *
 * API version 1.5:
 *  - Add openconnect_get_cert_details(), openconnect_get_cert_DER().
 *
 * API version 1.4:
 *  - Add openconnect_set_cancel_fd()
 *
 * API version 1.3:
 *  - Add openconnect_set_cert_expiry_warning() to change from default 60 days
 *
 * API version 1.2:
 *  - Add openconnect_vpninfo_new_with_cbdata()
 *
 * API version 1.1:
 *  - Add openconnect_vpninfo_free()
 *
 * API version 1.0:
 *  - Initial version
 *
 * NEW LIBRARY FUNCTION CHECKLIST:
 *
 * 1) Bump the API version if the current API version has already appeared
 *    in a release
 * 2) Add function to the above changelog
 * 3) Add function to libopenconnect.map.in
 * 4) Add declaration + comments in the latter part of this file
 * 5) Add function to jni.c, then test with ./configure --with-java && make
 * 6) Add declaration to LibOpenConnect.java, then run "ant" to test
 */

/* Before API version 1.4 (OpenConnect 3.19) this macro didn't exist.
 * Somewhat ironic, that the API version check itself needs to be
 * conditionally used depending on the API version. A very simple way
 * for users to handle this with an approximately correct answer is
 *   #include <openconnect.h>
 *   #ifndef OPENCONNECT_CHECK_VER
 *   #define OPENCONNECT_CHECK_VER(x,y) 0
 *   #endif
 */
#define OPENCONNECT_CHECK_VER(maj, min) \
	(OPENCONNECT_API_VERSION_MAJOR > (maj) || \
	(OPENCONNECT_API_VERSION_MAJOR == (maj) && \
	 OPENCONNECT_API_VERSION_MINOR >= (min)))

/****************************************************************************/

/* Authentication form processing */

#define OC_FORM_OPT_TEXT	1
#define OC_FORM_OPT_PASSWORD	2
#define OC_FORM_OPT_SELECT	3
#define OC_FORM_OPT_HIDDEN	4
#define OC_FORM_OPT_TOKEN	5

#define OC_FORM_RESULT_ERR		-1
#define OC_FORM_RESULT_OK		0
#define OC_FORM_RESULT_CANCELLED	1
#define OC_FORM_RESULT_NEWGROUP		2

#ifdef __OPENCONNECT_PRIVATE__
#define OC_FORM_RESULT_LOGGEDIN		255

#define OC_FORM_OPT_SECOND_AUTH		0x8000
#endif

#define OC_FORM_OPT_IGNORE		0x0001
#define OC_FORM_OPT_NUMERIC		0x0002

/* char * fields are static (owned by XML parser) and don't need to be
   freed by the form handling code — except for value, which for TEXT
   and PASSWORD options is allocated by openconnect_set_option_value()
   when process_form() interacts with the user and must be freed. */
struct oc_form_opt {
	struct oc_form_opt *next;
	int type;
	char *name;
	char *label;
	char *_value; /* Use openconnect_set_option_value() to set this */
	unsigned int flags;
	void *reserved;
};

/* To set the value to a form use the following function */
int openconnect_set_option_value(struct oc_form_opt *opt, const char *value);

/* All fields are static, owned by the XML parser */
struct oc_choice {
	char *name;
	char *label;
	char *auth_type;
	char *override_name;
	char *override_label;
#ifdef __OPENCONNECT_PRIVATE__
	int second_auth;
	char *secondary_username;
	int secondary_username_editable;
	int noaaa;
#endif
};

struct oc_form_opt_select {
	struct oc_form_opt form;
	int nr_choices;
	struct oc_choice **choices;
};

/* All char * fields are static, owned by the XML parser */
struct oc_auth_form {
	char *banner;
	char *message;
	char *error;
	char *auth_id;
	char *method;
	char *action;
	struct oc_form_opt *opts;
	struct oc_form_opt_select *authgroup_opt;
	int authgroup_selection;
};

struct oc_split_include {
	char *route;
	struct oc_split_include *next;
};

struct oc_ip_info {
	const char *addr;
	const char *netmask;
	const char *addr6;
	const char *netmask6;
	const char *dns[3];
	const char *nbns[3];
	const char *domain;
	const char *proxy_pac;
	int mtu;

	struct oc_split_include *split_dns;
	struct oc_split_include *split_includes;
	struct oc_split_include *split_excludes;
};

struct oc_vpn_option {
	char *option;
	char *value;
	struct oc_vpn_option *next;
};

struct oc_stats {
	uint64_t tx_pkts;
	uint64_t tx_bytes;
	uint64_t rx_pkts;
	uint64_t rx_bytes;
};

/****************************************************************************/

#define PRG_ERR		0
#define PRG_INFO	1
#define PRG_DEBUG	2
#define PRG_TRACE	3

/* Byte commands to write into the cmd_fd:
 *
 *  CANCEL closes network connections, logs off the session (cookie)
 *    and shuts down the tun device.
 *  PAUSE closes network connections and returns. The caller is expected
 *    to call openconnect_mainloop() again soon.
 *  DETACH closes network connections and shuts down the tun device.
 *    It is not legal to call openconnect_mainloop() again after this,
 *    but a new instance of openconnect can be started using the same
 *    cookie.
 *  STATS calls the stats_handler.
 */
#define OC_CMD_CANCEL		'x'
#define OC_CMD_PAUSE		'p'
#define OC_CMD_DETACH		'd'
#define OC_CMD_STATS		's'

#define RECONNECT_INTERVAL_MIN	10
#define RECONNECT_INTERVAL_MAX	100

struct openconnect_info;

#define OPENCONNECT_X509 void

typedef enum {
	OC_TOKEN_MODE_NONE,
	OC_TOKEN_MODE_STOKEN,
	OC_TOKEN_MODE_TOTP,
	OC_TOKEN_MODE_HOTP,
} oc_token_mode_t;

/* All strings are UTF-8. If operating in a legacy environment where
   nl_langinfo(CODESET) returns anything other than UTF-8, or on Windows,
   the library will take appropriate steps to convert back to the legacy
   character set (or UTF-16) for file handling and wherever else it is
   appropriate to do so. Library functions may (but probably don't yet)
   return -EILSEQ if passed invalid UTF-8 strings. */

/* Unlike previous versions of openconnect, no functions will take ownership
   of the provided strings. */


/* The buffer 'buf' must be at least 41 bytes. It will receive a hex string
   with trailing NUL, representing the SHA1 fingerprint of the certificate. */
int openconnect_get_cert_sha1(struct openconnect_info *vpninfo,
			      OPENCONNECT_X509 *cert, char *buf);
char *openconnect_get_cert_details(struct openconnect_info *vpninfo,
				   OPENCONNECT_X509 *cert);
/* Returns the length of the created DER output, in a newly-allocated buffer
   that will need to be freed by the caller. */
int openconnect_get_cert_DER(struct openconnect_info *vpninfo,
			     OPENCONNECT_X509 *cert, unsigned char **buf);

/* Contains a comma-separated list of authentication methods to enabled.
   Currently supported: Negotiate,NTLM,Digest,Basic */
int openconnect_set_proxy_auth(struct openconnect_info *vpninfo,
			       const char *methods);
int openconnect_set_http_proxy(struct openconnect_info *vpninfo,
			       const char *proxy);
int openconnect_passphrase_from_fsid(struct openconnect_info *vpninfo);
int openconnect_obtain_cookie(struct openconnect_info *vpninfo);
void openconnect_init_ssl(void);

const char *openconnect_get_hostname(struct openconnect_info *);
int openconnect_set_hostname(struct openconnect_info *, const char *);
char *openconnect_get_urlpath(struct openconnect_info *);
int openconnect_set_urlpath(struct openconnect_info *, const char *);

/* Some software tokens, such as HOTP tokens, include a counter which
 * needs to be stored in persistent storage.
 *
 * For such tokens, the lock function is first invoked to obtain a lock
 * on the storage because we're about to generate a new code. It is
 * permitted to call openconnect_set_token_mode() from the lock function,
 * if the token storage has been updated since it was first loaded. The
 * token mode must not change; only the token secret.
 *
 * The unlock function is called when a token code has been generated,
 * with a new token secret to be written to the persistent storage. The
 * secret will be in the same format as it was originally received by
 * openconnect_set_token_mode(). The new token may be NULL if an error
 * was encountered generating the code, in which case it is only
 * necessary for the callback function to unlock the storage.
 */
typedef int (*openconnect_lock_token_vfn)(void *tokdata);
typedef int (*openconnect_unlock_token_vfn)(void *tokdata, const char *new_tok);
int openconnect_set_token_callbacks(struct openconnect_info *, void *tokdata,
				    openconnect_lock_token_vfn,
				    openconnect_unlock_token_vfn);

int openconnect_set_token_mode(struct openconnect_info *,
			       oc_token_mode_t, const char *token_str);
/* Legacy stoken-only function; do not use */
int openconnect_set_stoken_mode(struct openconnect_info *, int, const char *);

/* The size must be 41 bytes, since that's the size of a 20-byte SHA1
   represented as hex with a trailing NUL. */
void openconnect_set_xmlsha1(struct openconnect_info *, const char *, int size);

int openconnect_set_cafile(struct openconnect_info *, const char *);
int openconnect_setup_csd(struct openconnect_info *, uid_t, int silent, const char *wrapper);
void openconnect_set_xmlpost(struct openconnect_info *, int enable);

/* Valid choices are: "linux", "linux-64", "win", "mac-intel",
   "android", and "apple-ios". This also selects the corresponding CSD
   trojan binary. */
int openconnect_set_reported_os(struct openconnect_info *, const char *os);

int openconnect_set_mobile_info(struct openconnect_info *vpninfo,
				const char *mobile_platform_version,
				const char *mobile_device_type,
				const char *mobile_device_uniqueid);
int openconnect_set_client_cert(struct openconnect_info *, const char *cert,
				const char *sslkey);
int openconnect_set_server_cert_sha1(struct openconnect_info *, const char *);
const char *openconnect_get_ifname(struct openconnect_info *);
void openconnect_set_reqmtu(struct openconnect_info *, int reqmtu);
void openconnect_set_dpd(struct openconnect_info *, int min_seconds);

/* The returned structures are owned by the library and may be freed/replaced
   due to rekey or reconnect. Assume that once the mainloop starts, the
   pointers are no longer valid. For similar reasons, it is unsafe to call
   this function from another thread. */
int openconnect_get_ip_info(struct openconnect_info *,
			    const struct oc_ip_info **info,
			    const struct oc_vpn_option **cstp_options,
			    const struct oc_vpn_option **dtls_options);

/* This is *not* yours and must not be destroyed with X509_free(). It
   will be valid when a cookie has been obtained successfully, and will
   be valid until the connection is destroyed or another attempt it made
   to use it. */
OPENCONNECT_X509 *openconnect_get_peer_cert(struct openconnect_info *);

int openconnect_get_port(struct openconnect_info *);
const char *openconnect_get_cookie(struct openconnect_info *);
void openconnect_clear_cookie(struct openconnect_info *);

void openconnect_reset_ssl(struct openconnect_info *vpninfo);
int openconnect_parse_url(struct openconnect_info *vpninfo, const char *url);
void openconnect_set_cert_expiry_warning(struct openconnect_info *vpninfo,
					 int seconds);
void openconnect_set_pfs(struct openconnect_info *vpninfo, unsigned val);

/* If this is set, then openconnect_obtain_cookie() will abort and return
   failure if the file descriptor is readable. Typically a user may create
   a pair of pipes with the pipe(2) system call, hand the readable one to
   this function, and then write a byte to the other end if it ever wants
   to cancel the connection. This way, a multi-threaded UI (which will be
   running openconnect_obtain_cookie() in a separate thread since it blocks)
   has the ability to cancel that call, reap its thread and free the
   vpninfo structure (or retry). An 'fd' argument of -1 will render the
   cancellation mechanism inactive. */
void openconnect_set_cancel_fd(struct openconnect_info *vpninfo, int fd);

/* Create a nonblocking pipe used to send cancellations and other commands
   to the library. This returns a file descriptor to the write side of
   the pipe. Both sides will be closed by openconnect_vpninfo_free().
   This replaces openconnect_set_cancel_fd(). */
#ifdef _WIN32
SOCKET
#else
int
#endif
openconnect_setup_cmd_pipe(struct openconnect_info *vpninfo);

const char *openconnect_get_version(void);

/* Open CSTP connection; on success, IP information will be available. */
int openconnect_make_cstp_connection(struct openconnect_info *vpninfo);

/* Create a tun device through the OS kernel (typical use case). Both
   strings are optional and can be NULL if desired. */
int openconnect_setup_tun_device(struct openconnect_info *vpninfo,
				 const char *vpnc_script, const char *ifname);

/* Pass traffic to a script program (no tun device). */
int openconnect_setup_tun_script(struct openconnect_info *vpninfo,
				 const char *tun_script);

#ifdef _WIN32
/* Caller will provide an overlap-capable handle for the tunnel traffic. */
int openconnect_setup_tun_fd(struct openconnect_info *vpninfo, intptr_t tun_fd);
#else
/* Caller will provide a file descriptor for the tunnel traffic. */
int openconnect_setup_tun_fd(struct openconnect_info *vpninfo, int tun_fd);
#endif
/* Optional call to enable DTLS on the connection. */
int openconnect_setup_dtls(struct openconnect_info *vpninfo, int dtls_attempt_period);

/* Start the main loop; exits if OC_CMD_CANCEL is received on cmd_fd or
   the remote site aborts. */
int openconnect_mainloop(struct openconnect_info *vpninfo,
			 int reconnect_timeout,
			 int reconnect_interval);

/* The first (privdata) argument to each of these functions is either
   the privdata argument provided to openconnect_vpninfo_new_with_cbdata(),
   or if that argument was NULL then it'll be the vpninfo itself. */

/* When the server's certificate fails validation via the normal means,
   this function is called with the offending certificate along with
   a textual reason for the failure (which may not be translated, if
   it comes directly from OpenSSL, but will be if it is rejected for
   "certificate does not match hostname", because that check is done
   in OpenConnect and *is* translated). The function shall return zero
   if the certificate is (or has in the past been) explicitly accepted
   by the user, and non-zero to abort the connection. */
typedef int (*openconnect_validate_peer_cert_vfn) (void *privdata,
						   OPENCONNECT_X509 *cert,
						   const char *reason);
/* On a successful connection, the server may provide us with a new XML
   configuration file. This contains the list of servers that can be
   chosen by the user to connect to, amongst other stuff that we mostly
   ignore. By "new", we mean that the SHA1 indicated by the server does
   not match the SHA1 set with the openconnect_set_xmlsha1() above. If
   they don't match, or openconnect_set_xmlsha1() has not been called,
   then the new XML is downloaded and this function is invoked. */
typedef int (*openconnect_write_new_config_vfn) (void *privdata, const char *buf,
						int buflen);
/* Handle an authentication form, requesting input from the user.
 * Return value:
 *  < 0, on error
 *  = 0, when form was parsed and POST required
 *  = 1, when response was cancelled by user
 */
typedef int (*openconnect_process_auth_form_vfn) (void *privdata,
						 struct oc_auth_form *form);
/* Logging output which the user *may* want to see. */
typedef void __attribute__ ((format(printf, 3, 4)))
		(*openconnect_progress_vfn) (void *privdata, int level,
					    const char *fmt, ...);
struct openconnect_info *openconnect_vpninfo_new(const char *useragent,
						 openconnect_validate_peer_cert_vfn,
						 openconnect_write_new_config_vfn,
						 openconnect_process_auth_form_vfn,
						 openconnect_progress_vfn,
						 void *privdata);
void openconnect_vpninfo_free(struct openconnect_info *vpninfo);

/* Callback to allow binding a newly created socket's file descriptor to
   a specific interface, e.g. with SO_BINDTODEVICE. This tells the kernel
   not to route the traffic in question over the VPN tunnel. */
typedef void (*openconnect_protect_socket_vfn) (void *privdata, int fd);
void openconnect_set_protect_socket_handler(struct openconnect_info *vpninfo,
					    openconnect_protect_socket_vfn protect_socket);

/* Callback for obtaining traffic stats via OC_CMD_STATS.
 */
typedef void (*openconnect_stats_vfn) (void *privdata, const struct oc_stats *stats);
void openconnect_set_stats_handler(struct openconnect_info *vpninfo,
				   openconnect_stats_vfn stats_handler);

/* SSL certificate capabilities. openconnect_has_pkcs11_support() means that we
   can accept PKCS#11 URLs in place of filenames, for the certificate and key. */
int openconnect_has_pkcs11_support(void);

/* The OpenSSL TPM ENGINE stores keys in a PEM file labelled with the string
   -----BEGIN TSS KEY BLOB-----. GnuTLS may learn to support this format too,
   in the near future. */
int openconnect_has_tss_blob_support(void);

/* Software token capabilities. */
int openconnect_has_stoken_support(void);
int openconnect_has_oath_support(void);

#endif /* __OPENCONNECT_H__ */
