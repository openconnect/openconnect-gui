/*
 * oath.h - header file for liboath
 * Copyright (C) 2009-2013 Simon Josefsson
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef OATH_H
#define OATH_H

#ifndef OATHAPI
# if defined LIBOATH_BUILDING && defined HAVE_VISIBILITY && HAVE_VISIBILITY
#  define OATHAPI __attribute__((__visibility__("default")))
# elif defined LIBOATH_BUILDING && defined _MSC_VER && ! defined LIBOATH_STATIC
#  define OATHAPI __declspec(dllexport)
# elif defined _MSC_VER && ! defined LIBOATH_STATIC
#  define OATHAPI __declspec(dllimport)
# else
#  define OATHAPI
# endif
#endif

#include <stdbool.h>		/* For bool. */
#include <stdint.h>		/* For uint64_t, SIZE_MAX. */
#include <string.h>		/* For size_t.t */
#include <time.h>		/* For time_t. */

# ifdef __cplusplus
extern "C"
{
# endif

/**
 * OATH_VERSION
 *
 * Pre-processor symbol with a string that describe the header file
 * version number.  Used together with oath_check_version() to verify
 * header file and run-time library consistency.
 */
#define OATH_VERSION "2.4.0.9"

/**
 * OATH_VERSION_NUMBER
 *
 * Pre-processor symbol with a hexadecimal value describing the header
 * file version number.  For example, when the header version is 1.2.3
 * this symbol will have the value 0x01020300.  The last two digits
 * are only used between public releases, and will otherwise be 00.
 */
#define OATH_VERSION_NUMBER 0x02040000

/**
 * oath_rc:
 * @OATH_OK: Successful return
 * @OATH_CRYPTO_ERROR: Internal error in crypto functions
 * @OATH_INVALID_DIGITS: Unsupported number of OTP digits
 * @OATH_PRINTF_ERROR: Error from system printf call
 * @OATH_INVALID_HEX: Hex string is invalid
 * @OATH_TOO_SMALL_BUFFER: The output buffer is too small
 * @OATH_INVALID_OTP: The OTP is not valid
 * @OATH_REPLAYED_OTP: The OTP has been replayed
 * @OATH_BAD_PASSWORD: The password does not match
 * @OATH_INVALID_COUNTER: The counter value is corrupt
 * @OATH_INVALID_TIMESTAMP: The timestamp is corrupt
 * @OATH_NO_SUCH_FILE: The supplied filename does not exist
 * @OATH_UNKNOWN_USER: Cannot find information about user
 * @OATH_FILE_SEEK_ERROR: System error when seeking in file
 * @OATH_FILE_CREATE_ERROR: System error when creating file
 * @OATH_FILE_LOCK_ERROR: System error when locking file
 * @OATH_FILE_RENAME_ERROR: System error when renaming file
 * @OATH_FILE_UNLINK_ERROR: System error when removing file
 * @OATH_TIME_ERROR: System error for time manipulation
 * @OATH_STRCMP_ERROR: A strcmp callback returned an error
 * @OATH_INVALID_BASE32: Base32 string is invalid
 * @OATH_BASE32_OVERFLOW: Base32 encoding would overflow
 * @OATH_MALLOC_ERROR: Memory allocation failed
 * @OATH_FILE_FLUSH_ERROR: System error when flushing file buffer
 * @OATH_FILE_SYNC_ERROR: System error when syncing file to disk
 * @OATH_FILE_CLOSE_ERROR: System error when closing file
 * @OATH_LAST_ERROR: Meta-error indicating the last error code, for use
 *   when iterating over all error codes or similar.
 *
 * Return codes for OATH functions.  All return codes are negative
 * except for the successful code %OATH_OK which are guaranteed to be
 * 0.  Positive values are reserved for non-error return codes.
 *
 * Note that the #oath_rc enumeration may be extended at a later date
 * to include new return codes.
 */
typedef enum
{
  OATH_OK = 0,
  OATH_CRYPTO_ERROR = -1,
  OATH_INVALID_DIGITS = -2,
  OATH_PRINTF_ERROR = -3,
  OATH_INVALID_HEX = -4,
  OATH_TOO_SMALL_BUFFER = -5,
  OATH_INVALID_OTP = -6,
  OATH_REPLAYED_OTP = -7,
  OATH_BAD_PASSWORD = -8,
  OATH_INVALID_COUNTER = -9,
  OATH_INVALID_TIMESTAMP = -10,
  OATH_NO_SUCH_FILE = -11,
  OATH_UNKNOWN_USER = -12,
  OATH_FILE_SEEK_ERROR = -13,
  OATH_FILE_CREATE_ERROR = -14,
  OATH_FILE_LOCK_ERROR = -15,
  OATH_FILE_RENAME_ERROR = -16,
  OATH_FILE_UNLINK_ERROR = -17,
  OATH_TIME_ERROR = -18,
  OATH_STRCMP_ERROR = -19,
  OATH_INVALID_BASE32 = -20,
  OATH_BASE32_OVERFLOW = -21,
  OATH_MALLOC_ERROR = -22,
  OATH_FILE_FLUSH_ERROR = -23,
  OATH_FILE_SYNC_ERROR = -24,
  OATH_FILE_CLOSE_ERROR = -25,
  /* When adding anything here, update OATH_LAST_ERROR, errors.c
     and tests/tst_errors.c. */
  OATH_LAST_ERROR = -25
} oath_rc;

/* Global */

extern OATHAPI int oath_init (void);
extern OATHAPI int oath_done (void);

extern OATHAPI const char *oath_check_version (const char *req_version);

/* Error handling */

extern OATHAPI const char *oath_strerror (int err);
extern OATHAPI const char *oath_strerror_name (int err);

/* Data encoding */

extern OATHAPI int oath_hex2bin (const char *hexstr,
				 char *binstr, size_t * binlen);
extern OATHAPI void oath_bin2hex (const char *binstr, size_t binlen,
				  char *hexstr);

extern OATHAPI int oath_base32_decode (const char *in, size_t inlen,
				       char **out, size_t *outlen);
extern OATHAPI int oath_base32_encode (const char *in, size_t inlen,
				       char **out, size_t *outlen);

/* HOTP */

#define OATH_HOTP_LENGTH(digits, checksum) (digits + (checksum ? 1 : 0))

#define OATH_HOTP_DYNAMIC_TRUNCATION SIZE_MAX

extern OATHAPI int
oath_hotp_generate (const char *secret,
		    size_t secret_length,
		    uint64_t moving_factor,
		    unsigned digits,
		    bool add_checksum,
		    size_t truncation_offset,
		    char *output_otp);


extern OATHAPI int
oath_hotp_validate (const char *secret,
		    size_t secret_length,
		    uint64_t start_moving_factor,
		    size_t window, const char *otp);

/**
 * oath_validate_strcmp_function:
 * @handle: caller handle as passed to oath_hotp_validate_callback()
 * @test_otp: OTP to match against.
 *
 * Prototype of strcmp-like function that will be called by
 * oath_hotp_validate_callback() or oath_totp_validate_callback() to
 * validate OTPs.
 *
 * The function should be similar to strcmp in that it return 0 only
 * on matches.  It differs by permitting use of negative return codes
 * as indication of internal failures in the callback.  Positive
 * values indicate OTP mismatch.
 *
 * This callback interface is useful when you cannot compare OTPs
 * directly using normal strcmp, but instead for example only have a
 * hashed OTP.  You would then typically pass in the hashed OTP in the
 * @strcmp_handle and let your implementation of @oath_strcmp hash the
 * test_otp OTP using the same hash, and then compare the results.
 *
 * Return value: 0 if and only if @test_otp is identical to the OTP to
 *   be validated.  Negative value if an internal failure occurs.
 *   Positive value if the test_otp simply doesn't match.
 *
 * Since: 1.6.0
 **/
typedef int (*oath_validate_strcmp_function) (void *handle,
					      const char *test_otp);

/* For backwards compatibility with 1.4.x. */
#define oath_hotp_validate_strcmp_function oath_validate_strcmp_function

extern OATHAPI int
oath_hotp_validate_callback (const char *secret,
			     size_t secret_length,
			     uint64_t start_moving_factor,
			     size_t window,
			     unsigned digits,
			     oath_validate_strcmp_function strcmp_otp,
			     void *strcmp_handle);

/* TOTP */

#define OATH_TOTP_DEFAULT_TIME_STEP_SIZE	30
#define OATH_TOTP_DEFAULT_START_TIME		((time_t) 0)

/**
 * oath_totp_flags:
 * @OATH_TOTP_HMAC_SHA256: Use HMAC-SHA256 instead of HMAC-SHA1.
 * @OATH_TOTP_HMAC_SHA512: Use HMAC-SHA512 instead of HMAC-SHA1.
 *
 * Flags for oath_totp_generate2().
 */
typedef enum {
  OATH_TOTP_HMAC_SHA256 = 1,
  OATH_TOTP_HMAC_SHA512 = 2
} oath_totp_flags;

extern OATHAPI int
oath_totp_generate (const char *secret,
		    size_t secret_length,
		    time_t now,
		    unsigned time_step_size,
		    time_t start_offset,
		    unsigned digits, char *output_otp);

extern OATHAPI int
oath_totp_generate2 (const char *secret,
		     size_t secret_length,
		     time_t now,
		     unsigned time_step_size,
		     time_t start_offset,
		     unsigned digits,
		     int flags,
		     char *output_otp);

extern OATHAPI int
oath_totp_validate (const char *secret,
		    size_t secret_length,
		    time_t now,
		    unsigned time_step_size,
		    time_t start_offset,
		    size_t window, const char *otp);

extern OATHAPI int
oath_totp_validate_callback (const char *secret,
			     size_t secret_length,
			     time_t now,
			     unsigned time_step_size,
			     time_t start_offset,
			     unsigned digits,
			     size_t window,
			     oath_validate_strcmp_function strcmp_otp,
			     void *strcmp_handle);

extern OATHAPI int
oath_totp_validate2 (const char *secret,
		     size_t secret_length,
		     time_t now,
		     unsigned time_step_size,
		     time_t start_offset,
		     size_t window,
		     int *otp_pos,
		     const char *otp);

extern OATHAPI int
oath_totp_validate2_callback (const char *secret,
			      size_t secret_length,
			      time_t now,
			      unsigned time_step_size,
			      time_t start_offset,
			      unsigned digits,
			      size_t window,
			      int *otp_pos,
			      oath_validate_strcmp_function strcmp_otp,
			      void *strcmp_handle);

extern OATHAPI int
oath_totp_validate3 (const char *secret,
		     size_t secret_length,
		     time_t now,
		     unsigned time_step_size,
		     time_t start_offset,
		     size_t window,
		     int *otp_pos,
		     uint64_t *otp_counter,
		     const char *otp);

extern OATHAPI int
oath_totp_validate3_callback (const char *secret,
			      size_t secret_length,
			      time_t now,
			      unsigned time_step_size,
			      time_t start_offset,
			      unsigned digits,
			      size_t window,
			      int *otp_pos,
			      uint64_t *otp_counter,
			      oath_validate_strcmp_function strcmp_otp,
			      void *strcmp_handle);

extern OATHAPI int
oath_totp_validate4 (const char *secret,
		     size_t secret_length,
		     time_t now,
		     unsigned time_step_size,
		     time_t start_offset,
		     size_t window,
		     int *otp_pos,
		     uint64_t *otp_counter,
		     int flags,
		     const char *otp);

extern OATHAPI int
oath_totp_validate4_callback (const char *secret,
			      size_t secret_length,
			      time_t now,
			      unsigned time_step_size,
			      time_t start_offset,
			      unsigned digits,
			      size_t window,
			      int *otp_pos,
			      uint64_t *otp_counter,
			      int flags,
			      oath_validate_strcmp_function strcmp_otp,
			      void *strcmp_handle);

/* Usersfile */

extern OATHAPI int
oath_authenticate_usersfile (const char *usersfile,
			     const char *username,
			     const char *otp,
			     size_t window,
			     const char *passwd,
			     time_t * last_otp);

# ifdef __cplusplus
}
# endif

#endif /* OATH_H */
