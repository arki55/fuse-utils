/* fmfconv_types.h: .fmf movie file types
   Copyright (c) 2017 Fredrick Meunier

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

   Author contact information:

   E-mail: fredm@spamcop.net
*/

#ifndef FMFCONV_TYPES_H
#define FMFCONV_TYPES_H

#include <stdio.h>

#include <libspectrum.h>

typedef enum {
  STANDARD,	/* screen type $ -> normal Spectrum */
  HIRES,	/* R -> Timex HighRes */
  HICOLOR,	/* C -> Timex HiColor */
  STANDARD_TIMEX	/* X -> Timex normal */
} fmf_screen_type;

fmf_screen_type get_screen_type(libspectrum_byte screen_type);

#endif	/* FMFCONV_TYPES_H */
