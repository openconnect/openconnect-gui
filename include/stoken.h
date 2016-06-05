/*
 * stoken.h - public libstoken library interface
 *
 * Copyright 2012 Kevin Cernekee <cernekee@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __STOKEN_H__
#define __STOKEN_H__

#include <errno.h>
#include <stdlib.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define STOKEN_API_VER_MAJOR	1
#define STOKEN_API_VER_MINOR	3

/* Before API version 1.3 (stoken 0.8) this macro didn't exist.
 * Somewhat ironic, that the API version check itself needs to be
 * conditionally used depending on the API version. A very simple way
 * for users to handle this with an approximately correct answer is
 *   #include <stoken.h>
 *   #ifndef STOKEN_CHECK_VER
 *   #define STOKEN_CHECK_VER(x,y) 0
 *   #endif
 */
#define STOKEN_CHECK_VER(maj, min) \
	(STOKEN_API_VER_MAJOR > (maj) || \
	(STOKEN_API_VER_MAJOR == (maj) && \
	 STOKEN_API_VER_MINOR >= (min)))

#define STOKEN_MAX_TOKENCODE	8

#if defined(_WIN32) && defined(LIBSTOKEN_BUILD)
#define STOKEN_EXPORT		__declspec(dllexport)
#elif defined(_WIN32)
#define STOKEN_EXPORT		__declspec(dllimport)
#else
#define STOKEN_EXPORT
#endif

struct stoken_ctx;

struct stoken_info {
	char			serial[16];
	time_t			exp_date;
	int			interval;
	int			token_version;
	int			uses_pin;
};

struct stoken_guid {
	const char		*tag;
	const char		*long_name;
	const char		*guid;
};

/*
 * Create/destroy library context.
 * stoken_new() returns NULL on error.
 */
STOKEN_EXPORT struct stoken_ctx *stoken_new(void);
STOKEN_EXPORT void stoken_destroy(struct stoken_ctx *ctx);

/*
 * Load a token from an existing .stokenrc file (PATH can be NULL to use
 * $HOME/.stokenrc).
 *
 * Return values:
 *
 *   0:       success; token is now stored in CTX
 *   -ENOENT: missing input file
 *   -EINVAL: invalid input file format
 *   -EIO:    any other failure (e.g. ran out of memory)
 */
STOKEN_EXPORT int stoken_import_rcfile(struct stoken_ctx *ctx,
				       const char *path);

/*
 * Parse a token string (nominally a string of ~71 digits, starting with
 * '1' or '2').
 *
 * Return values:
 *
 *   0:       success; token is now stored in CTX
 *   -EINVAL: invalid input string format
 *   -EIO:    any other failure (e.g. ran out of memory)
 */
STOKEN_EXPORT int stoken_import_string(struct stoken_ctx *ctx,
				       const char *token_string);

/*
 * Retrieve metadata for the currently imported token.  This returns a
 * callee-allocated, caller-freed struct, which may grow larger in the future.
 *
 * In general this should be called after stoken_decrypt_seed(), as
 * most of the token metadata is encrypted with the devid and/or password on
 * v3 tokens.
 *
 * Return values:
 *
 *   ptr:     success
 *   NULL:    any failure (e.g. ran out of memory)
 */
STOKEN_EXPORT struct stoken_info *stoken_get_info(struct stoken_ctx *ctx);

/*
 * Set *MIN_PIN and *MAX_PIN to reflect the valid range of PIN lengths
 * (e.g. 4-8).
 */
STOKEN_EXPORT void stoken_pin_range(struct stoken_ctx *ctx,
				    int *min_pin,
				    int *max_pin);

/*
 * Returns nonzero if the token in CTX requires a PIN, and doesn't have one
 * saved (i.e. you need to prompt for it).  stoken_info->uses_pin returns
 * nonzero if a PIN is used in the calculation.  If stoken_info->uses_pin is
 * 0, a PIN is not needed to generate the tokencode but you may need to
 * request and concatenate a PIN in order to log in to a protected resource:
 * PASSCODE = PIN + TOKENCODE
 */
STOKEN_EXPORT int stoken_pin_required(struct stoken_ctx *ctx);

/* returns nonzero if the token in CTX needs a password to decrypt the seed */
STOKEN_EXPORT int stoken_pass_required(struct stoken_ctx *ctx);

/* returns nonzero if the token in CTX needs a device ID to decrypt the seed */
STOKEN_EXPORT int stoken_devid_required(struct stoken_ctx *ctx);

/*
 * Check the PIN for proper format.  This does not validate whether the PIN
 * matches the PIN on file for the user's account; only the server knows that.
 *
 * Return values:
 *
 *   0:       success
 *   -EINVAL: invalid format
 */
STOKEN_EXPORT int stoken_check_pin(struct stoken_ctx *ctx, const char *pin);

/*
 * Obtain the list of known "class GUIDs" used to bind a token to a specific
 * type of device (e.g. iPhone).
 */
STOKEN_EXPORT const struct stoken_guid *stoken_get_guid_list(void);

/*
 * Check the device ID by performing a partial seed decrypt.  This helps
 * callers provide better user feedback after a stoken_decrypt_seed() failure.
 *
 * Return values:
 *
 *   0:       success
 *   -EINVAL: DEVID MAC failed
 *   -EIO:    any other failure (e.g. ran out of memory)
 */
STOKEN_EXPORT int stoken_check_devid(struct stoken_ctx *ctx,
				     const char *devid);

/*
 * Try to decrypt the seed stored in CTX, and compare the MAC to see if
 * decryption was successful.
 *
 * PASS may be NULL if stoken_needs_pass() == 0
 * DEVID may be NULL if stoken_needs_devid() == 0
 *
 * Return values:
 *
 *   0:       success
 *   -EINVAL: MAC failed (PASS or DEVID is probably incorrect)
 *   -EIO:    any other failure (e.g. ran out of memory)
 */
STOKEN_EXPORT int stoken_decrypt_seed(struct stoken_ctx *ctx,
				      const char *pass,
				      const char *devid);

/*
 * Generate a new token string for the previously-decrypted seed stored
 * in CTX.  PASS and DEVID may be NULL.  The returned string must be freed
 * by the caller.
 *
 * Return values:
 *
 *   ptr:     on success, a pointer to a new string
 *   NULL:    on failure
 */
STOKEN_EXPORT char *stoken_encrypt_seed(struct stoken_ctx *ctx,
					const char *pass,
					const char *devid);

/*
 * Generate a tokencode from the decrypted seed, for UNIX time WHEN.
 * OUT is allocated by the caller, and must be able to store at least
 * (STOKEN_MAX_TOKENCODE + 1) bytes.
 *
 * This can be called over and over again, as needed.
 *
 * If stoken_pin_required() returns 0, PIN may be NULL.  If PIN is not
 * NULL and the user stored a PIN in ~/.stokenrc, the PIN string passed
 * into this function will override the stored PIN.  This will affect
 * subsequent calls to stoken_compute_tokencode() but the change will not
 * be stored on disk.
 *
 * Return values:
 *
 *   0:       success
 *   -EINVAL: invalid PIN format
 *   -EIO:    general failure
 */
STOKEN_EXPORT int stoken_compute_tokencode(struct stoken_ctx *ctx,
					   time_t when,
					   const char *pin,
					   char *out);

/*
 * Inject a space in the middle of the code, e.g. "1234 5678".
 * Typical libstoken users would use the formatted tokencode for display
 * purposes only, and use the unformatted tokencode for "copy to clipboard",
 * pasting into login forms, etc.
 *
 * The returned string must be freed by the caller.  Returns NULL on malloc
 * failure.
 */
STOKEN_EXPORT char *stoken_format_tokencode(const char *tokencode);

#ifdef __cplusplus
}
#endif

#endif /* !__STOKEN_H__ */
