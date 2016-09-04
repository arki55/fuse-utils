/* rzxdump.c: get info on an RZX file
   Copyright (c) 2002-2015 Philip Kendall, Sergio Baldovi

   $Id$

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

   E-mail: philip-fuse@shadowmagic.org.uk

*/

#include <config.h>

#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#ifdef HAVE_GCRYPT_H
#include <gcrypt.h>
#endif				/* #ifdef HAVE_GCRYPT_H */

#include <libspectrum.h>

#include "utils.h"

#define PROGRAM_NAME "rzxdump"

char *progname;

const char *rzx_signature = "RZX!";

size_t block_number;

libspectrum_word read_word( unsigned char **ptr );
libspectrum_dword read_dword( unsigned char **ptr );

static int
do_file( const char *filename );

static int
read_creator_block( unsigned char **ptr, unsigned char *end );
static int
read_snapshot_block( unsigned char **ptr, unsigned char *end );
static int
read_input_block( unsigned char **ptr, unsigned char *end );
static int read_sign_start_block( unsigned char **ptr, unsigned char *end );
static int read_sign_end_block( unsigned char **ptr, unsigned char *end );

libspectrum_word
read_word( unsigned char **ptr )
{
  libspectrum_word result = (*ptr)[0] + (*ptr)[1] * 0x100;
  (*ptr) += 2;
  return result;
}

libspectrum_dword
read_dword( unsigned char **ptr )
{
  libspectrum_dword result = (*ptr)[0]             +
                             (*ptr)[1] *     0x100 +
                             (*ptr)[2] *   0x10000 +
                             (*ptr)[3] * 0x1000000;
  (*ptr) += 4;
  return result;
}

static void
show_version( void )
{
  printf(
    PROGRAM_NAME " (" PACKAGE ") " PACKAGE_VERSION "\n"
    "Copyright (c) 2002-2014 Philip Kendall\n"
    "License GPLv2+: GNU GPL version 2 or later "
    "<http://gnu.org/licenses/gpl.html>\n"
    "This is free software: you are free to change and redistribute it.\n"
    "There is NO WARRANTY, to the extent permitted by law.\n" );
}

static void
show_help( void )
{
  printf(
    "Usage: %s [OPTION] <rzxfile>...\n"
    "List contents of Sinclair ZX Spectrum input recording files.\n"
    "\n"
    "Options:\n"
    "  -h, --help     Display this help and exit.\n"
    "  -V, --version  Output version information and exit.\n"
    "\n"
    "Report %s bugs to <%s>\n"
    "%s home page: <%s>\n"
    "For complete documentation, see the manual page of %s.\n",
    progname,
    PROGRAM_NAME, PACKAGE_BUGREPORT, PACKAGE_NAME, PACKAGE_URL, PROGRAM_NAME
  );
}

int main( int argc, char **argv )
{
  int i, c;
  int error = 0;

  struct option long_options[] = {
    { "help", 0, NULL, 'h' },
    { "version", 0, NULL, 'V' },
    { 0, 0, 0, 0 }
  };

  progname = argv[0];

  while( ( c = getopt_long( argc, argv, "hV", long_options, NULL ) ) != -1 ) {

    switch( c ) {

    case 'h':
      show_help();
      return 0;

    case 'V':
      show_version();
      return 0;

    case '?':
      /* getopt prints an error message to stderr */
      error = 1;
      break;

    default:
      error = 1;
      fprintf( stderr, "%s: unknown option `%c'\n", progname, (char) c );
      break;

    }
  }
  argc -= optind;
  argv += optind;

  if( error ) {
    fprintf( stderr, "Try `%s --help' for more information.\n", progname );
    return error;
  }

  if( argc < 1 ) {
    fprintf( stderr, "%s: Usage: %s <rzx file(s)>\n", progname, progname );
    fprintf( stderr, "Try `%s --help' for more information.\n", progname );
    return 1;
  }

  for( i = 0; i < argc; i++ ) {
    error = do_file( argv[i] );
    if( error ) return error;
  }    

  return 0;
}

static int
do_file( const char *filename )
{
  unsigned char *buffer, *ptr, *end; size_t length;
  int error;

  printf( "Examining file %s\n", filename );

  error = read_file( filename, &buffer, &length ); if( error ) return error;

  ptr = buffer; end = buffer + length;

  /* Read the RZX header */

  if( end - ptr < (ptrdiff_t)strlen( rzx_signature ) + 6 ) {
    fprintf( stderr,
	     "%s: Not enough bytes for RZX header (%ld bytes)\n",
	     progname, (unsigned long)strlen( rzx_signature ) + 6 );
    free( buffer );
    return 1;
  }

  if( memcmp( ptr, rzx_signature, strlen( rzx_signature ) ) ) {
    fprintf( stderr, "%s: Wrong signature: expected `%s'\n", progname,
	     rzx_signature );
    free( buffer );
    return 1;
  }

  printf( "Found RZX signature\n" );
  ptr += strlen( rzx_signature );
  block_number = 0;

  printf( "  Major version: %d\n", (int)*ptr++ );
  printf( "  Minor version: %d\n", (int)*ptr++ );
  printf( "  Flags: %d\n", read_dword( &ptr ) );

  while( ptr < end ) {

    unsigned char id;

    id = *ptr++;

    switch( id ) {

    case 0x10: error = read_creator_block( &ptr, end ); break; 
    case 0x20: error = read_sign_start_block( &ptr, end ); break;
    case 0x21: error = read_sign_end_block( &ptr, end ); break;
    case 0x30: error = read_snapshot_block( &ptr, end ); break;
    case 0x80: error = read_input_block( &ptr, end ); break;

    default:
      fprintf( stderr, "%s: Unknown block type 0x%02x at offset %ld\n",
               progname, id, ptr - buffer - 1 );
      free( buffer );
      return 1;

    }

    if( error ) { free( buffer ); return 1; }
  }

  free( buffer );

  return 0;
}

static void
print_canonical_version( const char *creator, libspectrum_word major_version,
                         libspectrum_word minor_version )
{
  printf( "  Creator canonical version: " );

  if( !strncmp( creator, "Fuse", strlen( "Fuse" ) ) ||
      !strncmp( creator, "rzxtool", strlen( "rzxtool" ) )) {
    printf( "%d.%d.%d.%d\n", major_version / 0x100, major_version % 0x100,
            minor_version / 0x100, minor_version % 0x100 );
  } else if( !strncmp( creator, "Spectaculator", strlen( "Spectaculator" ) ) ) {
    printf( "%d.%d.%d\n", major_version / 10, major_version % 10,
            minor_version );
  } else if( !strncmp( creator, "RealSpectrum", strlen( "RealSpectrum" ) ) ) {
    printf( "%x.%x\n", major_version, minor_version );
  } else {
    /* Simple decimal version (EmuZWin, SpecEmu, SPIN) */
    printf( "%d.%d\n", major_version, minor_version );
  }
}

static int
read_creator_block( unsigned char **ptr, unsigned char *end )
{
  size_t length;
  libspectrum_word major_version, minor_version;
  unsigned char *creator;

  if( end - *ptr < 28 ) {
    fprintf( stderr,
	     "%s: Not enough bytes for creator block\n", progname );
    return 1;
  }

  printf( "Found a creator block\n" );

  length = read_dword( ptr );
  printf( "  Length: %ld bytes\n", (unsigned long)length );
  creator = *ptr; (*ptr) += 20;
  major_version = read_word( ptr );
  minor_version = read_word( ptr );
  printf( "  Creator: `%s'\n", creator );
  printf( "  Creator major version: %d\n", major_version );
  printf( "  Creator minor version: %d\n", minor_version );
  print_canonical_version( (char *)creator, major_version, minor_version );
  printf( "  Creator custom data: %ld bytes\n", (unsigned long)length - 29 );
  (*ptr) += length - 29;

  return 0;
}

static void
print_ascii_string( unsigned char *data, size_t length )
{
  unsigned int i;

  for( i = 0; i < length; i++) {
    unsigned char c;

    if( data[i] >= 32 && data[i] < 127 ) {
      c = data[i];
    } else {
      c = '?';
    }

    printf( "%c", c );
  }
}

static int
read_snapshot_block( unsigned char **ptr, unsigned char *end )
{
  size_t block_length, path_length;
  libspectrum_dword flags;

  if( end - *ptr < 16 ) {
    fprintf( stderr,
	     "%s: Not enough bytes for snapshot block\n", progname );
    return 1;
  }

  printf( "Found a snapshot block\n" );

  printf( "  Block: %lu\n", (unsigned long)block_number ); block_number++;

  block_length = read_dword( ptr );
  printf( "  Length: %lu bytes\n", (unsigned long)block_length );

  flags = read_dword( ptr );
  printf( "  Flags: %lu\n", (unsigned long)flags );
  printf( "  Snapshot extension: `%s'\n", *ptr ); (*ptr) += 4;
  printf( "  Snap length: %d bytes\n", read_dword( ptr ) );

  /* Snapshot descriptor */
  if( ( flags & 0x01 ) && !( flags & 0x02 ) ) {
    if( end - *ptr < 5 ) {
      fprintf( stderr, "%s: Not enough bytes for snapshot block\n", progname );
      return 1;
    }

    printf( "  Checksum: 0x%08x\n", read_dword( ptr ) );

    printf( "  Snap path: " );
    path_length = block_length - 17 - 4;
    print_ascii_string( *ptr, path_length - 1 );
    printf( "\n" );

    (*ptr) += path_length;
    return 0;
  }

  /* Snapshot data */
  (*ptr) += block_length - 17;

  return 0;
}

static int
read_input_block( unsigned char **ptr, unsigned char *end )
{
  size_t i, frames, length; int flags;

  if( end - *ptr < 17 ) {
    fprintf( stderr,
	     "%s: Not enough bytes for input recording block\n", progname );
    return 1;
  }

  printf( "Found an input recording block\n" );

  printf( "  Block: %lu\n", (unsigned long)block_number ); block_number++;
  
  length = read_dword( ptr );
  printf( "  Length: %ld bytes\n", (unsigned long)length );

  frames = read_dword( ptr );
  printf( "  Frame count: %ld\n", (unsigned long)frames );

  printf( "  Frame length (obsolete): %d bytes\n", *(*ptr)++ );
  printf( "  Tstate counter: %d\n", read_dword( ptr ) );

  flags = read_dword( ptr );
  printf( "  Flags: %d\n", flags );

  /* Check there's enough data */
  if( end - *ptr < (off_t)length - 18 ) {
    fprintf( stderr, "%s: not enough frame data\n", progname );
    return 1;
  }

  if( flags & 0x02 ) {		/* Data is compressed */
    printf( "  Skipping compressed data\n" );
    (*ptr) += length - 18;

  } else {			/* Data is not compressed */

    for( i=0; i<frames; i++ ) {

      size_t count;

      printf( "Examining frame %ld\n", (unsigned long)i );
    
      if( end - *ptr < 4 ) {
	fprintf( stderr, "%s: Not enough data for frame %ld\n", progname,
		 (unsigned long)i );
	return 1;
      }

      printf( "  Instruction count: %d\n", read_word( ptr ) );

      count = read_word( ptr );
      if( count == 0xffff ) {
	printf( "  (Repeat last frame's INs)\n" );
	continue;
      }

      printf( "  IN count: %ld\n", (unsigned long)count );

      if( end - *ptr < (ptrdiff_t)count ) {
	fprintf( stderr,
		 "%s: Not enough data for frame %ld (expected %ld bytes)\n",
		 progname, (unsigned long)i, (unsigned long)count );
	return 1;
      }

      (*ptr) += count;
    }

  }

  return 0;
}

static int
read_sign_start_block( unsigned char **ptr, unsigned char *end )
{
  size_t length;

  if( end - *ptr < 8 ) {
    fprintf( stderr, "%s: not enough bytes for sign start block\n", progname );
    return 1;
  }

  printf( "Found a signed data start block\n" );

  length = read_dword( ptr );

  printf( "  Length: %ld bytes\n", (unsigned long)length );
  printf( "  Key ID: 0x%08x\n", (unsigned int)read_dword( ptr ) );
  printf( "  Week code: 0x%08x\n", (unsigned int)read_dword( ptr ) );

  (*ptr) += length - 13;

  return 0;
}

static int
read_sign_end_block( unsigned char **ptr, unsigned char *end )
{
  size_t length;
#ifdef HAVE_GCRYPT_H
  gcry_mpi_t a; int error; size_t length2;
  unsigned char *buffer;
#endif				/* #ifdef HAVE_GCRYPT_H */

  if( end - *ptr < 4 ) {
    fprintf( stderr, "%s: not enough bytes for sign end block\n", progname );
    return 1;
  }

  printf( "Found a signed data end block\n" );

  length = read_dword( ptr );
  printf( "  Length: %ld bytes\n", (unsigned long)length );

  length -= 5;

  if( end - *ptr < (ptrdiff_t)length ) {
    fprintf( stderr, "%s: not enough bytes for sign end block\n", progname );
    return 1;
  }

#ifdef HAVE_GCRYPT_H

  error = gcry_mpi_scan( &a, GCRYMPI_FMT_PGP, *ptr, length, &length2 );
  if( error ) {
    fprintf( stderr, "%s: error reading r: %s\n", progname,
	     gcry_strerror( error ) );
    return 1;
  }
  *ptr += length2; length -= length2;

  error = gcry_mpi_aprint( GCRYMPI_FMT_HEX, &buffer, NULL, a );
  printf( "  r: %s\n", buffer );
  free( buffer ); gcry_mpi_release( a );

  error = gcry_mpi_scan( &a, GCRYMPI_FMT_PGP, *ptr, length, &length2 );
  if( error ) {
    fprintf( stderr, "%s: error reading s: %s\n", progname,
	     gcry_strerror( error ) );
    return 1;
  }
  *ptr += length2; length -= length2;

  error = gcry_mpi_aprint( GCRYMPI_FMT_HEX, &buffer, NULL, a );
  printf( "  s: %s\n", buffer );
  free( buffer ); gcry_mpi_release( a );

#endif				/* #ifdef HAVE_GCRYPT_H */

  (*ptr) += length;

  return 0;
}
