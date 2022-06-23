/**********************************************
 * CFD Routine module
 **********************************************/
#include "flow1.h"

/*----------------------------------+
 |  Defined Types (reference only)  |
 +----------------------------------+

 A vector
typedef double vector[2];

 A square control volume
typedef struct
{
   int type;       Type of volume (0=not active, 1=inflow, 2=outflow)
   double rho[4];  Density
   double u[4], v[4]; x and y components of velocity vector
   double p[4];       Pressure
   vector A;       Volume vector
   double fp;      Pressure flux
   double fu, fv;  velocity vector flux
}cvol_t;

 A structured grid of control volumes
typedef cvol_t sgrid[GRID_Y * GRID_X]; */

/* Set the fluxes in a control volume <v> to 0 */
void init_f( cvol_t *v )
{
   v->fp = 0.;
   v->fu = 0.;
   v->fv = 0.;
}

/* Calculate fluxes in the grid <g> for one iteration */
void flux_m( sgrid g )
{
   /* Local Variables */
   int i, i2, ind;

   /* Set all fluxes in the grid to 0 */
   for( i = 0; i < ( GRID_Y * GRID_X ); i ++ )
   {
      init_f( &g[i] );
   }

   /* Calculate and distribute fluxes for each element */
   for( i = 0; i < GRID_Y; i ++ )
   {
      for( i2 = 0; i2 < GRID_X; i2 ++ )
      {
	 ind = GRID_Y * i + i2;
	 /* Cannot calculate flux on nonexistant element */
	 if( g[ind].type == 0 )
	    continue;

	 /* Calculate fluxes in current element. . . */
	 flux_v( g, i2, i );
      }
   }
}

/* Calculate and distribute fluxes in the element at coord (<x>,<y>) in
 * grid <g> */
void flux_v( sgrid g, int x, int y )
{
   int i, ind;
   double uf, vf, pf, vdA;

   ind = y * GRID_Y + x;

   /* Calculate fluxes on each edge
    * Edges are arranged as the following chart:
    *               0
    *          +---------+
    *          |         |
    *        3 |         | 1
    *          |         |
    *          +---------+
    *               2
    */
   for( i = 0; i < 4; i ++ )
   {
      /* Calculate velocity dot area */
      vdA = g[ind].u[i] * g[ind].A[X] + g[ind].v[i] * g[ind].A[Y];

      /* Individual flux calculations */
      pf = g[ind].rho[i] * vdA;
      uf = g[ind].rho[i] * vdA * g[ind].u[i] + g[ind].p[i] * g[ind].A[X];
      vf = g[ind].rho[i] * vdA * g[ind].v[i] + g[ind].p[i] * g[ind].A[X];

      /* Distribute the flux */
      d_flux( g, x, y, i, uf, vf, pf );

      /* Change the flux inside this element, also */
      if( g[ind].type == 1 )
      {
	 g[ind].fu += uf;
	 g[ind].fv += vf;
	 g[ind].fp += pf;
      }
      if( g[ind].type == 2 )
      {
	 g[ind].fu -= uf;
	 g[ind].fv -= vf;
	 g[ind].fp -= pf;
      }
   }
}

/* Distribute fluxes in grid <g> from element at coord (<x>,<y>) on
 * edge <i> */
void d_flux( sgrid g, int x, int y, int i, double uf, double vf, double pf )
{
   int x2, y2, ty, err, ind;

   /* Store type of element in local variable for easier access */
   ind = y * GRID_Y + x;
   ty = g[ind].type;

   /* Determine which element to change fluxes */
   err = adje( i, x, y, &x2, &y2 );
   if( err )
      return;

   ind = y2 * GRID_Y + x2;

   /* Influx from other elements */
   if( ty == 1 )
   {
      g[ind].fu -= uf;
      g[ind].fv -= vf;
      g[ind].fp -= pf;
   }
   /* Outflux to other elements */
   if( ty == 2 )
   {
      g[ind].fu += uf;
      g[ind].fv += vf;
      g[ind].fp += pf;
   }
}

/* Get coords of an element adjacent to the one at (<x>,<y>), at side <i>,
 * Store coords in (<x2>,<y2>), returns nonzero if no such element exists
 */
int adje( int i, int x, int y, int *x2, int *y2 )
{
   int xn, yn, err;

   err = 0;

   /* Adjacent element is above the current one */
   if( i == 0 )
   {
      xn = x, yn = y - 1;

      if( yn < 0 )
	 err = 1;
   }
   /* Adjacent element is right of the current one */
   if( i == 1 )
   {
      xn = x + 1, yn = y;
      if( xn > GRID_X - 1 )
	 err = 1;
   }
   /* Adjacent element is below the current one */
   if( i == 2 )
   {
      xn = x, yn = y + 1;
      if( yn > GRID_Y - 1 )
	 err = 1;
   }
   /* Adjacent element is left of the current one */
   if( i == 3 )
   {
      xn = x - 1, yn = y;
      if( xn < 0 )
	 err = 1;
   }

   *x2 = xn, *y2 = yn;
   return( err );
}

/* Zero an entire grid <g> */
void z_grid( sgrid g )
{
   int i, e, tcnt, ind;

   tcnt = 1;
   for( i = 0; i < ( GRID_Y * GRID_X ); i ++ )
   {
      /* Zero the fluxes */
      init_f( &g[i] );

      /* Zero the edge-dependent variables */
      for( e = 0; e < 4; e ++ )
      {
	 g[i].rho[e] = 0.;
	 g[i].u[e] = 0.;
	 g[i].v[e] = 0.;
	 g[i].p[e] = 0.;
      }

      /* Zero the Area */
      g[i].A[X] = 1.;
      g[i].A[Y] = 1.;

      /* Set the type to be a checkerboard */
      if( ( ( i % GRID_Y ) == 0 ) && ( i != 0 ) )
      {
	 ind = i - GRID_Y;
	 tcnt = g[ind].type;
      }
      if( tcnt == 1 )
	 tcnt = 2;
      else
	 tcnt = 1;
      g[i].type = tcnt;
   }
}

/* Transfer flux to current values in grid */
void flux_use( sgrid g )
{
   int i, i2;

   for( i = 0; i < GRID_X * GRID_Y; i ++ )
   {
      /* Pressure flux */
      for( i2 = 0; i2 < 4; i2 ++ )
	 g[i].p[i2] += g[i].fp;

      /* Vector flux */
      g[i].u[1] += g[i].fu;
      g[i].u[3] -= g[i].fu;
      g[i].v[0] += g[i].fv;
      g[i].v[2] -= g[i].fv;
   }
}
