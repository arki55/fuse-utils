dnl Process this file with autoconf to produce a configure script.

dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License along
dnl with this program; if not, write to the Free Software Foundation, Inc.,
dnl 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
dnl
dnl Author contact information:
dnl
dnl E-mail: philip-fuse@shadowmagic.org.uk

dnl Package version
m4_define([fuse_utils_version], [1.4.3])

dnl Product full version
m4_define([fuse_utils_major_version], [1])
m4_define([fuse_utils_minor_version], [4])
m4_define([fuse_utils_micro_version], [3])
m4_define([fuse_utils_nano_version],  [0])
m4_define([fuse_utils_full_version],
          [fuse_utils_major_version.fuse_utils_minor_version.fuse_utils_micro_version.fuse_utils_nano_version])
m4_define([fuse_utils_rc_version],
          [fuse_utils_major_version,fuse_utils_minor_version,fuse_utils_micro_version,fuse_utils_nano_version])

dnl Package info
m4_define([fuse_url], [http://fuse-emulator.sourceforge.net/])
m4_define([fuse_bugreport], [http://sourceforge.net/p/fuse-emulator/bugs/])

AC_INIT([fuse-utils],[fuse_utils_version],[fuse_bugreport],
        [fuse-utils],[fuse_url])
AC_CONFIG_SRCDIR([tzxlist.c])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AC_CANONICAL_HOST

dnl Use automake to produce `Makefile.in'
AM_INIT_AUTOMAKE([1.11 subdir-objects])
AM_SILENT_RULES([yes])

dnl Definitions for config.h
AC_DEFINE([FUSE_UTILS_RC_VERSION], [fuse_utils_rc_version],
          [Define version information for win32 executables])

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CXX
LT_INIT

dnl Check for host specific programs.
case "$host_os" in
  mingw32*)
    AC_CHECK_TOOL([WINDRES], [windres])
    AC_CHECK_PROGS([MAN2HTML], [man2html])
    AC_CHECK_PROGS([GROFF], [groff])
    AC_CHECK_PROGS([UNIX2DOS], [unix2dos])
    ;;
esac
AM_CONDITIONAL([HAVE_WINDRES], [test "$WINDRES" != ""])

dnl Check non C89 functions
AC_CHECK_FUNCS(fdopen signal)

dnl Check for big endianness
AC_C_BIGENDIAN

dnl Check for header files
AC_CHECK_HEADERS(strings.h)
AC_CHECK_HEADERS(termios.h)

dnl Check for zlib (the UNIX version is called z, Win32 zdll)
AC_MSG_CHECKING(whether to use zlib)
AC_ARG_WITH(zlib,
[  --without-zlib          don't use zlib],
if test "$withval" = no; then zlib=no; else zlib=yes; fi,
zlib=yes)
AC_MSG_RESULT($zlib)
have_zlib="no"
if test "$zlib" = yes; then
  AC_CHECK_HEADERS(
    zlib.h,
    [AC_SEARCH_LIBS(compress2, z zdll)
     have_zlib="yes"]
  )
fi

dnl Allow the user to say that various libraries are in one place
AC_ARG_WITH(local-prefix,
[  --with-local-prefix=PFX local libraries installed in PFX (optional)],
CPPFLAGS="$CPPFLAGS -I$withval/include";
CXXFLAGS="$CXXFLAGS -I$withval/include";
LDFLAGS="$LDFLAGS -L$withval/lib",
if test "$prefix" != "NONE"; then
  CPPFLAGS="$CPPFLAGS -I$prefix/include";
  CXXFLAGS="$CXXFLAGS -I$prefix/include";
  LDFLAGS="$LDFLAGS -L$prefix/lib"
fi)

dnl Check if libjpeg is available and supports jpeg_write_scanlines
AC_MSG_CHECKING(whether to use libjpeg)
AC_ARG_WITH(libjpeg,
  [  --without-libjpeg        don't use libjpeg],
  if test "$withval" = no; then libjpeg=no; else libjpeg=yes; fi,
  libjpeg=yes
)
AC_MSG_RESULT($libjpeg)

have_libjpeg="no"
if test "$libjpeg" = yes; then
  AC_CHECK_HEADER(
    jpeglib.h,
    [AC_CHECK_LIB( jpeg, jpeg_write_scanlines,
      [AC_DEFINE([USE_LIBJPEG], 1,
        [Defined if we're going to be using the installed libjpeg])
        JPEG_LIBS='-ljpeg'
        have_libjpeg="yes"],
      [AC_MSG_WARN(jpeg_write_scanlines not found - jpeg output not supported...)],
    )],
    [AC_MSG_WARN(jpeglib.h not found - jpeg output not supported...)]
  )
fi
AM_CONDITIONAL(HAVE_LIBJPEG, test "$have_libjpeg" = "yes")
AC_SUBST(JPEG_LIBS)

dnl Check if a version of libpng which supplies png_write_png is available
AC_MSG_CHECKING(whether to use libpng)
AC_ARG_WITH(libpng,
  [  --without-libpng        don't use libpng],
  if test "$withval" = no; then libpng=no; else libpng=yes; fi,
  libpng=yes
)
AC_MSG_RESULT($libpng)
have_libpng="no"
if test "$libpng" = yes; then
  AC_CHECK_HEADER(
    png.h,
    [AC_CHECK_LIB( png, png_write_png,
      [AC_DEFINE([USE_LIBPNG], 1,
        [Defined if we're going to be using the installed libpng])
        PNG_LIBS='-lpng'
        have_libpng="yes"],
      [AC_MSG_WARN(png_write_png not found - PNG output not supported...)],
    )],
    [AC_MSG_WARN(png.h not found - PNG output not supported...)]
  )
fi
AM_CONDITIONAL(HAVE_LIBPNG, test "$have_libpng" = "yes")
AC_SUBST(PNG_LIBS)

dnl Check whether to use libgcrypt
AC_MSG_CHECKING(whether to use libgcrypt)
AC_ARG_WITH(libgcrypt,
[  --without-libgcrypt     don't use libgcrypt],
if test "$withval" = no; then libgcrypt=no; else libgcrypt=yes; fi,
libgcrypt=yes)
AC_MSG_RESULT($libgcrypt)
if test "$libgcrypt" = yes; then
  AC_CHECK_HEADERS(gcrypt.h,LIBS="$LIBS -lgcrypt",libgcrypt=no)
fi
AM_CONDITIONAL(BUILD_RZXCHECK, test "$libgcrypt" = yes)

dnl Do we want lots of warning messages?
AC_MSG_CHECKING(whether lots of warnings requested)
AC_ARG_ENABLE(warnings,
[  --enable-warnings       give lots of warnings if using gcc],
if test "$enableval" = yes; then
    warnings=yes;
else
    warnings=no;
fi,
warnings=no)
AC_MSG_RESULT($warnings)

dnl If it appears we're using gcc as our compiler, turn on warnings
if test "$ac_cv_c_compiler_gnu" = yes; then
    CFLAGS="$CFLAGS -Wall"
    CXXFLAGS="$CXXFLAGS -Wall"
    dnl And possibly lots of warnings
    if test "$warnings" = yes; then
        CFLAGS="$CFLAGS -Wstrict-prototypes -Wmissing-prototypes -Winline -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align -Wwrite-strings -Waggregate-return -W -Wsign-compare"
    fi
fi

dnl Check that libspectrum is available and that it is new enough
PKG_CHECK_MODULES([LIBSPECTRUM], [libspectrum >= 1.4.3])

dnl Check if supplied libspectrum has its own internal GLib replacement
AC_MSG_CHECKING(whether libspectrum has its own internal GLib replacement)
ac_save_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $LIBSPECTRUM_CFLAGS"
AC_PREPROC_IFELSE(
  [AC_LANG_SOURCE([
#include "libspectrum.h"
#if LIBSPECTRUM_HAS_GLIB_REPLACEMENT == 0
#error "GLib replacement not found"
#endif
  ])],
  [fakeglib=yes; AC_MSG_RESULT(yes)],
  [fakeglib=no; AC_MSG_RESULT(no)]
)
CPPFLAGS="$ac_save_CPPFLAGS"

# If we're not libspectrum's own internal GLib replacement, we must use the
# real thing
AS_IF([test "$fakeglib" = no], [
  PKG_CHECK_MODULES(
    [GLIB],
    [glib-2.0 >= 2.0.0],
    [AC_DEFINE([HAVE_LIB_GLIB], 1, [Defined if we've got GLib])
      LIBSPECTRUM_LIBS="$LIBSPECTRUM_LIBS $GLIB_LIBS"],
    [AC_MSG_ERROR([GLib not found, and libspectrum has no internal replacement])]
  )
])

# Look for audiofile (default=yes)
AC_MSG_CHECKING(whether to use audiofile)
AC_ARG_WITH(audiofile,
[  --without-audiofile     don't use audiofile],
if test "$withval" = no; then audiofile=no; else audiofile=yes; fi,
audiofile=yes)
AC_MSG_RESULT($audiofile)
if test "$audiofile" = yes; then
  PKG_CHECK_MODULES(
    AUDIOFILE,
    audiofile,
    AC_DEFINE([HAVE_LIB_AUDIOFILE], 1, [Defined if we've got audiofile]),
    true
  )
  if test -z "$AUDIOFILE_LIBS"; then
    AM_PATH_AUDIOFILE(
      0.2.3,
      AC_DEFINE([HAVE_LIB_AUDIOFILE], 1, [Defined if we've got audiofile]),
      audiofile=no
    )
  fi
fi
AM_CONDITIONAL(BUILD_AUDIOTOOLS, test "$audiofile" = yes)

# Look for iconv (default=yes)
AC_MSG_CHECKING(whether to use iconv)
AC_ARG_WITH(iconv,
[  --without-iconv         don't use iconv],
if test "$withval" = no; then iconv=no; else iconv=yes; fi,
iconv=yes)
AC_MSG_RESULT($iconv)
if test "$iconv" = yes; then
  AC_CHECK_HEADERS(locale.h)
  AC_CHECK_FUNCS(setlocale)
  AM_ICONV()
fi
AC_SUBST(LTLIBICONV)

dnl Work out which compatibility routines to use
AC_MSG_CHECKING(which compatibility routines to use)
case "$host_os" in
  amigaos)
    COMPAT_OSNAME='amiga'
    ;;
  mingw32*)
    COMPAT_OSNAME='win32'
    ;;
  *)
    COMPAT_OSNAME='unix'
  ;;
esac
AC_MSG_RESULT($COMPAT_OSNAME)
AM_CONDITIONAL(COMPAT_AMIGA, test "$COMPAT_OSNAME" = 'amiga')
AM_CONDITIONAL(COMPAT_UNIX, test "$COMPAT_OSNAME" = 'unix')
AM_CONDITIONAL(COMPAT_WIN32, test "$COMPAT_OSNAME" = 'win32')

if test "$ac_cv_func_getopt_long" = no; then
  case "$host_os" in
    amigaos|morphos)
      dnl Our getopt_long replacement doesn't work, so don't use it
      ;;
    *)
      compat_getopt=yes
      missing_routines="$missing_routines"'getopt_long '
      ;;
  esac
fi
AM_CONDITIONAL(COMPAT_GETOPT, test "$compat_getopt" = yes)
if test "$ac_cv_func_mkstemp" = no; then
  missing_routines="$missing_routines"'mkstemp '
fi

AC_CONFIG_FILES([
  Makefile
  fuse-utils.qpg
])

AC_OUTPUT

dnl Status summary
echo ""
echo "***********************************************"
echo "*** The Fuse utils are ready to be compiled ***"
echo "***********************************************"
echo ""
echo "zlib support: $have_zlib"
echo "libjpeg support: $have_libjpeg"
echo "libpng support: $have_libpng"
echo "libgcrypt support: $libgcrypt"
echo "libaudiofile support: $audiofile"
echo "libspectrum's internal GLib replacement: $fakeglib"
echo ""
echo "Type 'make' to compile the Fuse utils"
echo ""
