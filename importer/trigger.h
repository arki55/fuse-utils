/* trigger.h: Abstract class for detecting the transition between a level 0
              and 1
   Copyright (c) 2008 Fredrick Meunier

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

#ifndef TRIGGER_H
#define TRIGGER_H

#include "libspectrum.h"

class trigger {
  public:
    trigger() {};
    virtual ~trigger();

    virtual libspectrum_byte get_level(libspectrum_byte b) = 0;
};

#endif /* #ifndef TRIGGER_H */
