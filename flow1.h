/**************************************
 * CFD Header File
 **************************************/

/*----------------------------------+
 |            Constants             |
 +----------------------------------*/
#define GRID_X 10     /* Number of horizontal grid elements */
#define GRID_Y 10     /* Number of vertical grid elements */
#define X 0           /* Index of x component in vector */
#define Y 1           /* Index of y component in vector */

/*----------------------------------+
 |         Defined Types            |
 +----------------------------------*/

/* A vector */
typedef double vector[2];

/* A square control volume */
typedef struct
{
   int type;            /* Type of volume (0=not active, 1=in, 2=out) */
   double rho[4];       /* Density */
   double u[4], v[4];   /* x and y components of velocity vector */
   double p[4];         /* Pressure */
   vector A;            /* Volume vector */
   double fp;           /* Pressure flux */
   double fu, fv;       /* velocity vector flux */
}cvol_t;

/* A structured grid of control volumes */
typedef cvol_t sgrid[GRID_Y * GRID_X];

/*----------------------------------+
 |      Functions and Routines      |
 +----------------------------------*/

/* Set the fluxes in a control volume <v> to 0 */
void init_f( cvol_t *v );

/* Calculate fluxes in the grid <g> for one iteration */
void flux_m( sgrid g );

/* Calculate and distribute fluxes in the element at coord (<x>,<y>) in
 * grid <g> */
void flux_v( sgrid g, int x, int y );

/* Distribute fluxes in grid <g> from element at coord (<x>,<y>) on
 * edge <i> */
void d_flux( sgrid g, int x, int y, int i, double uf, double vf, double pf );

/* Get coords of an element adjacent to the one at (<x>,<y>), at side <i>,
 * Store coords in (<x2>,<y2>), returns nonzero if no such element exists
 */
int adje( int i, int x, int y, int *x2, int *y2 );

/* Zero an entire grid <g> */
void z_grid( sgrid g );

/* Transfer flux to current values in grid */
void flux_use( sgrid g );
