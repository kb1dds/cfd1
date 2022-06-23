/***************************
 * CFD Mesh plotter Header *
 ***************************/

/*--------------------+
 | Manifest Constants |
 +--------------------*/

/* Maximums */
#define MAX_E 307 /* Max. mesh elements */
#define MAX_F 5 /* Max faces in an element */

/* Indexes of point and vector types */
#define X 0
#define Y 1
#define Z 2

double minv;  /* Minimum value of displayed data */
double maxv;  /* Maximum value of displayed data */

/*-----------+
 | New Types |
 +-----------*/

/* A three-dimensional point */
typedef double pt[3];

/* A closed element shape */
typedef struct
{
   char pres;   /* Present: 0 = Not present, 1 = present */
   int nodes;   /* Number of node points */
   pt p[MAX_F]; /* Each of the node points */
}poly;

/*------------------+
 | Global Variables |
 +------------------*/
double max_x; /* Maximum X value on screen */
double min_x; /* Minimum X value on screen */
double max_y; /* Maximum Y value on screen */
double min_y; /* Minimum Y value on screen */

/*---------------------+
 | Function Prototypes |
 +---------------------*/

/***************************************************************
 * Read in and plot mesh model file:
 * Input: file = mesh model file to read
 *        fil2 = data file to read
 *        ple  = Parameter in data file to use
 * Output: none
 * Returns: number of elements read, <= 0 if error
 */
int msh_mtx( char *file, char *fil2, int ple );

/****************************************************************
 * Draw an element on the screen
 * Input: p = element to draw
 * Output: none
 * Returns: none, void.
 */
void draw_e( poly p );

/***************************************************************
 * Scale screen to fit file
 * Input: file = mesh model file to read
 * Output: none
 * Returns: number of elements read, <= 0 if error
 */
int scal_mtx( char *file );

/****************************************************************
 * Draw an element on the screen as a windvane
 * Input: p   = element to draw
 *        u,v = vector of velocity
 * Output: none
 * Returns: none, void.
 */
void draw_w( poly p, double u, double v );

