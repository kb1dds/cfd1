/* Flow Test */
#include <graph.h>
#include <conio.h>
#include "flow1.h"

void init( sgrid g );
void sflow( sgrid g );
void dgrid( sgrid g );

void main()
{
   int i, i2, ind, col, iter;
   sgrid g;

   /* Reset the grid */
   z_grid( g );
   init( g );
   sflow( g );

   iter = 0;
   _setvideomode( _VRES16COLOR );
   while( 1 )
   {
      if( iter >= 4 )
	 iter=iter;
      flux_m( g );
      flux_use( g );
      dgrid( g );
      if( getch() == 'q' )
	 break;
      iter ++;
   }
   _setvideomode( _DEFAULTMODE );
}

void init( sgrid g )
{
   int i, i2;

   for( i = 0; i < GRID_X*GRID_Y; i ++ )
   {
      for( i2 = 0; i2 < 4; i2 ++ )
	 g[i].rho[i2] = 10.;
   }
}

void sflow( sgrid g )
{
   int i;

   for( i = 0; i < GRID_X*GRID_Y; i += GRID_X )
   {
      g[i].u[1] = 10.;
      g[i].u[3] = -10.;
      g[i].p[1] = 10.;
      g[i].p[3] = 10.;
      g[i].rho[1] = 10.;
      g[i].rho[3] = 10.;
   }
}

void dgrid( sgrid g )
{
   int i, i2, ind, col;

   for( i = 0; i < GRID_Y; i ++ )
   {
      for( i2 = 0; i2 < GRID_X; i2 ++ )
      {
	 ind = i * GRID_Y + i2;
	 col = (int) ( g[ind].p[0]+g[ind].p[1]+g[ind].p[2]+g[ind].p[3] )/4;
	 col /= 100;
	 _setcolor( col );

	 _rectangle( _GFILLINTERIOR, i2 * 64,i * 48, \
				     ( i2 + 1 ) * 64,( i + 1 ) * 48 );
      }
   }
}
