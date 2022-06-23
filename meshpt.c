/***********************
 * Mesh Plotter Module *
 ***********************/

#include <conio.h>
#include <graph.h>
#include <math.h>
#include <graph.h>
#include <stdio.h>
#include "meshpt.h"

/*********************************************************************
 * Extract a real number from a list starting at a specific byte index
 * Input: str = string from which to extract
 *        i   = byte index in string to start
 * Output: val = real numbered value extracted
 *         i2  = current byte index after extraction
 */
static void extract( char *str, int i, double *val, int *i2 );

/*****************************************************************
 * Extract a integer from a list starting at a specific byte index
 * Input: str = string from which to extract
 *        i   = byte index in string to start
 * Output: val = integer value extracted
 *         i2  = current byte index after extraction
 */
static void extract_i( char *str, int i, int *val, int *i2 );

/***************************************************************
 * Read in and plot mesh model file:
 * Input: file = mesh model file to read
 *        fil2 = data file to read
 *        ple  = Parameter in data file to use
 * Output: none
 * Returns: number of elements read, <= 0 if error
 */
int msh_mtx( char *file, char *fil2, int ple )
{
   FILE *fp, *fp2; /* Pointers to data files */
   int i, j;   /* Element and Face indexes */
   int col;  /* Color of element */
   int nodes; /* Max nodes of element */
   int cnt, tempi; /* Number of elements read, temporary integer */
   int i2, i3, i4, i5; /* Temporary indexes */
   double tempv;  /* Temporary real number */
   double d[7];   /* Data in from data file */
   double u, v;   /* Vector for windvane */
   char ln[80];   /* One line read from mesh file */
   char ld[80];   /* One line read from data file */
   poly p;        /* Polygonal representation of element */

   scal_mtx( file );

   /* Open data files */
   if( ( fp = fopen( file, "r" ) ) == NULL ) /* Mesh File */
   {
      /* File error */
      return( 0 );
   }
   if( ( fp2 = fopen( fil2, "r" ) ) == NULL ) /* Data file */
   {
      /* File error */
      return( 0 );
   }

   /* Read and process loop */
   cnt = 0;
   while( !feof( fp ) )
   {
      /* Index in line is 0 */
      i2 = 0;
      if( fgets( ln, 80, fp ) == NULL )
	 break;

      /* Extract Index */
      extract_i( ln, i2, &tempi, &i3 );
      i2 = i3;
      i = tempi - 1;

      /* Get data file's record for this element */
      d[0] = i + 1;
      while( ( (int) d[0] ) != i )
      {
	 /* Get the line */
	 if( fgets( ld, 80, fp2 ) == NULL )
	    break;

	 /* Parse the line */
	 sscanf( ld, "%lf,%lf,%lf,%lf,%lf,%lf,%lf\n",&d[0],&d[1],&d[2], \
		 &d[3],&d[4],&d[5],&d[6] );

	 tempv = d[ple];
	 tempv -= minv;
	 if( ple > 0 )
	 {
	    tempv = 15. * tempv / maxv;
	    col = (int) tempv;
	 }
	 if( ple < 0 )
	 {
	    /* Total velocity */
	    tempv = sqrt( d[1] * d[1] + d[2] * d[2] );
	    tempv -= minv;
	    tempv = 15. * tempv / maxv;
	    col = (int) tempv;
	    u = d[1], v = d[2];
	 }

	 _setcolor( col );
      }

      /* Extract Shape */
      extract_i( ln, i2, &tempi, &i3 );
      i2 = i3;

      /* Extract number of nodes */
      extract_i( ln, i2, &tempi, &i3 );
      i2 = i3;
      nodes = tempi;
      p.nodes = nodes;

      /* Extract number of faces */
      extract_i( ln, i2, &tempi, &i3 );
      i2 = i3;

      /* Extract each node */
      for( i5 = 0; i5 < 3; i5 ++ )
      {
	 i2 = 0;
	 fgets( ln, 80, fp );
	 for( i4 = 0; i4 < nodes; i4 ++ )
	 {
	    extract( ln, i2, &tempv, &i3 );
	    i2 = i3;
	    p.p[i4][i5] = tempv;
	 }
      }

      /* See if max/min values of points have changed */

      i2 = 0;
      fgets( ln, 80, fp );
      /* Extract connectivity matrix */
      for( i4 = 0; i4 < nodes; i4 ++ )
      {
	 extract_i( ln, i2, &tempi, &i3 );
	 i2 = i3;
      }

      /* Set Present byte on element */
      p.pres = 1;

      if( ple != -2 )
	 draw_e( p );
      else
      {
	 _setcolor( 15 );
	 draw_w( p, u, v );
      }
      cnt ++;
      if( kbhit() )
	 return( cnt );
   }

   /* Close files on completion */
   fclose( fp );
   fclose( fp2 );
   printf("\a");
   return( cnt );
}

/****************************************************************
 * Draw an element on the screen
 * Input: p = element to draw
 * Output: none
 * Returns: none, void.
 */
void draw_e( poly p )
{
   struct xycoord xyc[MAX_F];
   int i; /* Index on node point */
   int x, y; /* Temporary screen x and y coord */
   int x0, y0; /* First screen x and y coord */
   double tv1, tv2; /* Temporary real values */

   /* Don't draw an element that doesn't exist */
   if( !p.pres )
     return;

   for( i = 0; i < p.nodes; i ++ )
   {
      tv1 = p.p[i][X] - min_x;
      tv2 = max_x - min_x;
      x = ( tv1 / tv2 ) * 640;
      if( x <= 0 )
	 x = 1;
      if( x >= 640 )
	 x = 639;

      tv1 = p.p[i][Y] - min_y;
      tv2 = max_y - min_y;
      y = 480 - ( tv1 / tv2 ) * 480;
      if( y <= 0 )
	 y = 1;
      if( y >= 480 )
	 y = 479;

      xyc[i].xcoord = x;
      xyc[i].ycoord = y;

   }

   /* Draw the element */
   if( minv != maxv )
      _polygon( _GFILLINTERIOR, xyc, p.nodes );
   else
   {
      i = _getcolor();
      _setcolor( 15 );
      _polygon( _GBORDER, xyc, p.nodes );
      _setcolor( i );
   }
}

/****************************************************************
 * Draw an element on the screen as a windvane
 * Input: p   = element to draw
 *        u,v = vector of velocity
 * Output: none
 * Returns: none, void.
 */
void draw_w( poly p, double u, double v )
{
   int i; /* Index on node point */
   int x, y; /* screen x and y coord of tail */
   int x0, y0; /* screen x and y coord of head */
   double tv1, tv2; /* Temporary real values */

   /* Don't draw an element that doesn't exist */
   if( !p.pres )
     return;

   /* Calculate x values */
   tv1 = p.p[0][X] - min_x;
   tv2 = max_x - min_x;
   x = ( tv1 / tv2 ) * 640;
   if( x <= 0 )
      x = 1;
   if( x >= 640 )
      x = 639;
   x0 = u / maxv;

   /* Calculate y values */
   tv1 = p.p[0][Y] - min_y;
   tv2 = max_y - min_y;
   y = 480 - ( tv1 / tv2 ) * 480;
   if( y <= 0 )
      y = 1;
   if( y >= 480 )
      y = 479;
   y0 = v / maxv;

   /* Draw the vane */
   _moveto( x,y );
   _lineto( x0 + x,y - y0 );
   _rectangle( _GBORDER, x - 1,y - 1, x + 1,y + 1 );
}

/*********************************************************************
 * Extract a real number from a list starting at a specific byte index
 * Input: str = string from which to extract
 *        i   = byte index in string to start
 * Output: val = real numbered value extracted
 *         i2  = current byte index after extraction
 */
static void extract( char *str, int i, double *val, int *i2 )
{
   int i3, ind;  /* Temporary indexes */
   char tmp[20], chk; /* Temporary string, flag: if number has
					   started = 1, else = 0 */

   ind = i;

   i3 = 0;
   chk = 0;
   for( ; ind < 80; ind ++ )
   {
      if( str[ind] == ' ' && !chk )
	 continue;
      tmp[i3] = str[ind];
      if( isdigit( tmp[i3] ) )
	 chk = 1;
      if( ( tmp[i3] == ' ' && chk ) || ( tmp[i3] == '\n' ) || \
	  ( tmp[i3] == '\0' ) )
      {
	 tmp[i3] = '\0';
	 sscanf( tmp, "%lf", val );
	 break;
      }
      i3 ++;
   }

   *i2 = ind + 1;
}

/*****************************************************************
 * Extract a integer from a list starting at a specific byte index
 * Input: str = string from which to extract
 *        i   = byte index in string to start
 * Output: val = integer value extracted
 *         i2  = current byte index after extraction
 */
static void extract_i( char *str, int i, int *val, int *i2 )
{
   int i3, ind;  /* Temporary indexes */
   char tmp[20], chk; /* Temporary string, flag: if number has
					   started = 1, else = 0 */

   ind = i;
   chk = 0;
   i3 = 0;
   for( ; ind < 80; ind ++ )
   {
      if( str[ind] == ' ' && !chk )
	 continue;
      tmp[i3] = str[ind];
      if( isdigit( tmp[i3] ) )
	 chk = 1;
      if( ( tmp[i3] == ' ' && chk ) || ( tmp[i3] == '\n' ) || \
	  ( tmp[i3] == '\0' ) )
      {
	 tmp[i3] = '\0';
	 sscanf( tmp, "%d", val );
	 break;
      }
      i3 ++;
   }

   *i2 = ind + 1;
}

/***************************************************************
 * Scale screen to fit file
 * Input: file = mesh model file to read
 * Output: none
 * Returns: number of elements read, <= 0 if error
 */
int scal_mtx( char *file )
{
   FILE *fp; /* Pointer to file */
   int i, j;   /* Element and Face indexes */
   int nodes; /* Max nodes of element */
   int cnt, tempi; /* Number of elements read, temporary integer */
   int i2, i3, i4, i5; /* Temporary indexes */
   double tempv;  /* Temporary real number */
   char ln[80];   /* One line read from file */
   poly p;

   /* Start max and min values at 0 */
   max_x = 0.;
   min_x = 0.;
   max_y = 0.;
   min_y = 0.;

   if( ( fp = fopen( file, "r" ) ) == NULL )
   {
      /* File error */
      return( 0 );
   }

   /* Read and process loop */
   cnt = 0;
   while( !feof( fp ) )
   {
      /* Index in line is 0 */
      i2 = 0;
      if( fgets( ln, 80, fp ) == NULL )
	 break;

      /* Extract Index */
      extract_i( ln, i2, &tempi, &i3 );
      i2 = i3;
      i = tempi - 1;

      /* Extract Shape */
      extract_i( ln, i2, &tempi, &i3 );
      i2 = i3;

      /* Extract number of nodes */
      extract_i( ln, i2, &tempi, &i3 );
      i2 = i3;
      nodes = tempi;
      p.nodes = nodes;

      /* Extract number of faces */
      extract_i( ln, i2, &tempi, &i3 );
      i2 = i3;

      /* Extract each node */
      for( i5 = 0; i5 < 3; i5 ++ )
      {
	 i2 = 0;
	 fgets( ln, 80, fp );
	 for( i4 = 0; i4 < nodes; i4 ++ )
	 {
	    extract( ln, i2, &tempv, &i3 );
	    i2 = i3;
	    p.p[i4][i5] = tempv;
	 }
      }

      /* See if max/min values of points have changed */
      for( i4 = 0; i4 < nodes; i4 ++ )
      {
	 if( p.p[i4][X] > max_x )
	    max_x = p.p[i4][X];
	 if( p.p[i4][X] < min_x )
	    min_x = p.p[i4][X];
	 if( p.p[i4][Y] > max_y )
	    max_y = p.p[i4][Y];
	 if( p.p[i4][Y] < min_y )
	    min_y = p.p[i4][Y];
      }

      i2 = 0;
      fgets( ln, 80, fp );
      /* Extract connectivity matrix */
      for( i4 = 0; i4 < nodes; i4 ++ )
      {
	 extract_i( ln, i2, &tempi, &i3 );
	 i2 = i3;
      }

      /* Set Present byte on element */
      p.pres = 1;

      cnt ++;
   }

   /* Close file on completion */
   fclose( fp );
   return( cnt );
}

