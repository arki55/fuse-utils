/* fmfconv_types.c: .fmf movie file types
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

#include <stdlib.h>

#include "fmfconv.h"

fmf_screen_type
get_screen_type(libspectrum_byte screen_type)
{
  fmf_screen_type retval;

  switch(screen_type) {
  case '$':
    retval = STANDARD;
    break;
  case 'X':
    retval = STANDARD_TIMEX;
    break;
  case 'R':
    retval = HIRES;
    break;
  case 'C':
    retval = HICOLOR;
    break;
  default:
    printe( "Unknown Screen$ type '%d', sorry...\n", screen_type );
    exit(-1);
    break;
  }

  return retval;
}

fmf_sound_type
get_sound_type( int sound_type )
{
  fmf_sound_type retval;

  switch(sound_type) {
  case 'P':
    retval = PCM;
    break;
  case 'A':
    retval = ALW;
    break;
  case 'U':
    retval = ULW;
    break;
  default:
    printe( "Unknown sound_type:%x\n", sound_type );
    exit( -1 );
    break;
  }

  return retval;
}

const char*
get_sound_type_string( fmf_sound_type sound_type )
{
  const char *retval;

  switch( sound_type ) {
  case PCM:
    retval = "PCM";
    break;
  case ALW:
    retval = "ALAW";
    break;
  case ULW:
    retval = "ULAW";
    break;
  default:
    printe( "Unknown sound_type:%x\n", sound_type );
    exit(-1);
    break;
  }

  return retval;
}
