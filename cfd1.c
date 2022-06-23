/******************************************
 * CFD MODULE FILE                        *
 * Note: for definitions, see "CFD1.H"    *
 ******************************************/

#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include "cfd1.h"

/* Extract a number from a list
 * Input: str = string from which to extract
 *        i   = index in string to start
 * Output: val = value extracted
 *         i2  = current index after extraction
 */
static void extract( char *str, int i, double *val, int *i2 );

/* Extract a number from a list
 * Input: str = string from which to extract
 *        i   = index in string to start
 * Output: val = value extracted
 *         i2  = current index after extraction
 */
static void extract_i( char *str, int i, int *val, int *i2 );

/* Distance between p1 and p2 is <d> */
static void dist( pt p1, pt p2, double *d );

/* Calculate phi-bar for an element */
static void b_cond( elem e[],             /* I: Mesh of elements */
		    conn c1[MAX_E],       /* I: Connection matrix */
		    double *ubar,         /* O: Average x-velocity */
		    double *vbar,         /* O: Average y-velocity */
		    double *pbar,         /* O: Average pressure */
		    double *rhobar,       /* O: Average density */
		    double *hbar,         /* O: Average Energy */
		    int i,                /* I: Element number */
		    int j                 /* I: face number */
		    );

/*--------------------------------------+
 | Functions and Routines               |
 +--------------------------------------*/

/***************************************************************
 * Read in a mesh model file:
 * Input: file = mesh model file to read
 * Output: e  = array of elements, length less than or equal to MAX_E
 *         c1 = connectivity map
 * Returns: number of elements read, <= 0 if error
 */
int read_msh( char *file, elem e[], conn c1[MAX_E] )
{
   FILE *fp; /* Pointer to file */
   int i, j;   /* Element and Face indexes */
   int nodes, faces, shape; /* Max nodes, faces, and type of element */
   int cnt, tempi; /* Number of elements read, temporary integer */
   int i2, i3, i4, i5; /* Temporary indexes */
   double tempv;  /* Temporary real number */
   char ln[80];   /* One line read from file */
   pt xc[MAX_F];  /* Node points of element */

   /* Mark all elements as "not present" */
   for( i = 0; i < MAX_E; i ++ )
      e[i].pres = 0;

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
      shape = tempi;

      /* Extract number of nodes */
      extract_i( ln, i2, &tempi, &i3 );
      i2 = i3;
      nodes = tempi;

      /* Extract number of faces */
      extract_i( ln, i2, &tempi, &i3 );
      i2 = i3;
      faces = tempi;

      /* Extract each node */
      for( i5 = 0; i5 < 3; i5 ++ )
      {
	 i2 = 0;
	 fgets( ln, 80, fp );
	 for( i4 = 0; i4 < nodes; i4 ++ )
	 {
	    extract( ln, i2, &tempv, &i3 );
	    i2 = i3;
	    xc[i4][i5] = tempv;
	 }
      }

      i2 = 0;
      fgets( ln, 80, fp );
      /* Extract connectivity matrix */
      for( i4 = 0; i4 < faces; i4 ++ )
      {
	 extract_i( ln, i2, &tempi, &i3 );
	 i2 = i3;
	 c1[i][i4] = tempi;
      }

      /* Now that data is all in, process it */
      e[i].nodes = nodes;
      calc_A( e[i].A, nodes, xc );
      calc_V( &e[i].V, nodes, xc );

      /* Initialize some data elements */
      e[i].pres = 1;
      e[i].F[X] = 0., e[i].F[Y] = 0., e[i].F[Z] = 0.;

      /* Convert data to standardized format */
      remap_f( faces, e[i].A );

      cnt ++;
   }

   /* Close file on completion */
   fclose( fp );
   return( cnt );
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
      if( ( tmp[i3] == ' ' && chk ) || ( tmp[i3] == '\n' ) ||
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
      if( ( tmp[i3] == ' ' && chk ) || ( tmp[i3] == '\n' ) ||
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

/*****************************************************************
 * Calculate the Area vectors:
 * Input: nodes = number of nodes
 *        xc    = point of each node
 * Output: A = array of area vectors
 * Returns: void, none
 */
void calc_A( vect A[MAX_F], int nodes, pt xc[MAX_F] )
{
   int j;    /* Face counter */
   double sA, sN;   /* Scalar length of A and N (before normalizing) */
   double xl, yl, dp; /* Delta-x, delta-y, dot product of N and C */
   pt c, m, p1, p2; /* c,m: centroid vector endpoints; p1,p2: endpoints
							    of vector A */
   vect n, cv;  /* Vector n (normalized A), and centroid vector */

   /* Calculate the centroid point */
   xl = 0, yl = 0;
   for( j = 0; j < nodes; j ++ )
      xl += xc[j][X], yl += xc[j][Y];
   xl /= nodes, yl /= nodes;
   c[X] = xl, c[Y] = yl, c[Z] = 0.;

   for( j = 1; j < nodes + 1; j ++ )
   {
      /* Store endpoints of vector */
      if( j != nodes )
      {
	 p1[X] = xc[j][X], p2[X] = xc[j-1][X];
	 p1[Y] = xc[j][Y], p2[Y] = xc[j-1][Y];
	 p1[Z] = xc[j][Z], p2[Z] = xc[j-1][Z];
      }
      else
      {
	 p1[X] = xc[j-1][X], p2[X] = xc[0][X];
	 p1[Y] = xc[j-1][Y], p2[Y] = xc[0][Y];
	 p1[Z] = xc[j-1][Z], p2[Z] = xc[0][Z];
      }

      /* Calculate scalar distance of vector */
      dist( p1, p2, &sA );

      /* Calculate normalized vector n for side */
      xl = p1[X] - p2[X];
      yl = p1[Y] - p2[Y];
      n[X] = yl;
      n[Y] = - xl;
      n[Z] = 0.;

      /* normalize . . .*/
      sN = sqrt( n[X] * n[X] + n[Y] * n[Y] );
      n[X] /= sN, n[Y] /= sN, n[Z] /= sN;

      /** Point vector outwards */
      /* midpoint of face */
      m[X] = ( p1[X] + p2[X] ) / 2.;
      m[Y] = ( p1[Y] + p2[Y] ) / 2.;

      /* Centroid vector */
      cv[X] = m[X] - c[X];
      cv[Y] = m[Y] - c[Y];

      /* Dot product */
      dp = cv[X] * n[X] + cv[Y] * n[Y];
      if( dp < 0 )
	 n[X] = - n[X], n[Y] = -n[Y];

      /* New length of vector */
      n[X] *= sA;
      n[Y] *= sA;
      n[Z] = 0.;
      A[j-1][X] = n[X];
      A[j-1][Y] = n[Y];
      A[j-1][Z] = n[Z];
   }
}

/*****************************************************************
 * Calculate the volume of an element
 * Input: nodes = number of nodes
 *        xc    = array of nodes
 * Output: V = volume of element
 * Returns: none, void
 */
void calc_V( double *V, int nodes, pt xc[MAX_F] )
{
   double a, b, c, d, s, p, q;  /* Variables used in volume finding */
   if( nodes == 3 )
   {
      /* Triangle */
      /* Get lengths of each side */
      dist( xc[0], xc[1], &a );
      dist( xc[1], xc[2], &b );
      dist( xc[2], xc[0], &c );

      /* Calculate Volume */
      s = .5 * ( a + b + c );
      *V = sqrt( s * ( s - a ) * ( s - b ) * ( s - c ) );
   }
   if( nodes == 4 )
   {
      /* Quadrilateral */
      /* Get the lengths of each side and diagonal */
      dist( xc[0], xc[1], &a );
      dist( xc[1], xc[2], &b );
      dist( xc[2], xc[3], &c );
      dist( xc[3], xc[0], &d );
      dist( xc[0], xc[2], &p );
      dist( xc[1], xc[3], &q );

      /* Calculate Volume */
      s =  ( b * b + d * d - a * a - c * c );
      s *= s;
      *V = .25 * sqrt( 4 * p * q * p * q - s );
   }
}

/*****************************************************************
 * Distance Formula
 * Inputs: p1 = first point
 *         p2 = second point
 * Outputs: d = distance between p1 and p2
 * Returns: none, void.
 */
static void dist( pt p1, pt p2, double *d )
{
   double xl, yl;  /* Delta-x and Delta-y in the distance formula */
   xl = p1[X] - p2[X];
   yl = p1[Y] - p2[Y];
   *d = sqrt( xl * xl + yl * yl );
}


/*****************************************************************
 * Remap Raw face data to standard face data
 * Input: faces = number of faces
 *        A    = Raw Area vectors
 * Output: A = Standard area vectors
 * Returns: none, void
 * Notes: This routine converts the area vector format from a
 *        temporary format used only during loading of a mesh
 *        to a standarized one used throughout all other parts
 *        of the program
 */
void remap_f( int faces, vect A[MAX_F] )
{
   int j;      /* Index of Face */
   vect t[MAX_F]; /* Temporary vector for transferring */

   /* Determine whether source is a quad or a triangle */
   if( faces == 3 ) /* Triangle */
   {
      /* Mapping pattern is:
       * Original format --> Standard format
       * 0 --> 2
       * 1 --> 0
       * 2 --> 1
       */
      v_copy( A[0], t[2] );
      v_copy( A[1], t[0] );
      v_copy( A[2], t[1] );
   }
   if( faces == 4 ) /* Quadrilateral */
   {
      /* Mapping pattern is:
       * Original format --> Standard format
       * 0 --> 2
       * 1 --> 3
       * 2 --> 0
       * 3 --> 1
       */
      v_copy( A[0], t[2] );
      v_copy( A[1], t[3] );
      v_copy( A[2], t[0] );
      v_copy( A[3], t[1] );
   }

   /* Copy temp vector onto actual vector */
   for( j = 0; j < MAX_F; j ++ )
   {
      v_copy( t[j], A[j] );
   }
}

/*****************************************************************
 * Copy a vector
 * Input: vi = input vector
 * Output: vo = output vector
 * Returns: none,viod
 */
void v_copy( vect vi, vect vo )
{
   vo[X] = vi[X];
   vo[Y] = vi[Y];
   vo[Z] = vi[Z];
}

/*****************************************************************
 * Dot product of two vectors
 * Input: v1, v2 = two vectors to get dot product of
 * Output: dp = the dot product
 */
void v_dot( vect v1, vect v2, double *dp )
{
   *dp = v1[X] * v2[X] + v1[Y] * v2[Y] + v1[Z] * v2[Z];
}

/*****************************************************************
 * Read initial conditions file and set mesh accordingly
 * Input: file = filename of initial conditions file
 * Output: e = the initialized mesh
 * Returns: nonzero on file error
 */
int read_init( char *file, elem e[] )
{
   FILE *fp;  /* Pointer to a file */
   int i;     /* ID number of element used for indexing */
   double pp, tt, rr; /* Initial: pressure, temp, density */

   /* Read initial conditions file */
   if( ( fp = fopen( file, "r" ) ) == NULL )
   {
      /* File error while opening */
      return( 1 );
   }

   /* Read in one line of data from file */
   fscanf( fp, "%lf %lf %lf\n", &pp, &tt, &rr );

   /* Close file when done */
   fclose( fp );

   /* Set mesh elements to initial conditions */
   for( i = 0; i < MAX_E; i ++ )
   {
      e[i].p = pp - Pref;
      e[i].t = tt;
      e[i].rho = rr;
      e[i].dt = 0.01;
   }

   return( 0 );
}

/*****************************************************************
 * Read Boundary conditions file
 * Input: file = filename of boundary conditions file
 * Output: writes to global variables
 * Returns: nonzero on file error
 */
int read_bndy( char *file )
{
   FILE *fp;  /* Pointer to a File */

   /* Read boundary conditions file */
   if( ( fp = fopen( file, "r" ) ) == NULL )
   {
      /* File error while opening */
      return( 1 );
   }

   /* Read in one line of data from file */
   fscanf( fp, "%lf %lf %lf %lf %lf %lf\n", &Fx[X], &Fx[Y], &Fx[Z], &rhox,
	       &tx, &px );

   /* Close file when done */
   fclose( fp );

   /* Set gauge pressure constant Pref to 100000 */
   Pref = px;

   return( 0 );
}

/*****************************************************************
 * Calculate the invicid fluxes on a mesh
 * Input: e  = mesh of elements
 *        c1 = connection matrix of mesh
 *        i1 = element to process, or < 0 means process entire mesh
 * Output: e = fluxes in elements
 * Returns: none, void
 */
void calc_f( elem e[], conn c1[MAX_E], int i1 )
{
   int i, j;    /* Element and Face indexes, respectively */
   double FdA;  /* Vel-dot-area */
   double ubar, vbar, pbar, rhobar, hbar; /* Phi-bar values */
   double fu, fv, fp;   /* Flux values in u, v, and p */

   /* Calculate fluxes for each element */
   for( i = 0; i < MAX_E; i ++ )
   {
      if( i1 > -1 )
	 i = i1;

      /* If element is not present, don't process it */
      if( !e[i].pres )
	 continue;

      /* Set fluxes on this node initially to zero */
      e[i].fv[X] = 0., e[i].fv[Y] = 0., e[i].fv[Z] = 0.;
      e[i].flp = 0.;

      /* Calculate fluxes on each face */
      for( j = 0; j < e[i].nodes; j ++ )
      {
	 /* Calculate phi-bar values */
	 b_cond( e, c1, &ubar, &vbar, &pbar, &rhobar, &hbar, i, j );

	 /* Calculate F (dot) Area on this face */
	 FdA = ubar * e[i].A[j][X] + vbar * e[i].A[j][Y];
	 if( rhobar > 10 )
	 {
	    FdA = FdA;
	 }

	 /* Calculate the fluxes */
	 fp = rhobar * FdA;
	 fu = rhobar * ubar * FdA + pbar * e[i].A[j][X];
	 fv = rhobar * vbar * FdA + pbar * e[i].A[j][Y];

	 /* Sum fluxes */
	 e[i].flp += fp;
	 e[i].fv[X] += fu;
	 e[i].fv[Y] += fv;
      }

      if( i1 > -1 )
	 break;
   }
}

/*****************************************************************
 * Calculate phi-bar for an element
 */
static void b_cond( elem e[],             /* I: Mesh of elements */
		    conn c1[MAX_E],       /* I: Connection matrix */
		    double *ubar,         /* O: Average x-velocity */
		    double *vbar,         /* O: Average y-velocity */
		    double *pbar,         /* O: Average pressure */
		    double *rhobar,       /* O: Average density */
		    double *hbar,         /* O: Average Energy */
		    int i,                /* I: Element number */
		    int j                 /* I: face number */
		    )
{
   int ind;  /* Temporary index of adjacent element */
   char flg = 0; /* Flag set when one of the conditions is met */
   vect uv; /* Temporary Vector used for projection onto wall */
   double te;  /* Temporary real number */

   /* Get index of element adjacent to current element (i) at current
    * face (j) */
   ind = c1[i][j];

   if( ind > 0 )
   {
      /* No boundary conditions */
      ind --;
      *ubar = ( e[i].F[X] + e[ind].F[X] ) / 2.;
      *vbar = ( e[i].F[Y] + e[ind].F[Y] ) / 2.;
      *pbar = ( e[i].p + e[ind].p ) / 2. + Pref;
      *rhobar = ( e[i].rho + e[ind].rho ) / 2.;
      flg = 1;
   }
   if( ind == INFLOW )
   {
      /* Boundary condition of inflow inlet */
      *ubar = Fx[X];
      *vbar = Fx[Y];
      *rhobar = rhox;

      /* Conditions are different for supersonic */
      if( sqrt( Fx[X] * Fx[X] + Fx[Y] * Fx[Y] ) <=
	 ( 20.05 * sqrt( e[i].t ) ) )
      {
	 *pbar = e[i].p + Pref;
      }
      else
      {
	 *pbar = px;
      }

      flg = 1;
   }
   if( ind == EXIT )
   {
      /* Boundary condition of outflow outlet */
      *ubar = e[i].F[X];
      *vbar = e[i].F[Y];
      *rhobar = e[i].rho;

      /* Conditions are different for supersonic */
      if( sqrt( Fx[X] * Fx[X] + Fx[Y] * Fx[Y] ) <=
	 ( 20.05 * sqrt( e[i].t ) ) )
      {
	 *pbar = px;
      }
      else
      {
	 *pbar = e[i].p + Pref;
      }

      flg = 1;
   }
   if( ( ( ind == WALL1 || ind == WALL2 ) || ( !e[ind].pres ) ) && !flg )
   {
      /* Boundary condition is a wall */
      *pbar = e[i].p + Pref;
      *rhobar = e[i].rho;

      /* Project velocity vector onto wall */
      te = sqrt( e[i].A[j][X] * e[i].A[j][X] +
	    e[i].A[j][Y] * e[i].A[j][Y] );
      v_copy( e[i].A[j], uv );
      uv[X] /= te, uv[Y] /= te;
      v_dot( e[i].A[j], e[i].F, &te );
      te /= sqrt( e[i].A[j][X] * e[i].A[j][X] +
	    e[i].A[j][Y] * e[i].A[j][Y] );
      uv[X] *= te, uv[Y] *= te;
      *ubar = e[i].F[X] - uv[X], *vbar = e[i].F[Y] - uv[Y];
   }
}

/* Calculate flux dissipation terms
 * Input: e  = mesh of elements
 *        c1 = connection matrix of mesh elements
 *        i  = current element ID
 * Output: dv = flux dissipation vector
 *         dp = pressure flux dissipation
 * Returns: none, void.
 */
void calc_d( elem e[], conn c1[MAX_E], vect dv, double *dp, int i )
{
   int j;   /* face index */
   double eps2, tl1, tl2;  /* Dissipation factor, time steps */
   double dphi; /* dissipation term (temporary) */
   double vp;  /* First part of dissipation term */
   int ind;   /* Temporary index of adjacent element */

   /* Setup variables */
   tl1 = e[i].dt;
   eps2 = 0.05;

   /* If element is not present, don't process it */
   if( !e[i].pres )
      return;

   /* Set dissipations to 0.0 */
   dv[X] = 0., dv[Y] = 0., dv[Z] = 0.;
   *dp = 0.;

   /* Calculate dissipations on each face */
   for( j = 0; j < e[i].nodes; j ++ )
   {
      /* Get index of element adjacent to current element (i) at current
       * face (j) */
      ind = c1[i][j];

      /* Don't dissipate boundary conditions */
      if( ind <= 0 )
	 continue;

      ind --;
      /* Second element's time step is tl2 */
      tl2 = e[ind].dt;

      /* Calculate the first part of dissipation term (constant for
       * all phi on this face */
      vp = ( e[i].V / ( tl1 * e[i].nodes ) ) + ( e[ind].V / ( tl2 *
	   e[ind].nodes ) );
      vp *= eps2;

      /* Calculate phi-dependent part of dissipation term */
      /* Using u for phi */
      dphi = vp * ( e[i].F[X] - e[ind].F[X] );
      dv[X] -= dphi;

      /* Using v for phi */
      dphi = vp * ( e[i].F[Y] - e[ind].F[Y] );
      dv[Y] -= dphi;

      /* Using rho for phi */
      dphi = vp * ( e[i].rho - e[ind].rho );
      *dp -= dphi;
   }
}

/***************************************************************
 * Integrate the fluxes in each element
 * Input: e  = mesh of elements
 *        c1 = connection matrix of mesh
 * Output: e = phi values are changed
 * Returns: none, void.
 */
void integ_f( elem e[], conn c1[MAX_E] )
{
   int i;    /* Index of element */
   double u0, v0, w0, rh0, h0;  /* Original phis used in Runge-Kutta */
   double u1, v1, w1, rh1, h1;  /* New phis used in Runge-Kutta */
   double fpd; /* Pressure flux dissipation */
   vect fd;   /* Flux dissipation vector */

   /* Loop through all elements */
   for( i = 0; i < MAX_E; i ++ )
   {
      /* Do not process non-existant elements */
      if( !e[i].pres )
	 continue;

      /* Recalculate time step for this element */
      calc_t( e, i );

      /* Calculate flux dissipation (used throughout solver) */
      calc_d( e, c1, fd, &fpd, i );

      /* Runge-Kutta solver */
      /* Get original phis */
      u0 = e[i].F[X], v0 = e[i].F[Y], w0 = e[i].F[Z], rh0 = e[i].rho;

      /* First stage. . . */
      calc_f( e, c1, i );
      u1 = u0 - ( e[i].dt / ( 4 * e[i].V ) ) * ( e[i].fv[X] - fd[X] );
      v1 = v0 - ( e[i].dt / ( 4 * e[i].V ) ) * ( e[i].fv[Y] - fd[Y] );
      w1 = w0 - ( e[i].dt / ( 4 * e[i].V ) ) * ( e[i].fv[Z] - fd[Z] );
      rh1 = rh0 - ( e[i].dt / ( 4 * e[i].V ) ) * ( e[i].flp - fpd );
      e[i].F[X] = u1, e[i].F[Y] = v1, e[i].F[Z] = w1, e[i].rho = rh1;

      /* Calculate pressure from ideal gas law */
      e[i].p = ( e[i].rho * 8314.4 * e[i].t / 29.9 ) - Pref;

      /* Second stage. . . */
      calc_f( e, c1, i );
      u1 = u0 - ( e[i].dt / ( 3 * e[i].V ) ) * ( e[i].fv[X] - fd[X] );
      v1 = v0 - ( e[i].dt / ( 3 * e[i].V ) ) * ( e[i].fv[Y] - fd[Y] );
      w1 = w0 - ( e[i].dt / ( 3 * e[i].V ) ) * ( e[i].fv[Z] - fd[Z] );
      rh1 = rh0 - ( e[i].dt / ( 3 * e[i].V ) ) * ( e[i].flp - fpd );
      e[i].F[X] = u1, e[i].F[Y] = v1, e[i].F[Z] = w1, e[i].rho = rh1;

      /* Calculate pressure from ideal gas law */
      e[i].p = ( e[i].rho * 8314.4 * e[i].t / 29.9 ) - Pref;

      /* Third stage. . . */
      calc_f( e, c1, i );
      u1 = u0 - ( e[i].dt / ( 2 * e[i].V ) ) * ( e[i].fv[X] - fd[X] );
      v1 = v0 - ( e[i].dt / ( 2 * e[i].V ) ) * ( e[i].fv[Y] - fd[Y] );
      w1 = w0 - ( e[i].dt / ( 2 * e[i].V ) ) * ( e[i].fv[Z] - fd[Z] );
      rh1 = rh0 - ( e[i].dt / ( 2 * e[i].V ) ) * ( e[i].flp - fpd );
      e[i].F[X] = u1, e[i].F[Y] = v1, e[i].F[Z] = w1, e[i].rho = rh1;

      /* Calculate pressure from ideal gas law */
      e[i].p = ( e[i].rho * 8314.4 * e[i].t / 29.9 ) - Pref;

      /* Fourth stage. . . */
      calc_f( e, c1, i );
      u1 = u0 - ( e[i].dt / e[i].V ) * ( e[i].fv[X] - fd[X] );
      v1 = v0 - ( e[i].dt / e[i].V ) * ( e[i].fv[Y] - fd[Y] );
      w1 = w0 - ( e[i].dt / e[i].V ) * ( e[i].fv[Z] - fd[Z] );
      rh1 = rh0 - ( e[i].dt / e[i].V ) * ( e[i].flp - fpd );
      e[i].F[X] = u1, e[i].F[Y] = v1, e[i].F[Z] = w1, e[i].rho = rh1;

      /* Calculate pressure from ideal gas law */
      e[i].p = ( e[i].rho * 8314.4 * e[i].t / 29.9 ) - Pref;

      /*printf("%d, rho=%g, flp=%g, p=%g, fF=<%g,%g>\n",i,e[i].rho,e[i].flp,
	      e[i].p,e[i].fv[X],e[i].fv[Y]);*/
   }
}

/***********************************************************
 * Save the data of a mesh to and output file for plotting
 * Input: e    = mesh of elements
 *        file = file to write to (will overwrite when necessary)
 * Output: none.
 * Returns: 0 if sucess, nonzero on file fail
 */
int save_e( elem e[], char *file )
{
   FILE *fp;  /* File pointer */
   int i;   /* Element ID */

   /* Open file for output */
   if( ( fp = fopen( file, "w" ) ) == NULL )
   {
      return( 1 );
   }

   for( i = 0; i < MAX_E; i ++ )
   {
      /* Don't write an element that doesn't exist */
      if( !e[i].pres )
	 continue;

      fprintf( fp, "%d,%g,%g,%g,%g,%g,%g\n",i,e[i].F[X],e[i].F[Y],e[i].F[Z],
	       e[i].p, e[i].t, e[i].rho );
   }

   /* Close file when done, return */
   fclose( fp );
   return( 0 );
}

/***********************************************************
 * Calculate the time step for element
 * Input: e = mesh of elements
 *        i = element ID to recalculate time step
 * Output: time step in element
 * Returns: none, void.
 */
void calc_t( elem e[], int i )
{
   int j; /* Face index */
   double sum, CFL, lambda; /* Sum of lambdas, CFL number, spectral radius */
   double c;   /* Speed of sound */

   /* Set speed of sound */
   c = 20.05 * sqrt( e[i].t );

   /* Set CFL number */
   CFL = 1;

   /* Sum the lambdas */
   sum = 0.;
   for( j = 0; j < e[i].nodes; j ++ )
   {
      /* Lambda = |u dot A| + c|A| */
      v_dot( e[i].F, e[i].A[j], &lambda );
      lambda = fabs( lambda );
      lambda += c *
	  sqrt( e[i].A[j][X] * e[i].A[j][X] + e[i].A[j][Y] * e[i].A[j][Y] );

      sum += lambda / CFL;
   }

   /* Calculate time step */
   e[i].dt = e[i].V / sum;
   /*e[i].dt /= 5;*/
}
