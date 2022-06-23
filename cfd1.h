/******************************************
 * CFD HEADER FILE                        *
 ******************************************/

/*--------------------------------------+
 | #defines                             |
 +--------------------------------------*/
/* Indexes of point and vector types */
#define X 0
#define Y 1
#define Z 2

/* Maximum number of faces */
#define MAX_F 4

/* Maximum elements in a mesh */
#define MAX_E 310

/* Define boundary conditions */
#define INFLOW -1   /* Inflow boundary */
#define EXIT -2     /* Exit boundary */
#define WALL1 -3    /* Wall boundary */
#define WALL2 0     /* Another Wall boundary */

/*--------------------------------------+
 | New types                            |
 +--------------------------------------*/

/* Connection matrix element */
typedef int conn[MAX_F];

/* Point */
typedef double pt[3];

/* Vector */
typedef double vect[3];

/* Element */
typedef struct
{
   char pres;      /* Present: 0 = not present, 1 present */
   int nodes;      /* Number of nodes */

   vect A[MAX_F];  /* Area vector on each face */
   double V;       /* Volume of element */

   double rho;     /* Density */
   vect F;         /* Vector of flow (u,v,w) */
   double p;       /* Gauge Pressure */
   double t;       /* Temperature */
   vect fv;        /* Flux vector (flu,flv,flw) */
   double flp;     /* Pressure flux */
   double dt;      /* Local time step */
}elem;

/*--------------------------------------+
 | Global Variables                     |
 +--------------------------------------*/

/* Boundary conditions */
vect Fx;      /* Flux at inflow */
double tx;    /* Temperature at inflow */
double px;    /* Pressure at exit */
double rhox;  /* Density at inflow */

/* Constants */
double Pref;  /* 0 gauge pressure value */

/*--------------------------------------+
 | Functions and Routines               |
 +--------------------------------------*/

/* Read in a mesh model file:
 * Input: file = filename to read
 * Output: e  = array of elements, length less than or equal to MAX_E
 *         c1 = connectivity map
 * Returns: number of elements read, <= 0 if error
 */
int read_msh( char *file, elem e[], conn c1[MAX_E] );

/* Calculate the Area vectors:
 * Input: nodes = number of nodes
 *        xc    = point of each node
 * Output: A = array of area vectors
 * Returns: void, none
 */
void calc_A( vect A[MAX_F], int nodes, pt xc[MAX_F] );

/* Calculate the volume of an element
 * Input: nodes = number of nodes
 *        xc    = array of nodes
 * Output: V = volume of element
 * Returns: none, void
 */
void calc_V( double *V, int nodes, pt xc[MAX_F] );

/* Remap Raw face data to standard face data
 * Input: faces = number of faces
 *        A     = Raw Area vectors
 * Output: A = Standard area vectors
 * Returns: none, void
 */
void remap_f( int faces, vect A[MAX_F] );

/* Copy a vector
 * Input: vi = input vector
 * Output: vo = output vector
 * Returns: none,viod
 */
void v_copy( vect vi, vect vo );

/* Dot product of two vectors
 * Input: v1, v2 = two vectors to get dot product of
 * Output: dp = the dot product
 * Returns: none, void
 */
void v_dot( vect v1, vect v2, double *dp );

/* Read initial conditions file and set mesh accordingly
 * Input: file = filename of initial conditions file
 * Output: e = the initialized mesh
 * Returns: nonzero on file error
 */
int read_init( char *file, elem e[] );

/* Read Boundary conditions file
 * Input: file = filename of boundary conditions file
 * Returns: nonzero on file error
 */
int read_bndy( char *file );

/* Calculate the invicid fluxes on a mesh
 * Input: e  = mesh of elements
 *        c1 = connection matrix of mesh
 *        i1 = element to process, or < 0 means process entire mesh
 * Output: e = fluxes in elements
 * Returns: none, void
 */
void calc_f( elem e[], conn c1[MAX_E], int i1 );

/* Calculate flux dissipation terms
 * Input: e  = mesh of elements
 *        c1 = connection matrix of mesh elements
 *        i  = current element ID
 * Output: dv = flux dissipation vector
 *         dp = pressure flux dissipation
 * Returns: none, void.
 */
void calc_d( elem e[], conn c1[MAX_E], vect dv, double *dp, int i );

/***************************************************************
 * Integrate the fluxes in each element
 * Input: e  = mesh of elements
 *        c1 = connection matrix of mesh
 * Output: e = phi values are changed
 * Returns: none, void.
 */
void integ_f( elem e[], conn c1[MAX_E] );

/***********************************************************
 * Save the data of a mesh to and output file for plotting
 * Input: e    = mesh of elements
 *        file = file to write to (will overwrite when necessary)
 * Output: none.
 * Returns: 0 if sucess, nonzero on file fail
 */
int save_e( elem e[], char *file );

/***********************************************************
 * Calculate the time step for element
 * Input: e = mesh of elements
 *        i = element ID to recalculate time step
 * Output: time step in element
 * Returns: none, void.
 */
void calc_t( elem e[], int i );
