/* Invicid Fluxes */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef MSQC
#include <conio.h>
#include <process.h>
#else
#define itoa(x,y,n) sprintf(y,"%d",x)
#endif
#include "cfd1.h"

void main()
{
   elem *e;
   conn *c1;
   char file[30], k;
   char ftem[30], fact[40];
   int fnum;
   int max_el, i, i2, maxiter;

   Pref = 100000;

   /* Allocate space for mesh */
   e = (elem *) calloc( MAX_E, sizeof( elem ) );
   if( e == NULL )
   {
      /* Can't allocate enough space */
      perror( "flow2t" );
      exit( 2 );
   }
   c1 = (int *) calloc( MAX_E, sizeof( conn ) );
   if( c1 == NULL )
   {
      /* Can't allocate enough space */
      free( e );
      perror( "flow2t" );
      exit( 2 );
   }

   /* Retrieve all needed data */
   printf("Enter mesh file: ");
   scanf( "%s", file );
   max_el = read_msh( file, e, c1 );
   if( max_el <= 0 )
   {
      perror( "flow2t" );
      free( e );
      free( c1 );
      exit( 1 );
   }

   printf("Enter boundary conditions file: ");
   scanf( "%s", file );
   if( read_bndy( file ) )
   {
      perror( "flow2t" );
      free( e );
      free( c1 );
      exit( 1 );
   }

   printf("Enter initial conditions file: ");
   scanf( "%s", file );
   if( read_init( file, e ) )
   {
      perror( "flow2t" );
      free( e );
      free( c1 );
      exit( 1 );
   }
   printf("Enter output file template (\'.\' = no file): " );
   scanf( "%s", ftem );
   if( ftem[0] == '.' )
   {
      printf("Not saving to file.\n");
      fnum = -1;
   }
   else
   {
      printf("Saving to file.\n");
      fnum = 0;
   }

   printf("Enter max. number of iterations: ");
   scanf("%d",&maxiter);

   k = '\0';
   i2 = 0;
   while( i2 < maxiter )
   {
      printf("Time step number %d started.\n",i2);
      integ_f( e, c1 );

      /* If not saving to file, output data to screen */
      if( fnum == -1 )
      {
	 printf( "*****************************\n" );
	 for( i = 0; i < max_el; i ++ )
	 {
	    printf("Element ID: %d\n", i + 1 );
	    printf("Flux vector:\n (%g,%g,%g)\n",e[i].fv[X], e[i].fv[Y],
		   e[i].fv[Z] );
	    printf("Velocity Vector:\n (%g,%g,%g)\n",e[i].F[X], e[i].F[Y],
		   e[i].F[Z] );
	    printf("Pressure flux: %g\n",e[i].flp );
	    printf("Gauge Pressure: %g\n",e[i].p);
	    printf("Rho: %g\n",e[i].rho );
	 }
      }

      /* If saving to file, save every 20th time step */
      if( ( fnum > -1 ) && ( !( i2 % 20 ) ) )
      {
	 strcpy( fact, ftem );
	 itoa( fnum, file, 10 );
	 strcat( fact, file );
	 save_e( e, fact );
	 fnum ++;
      }

      printf("Time step number %d complete.\n",i2);
      i2 ++;

#ifdef MSQC
      /* A keypress stops the calculation prematurely */
      if( kbhit() )
      {
	 k = (char) getch();
	 if( k == 'q' )
	    break;
      }
#endif
   }

   free( e );
   free( c1 );
}
