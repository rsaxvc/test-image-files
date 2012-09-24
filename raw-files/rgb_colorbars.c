#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int img_open( const char * fname_base, int width, int height, const char * fname_ext, int flags )
{
char fname[1000];
snprintf( fname, sizeof( fname ), "%s_%ix%i.%s", fname_base, width, height, fname_ext );
printf("Opening:%s\n", fname );
return open( fname, flags );
}

int machine_is_little_endian( void )
{
const uint8_t lhs = 0x12;
const uint8_t rhs = 0x34;
const uint16_t key = (uint16_t)lhs << 8 | rhs;
return *(uint8_t*)&key != lhs;
}

uint16_t rgb_888_to_565( uint8_t r, uint8_t g, uint8_t b )
{
return ((uint16_t)r>>3)<<11 | ((uint16_t)g>>2)<<5 | ((uint16_t)b>>3)<<0;
}

uint16_t flip_endian_565( uint16_t input )
{
return
	( input & 0xFF00 ) >> 8 |
	( input & 0x00FF ) << 8 ;
}

uint16_t rgb_888_to_565le( uint8_t r, uint8_t g, uint8_t b )
{
uint16_t buf = rgb_888_to_565( r, g, b );
if( !machine_is_little_endian() )
	{
	buf = flip_endian_565( buf );
	}
return buf;
}

uint16_t rgb_888_to_565be( uint8_t r, uint8_t g, uint8_t b )
{
uint16_t buf = rgb_888_to_565( r, g, b );
if( machine_is_little_endian() )
	{
	buf = flip_endian_565( buf );
	}
return buf;
}

int main()
{
int fd_rgb_565_be;
int fd_rgb_565_le;
int fd_rgb_888;
int width = 50;
int height = 150;
int x;
int y;

uint8_t r;
uint8_t g;
uint8_t b;
uint16_t rgb565;

#define FLAGS ( O_RDWR | O_CREAT | O_TRUNC )
#define EXT "data"

fd_rgb_565_le = img_open("colorbars_rgb565le", width, height, EXT, FLAGS );
fd_rgb_565_be = img_open("colorbars_rgb565be", width, height, EXT, FLAGS );
fd_rgb_888    = img_open("colorbars_rgb888"  , width, height, EXT, FLAGS );

for( y = 0; y < height; ++y )
	{
	for( x = 0; x < width; ++x )
		{
		r = ( y < ( height / 3 ) ) ? 0xFF:0;
		write( fd_rgb_888, &r, sizeof( r ) );

		g = ( y > ( height / 3 ) && y < ( 2 * height / 3 ) ) ? 0xFF:0;
		write( fd_rgb_888, &g, sizeof( g ) );

		b = ( y > ( 2 * height / 3 ) ) ? 0xFF:0;
		write( fd_rgb_888, &b, sizeof( b ) );

		rgb565 = rgb_888_to_565le( r, g, b );
		write( fd_rgb_565_le, &rgb565, sizeof( rgb565 ) );

		rgb565 = rgb_888_to_565be( r, g, b );
		write( fd_rgb_565_be, &rgb565, sizeof( rgb565 ) );
		}
	}

close( fd_rgb_888 );
close( fd_rgb_565_le );
close( fd_rgb_565_be );
}
