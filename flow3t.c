/* Mesh plotter */

#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <graph.h>
#include "meshpt.h"

#define RGB( r, g, b ) (0x3f3f3fL & ((long)(b) << 16 | (g) << 8 | (r)))

void main()
{
   char file[40], fil2[40]; /* Data filenames */
   char filt[30],temp[10]; /* Temporary filenames */
   int fnum, k; /* File counter, keypress */
   int p; /* Element in data file to plot (u,v,w,...) */
   long col[64];  /* Colors for palette remapping */
   int i; /* Temporary counter */

   printf( "Enter mesh file: ");
   scanf( "%s", file );

   printf( "Enter data template filename: " );
   scanf( "%s",fil2 );

   printf( "Enter parameter in file: ");
   scanf( "%d", &p );

   printf( "Enter MINIMUM value: ");
   scanf( "%lf", &minv );

   printf( "Enter MAXIMUM value: ");
   scanf( "%lf", &maxv );

   /* Remap the Pallette */
   if( p != -2 )
   {
      col[0] = RGB( 0, 0, 0 );
      col[1] = RGB( 0, 0, 45 );
      col[2] = RGB( 30, 3, 35 );
      col[3] = RGB( 24, 3, 35 );
      col[4] = RGB( 63, 63, 63 );
      col[5] = RGB( 7, 33, 0 );
      col[6] = RGB( 7, 29, 0 );
      col[7] = RGB( 0, 26, 0 );
      col[8] = RGB( 63, 42, 42 );
      col[9] = RGB( 63, 32, 39 );
      col[10] = RGB( 63, 0, 42 );
      col[11] = RGB( 63, 63, 0 );
      col[12] = RGB( 63, 46, 0 );
      col[13] = RGB( 62, 28, 0 );
      col[14] = RGB( 63, 0, 0 );
      col[15] = RGB( 63, 0, 0 );
   }

   _setvideomode( _VRES16COLOR );
   if( p != -2 )
      _remapallpalette( col );
   _setcolor( 15 );

   fnum = 0;
   i = 10;
   while( 1 )
   {
      strcpy( filt, fil2 );
      if( strchr( fil2, '.' ) == NULL )
      {
	 itoa( fnum, temp, 10 );
	 strcat( filt, temp );
	 if( fnum < 0 )
	    fnum = 0;
	 fnum ++;
      }

      if( p == -2 )
	 _clearscreen( _GCLEARSCREEN );

      i = msh_mtx( file, filt, p );

      if( i <= 0 )
      {
	 _clearscreen( _GCLEARSCREEN );
	 printf("End of Data set.");
      }

      k = getch();
      if( k == 'q' )
	 break;

      if( k == ' ' )
	 fnum -= 2;

      if( k == 'v' )
      {
	 _settextposition( 0,0 );
	 scanf("%lf,%lf",&minv,&maxv);
	 fnum --;
      }
   }

   _setvideomode( _DEFAULTMODE );
}

