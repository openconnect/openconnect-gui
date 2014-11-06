/*
 * hotp.h - library internal function prototypes
 * Copyright (C) 2013 Simon Josefsson
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

#ifndef HOTP_H
#define HOTP_H

extern int
_oath_hotp_generate2 (const char *secret,
		      size_t secret_length,
		      uint64_t moving_factor,
		      unsigned digits,
		      bool add_checksum,
		      size_t truncation_offset, int flags, char *output_otp);

#endif /* HOTP_H */
