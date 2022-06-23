# cfd1: A simple CFD solver

Copyright (c) 2022, Michael Robinson

## Synopsis

This is the source code for a simple fluid dynamics solver I wrote many years ago (circa 1995).  It may not entirely work and was never fully tested.  It was intended to be built using the Microsoft Quick C compiler on x86.

The main Quick C dependency is to the graphics library in `<graph.h>`.  It also uses a nonstandard `<conio.h>` and `<stdlib.h>` functions.  These are hacked around using a `#define`.

## Libraries

There are three related library C files:

* `flow1.c` : Solver for a structured rectangular grid
* `cfd1.c` : Solver for an unstructured polyhedral mesh, including file readers for the mesh geometry and initial conditions
* `meshpt.c` : A library for drawing unstructured polyhedral meshes on the screen using Microsoft Quick C graphics commands.

## Top-level programs

There are three programs, which might be best considered only unit tests.  They are what was intended to be run from the command line.  Each has an associated Makefile, with extension `.mak`.

* `flowt1.c` : Unit test for the structured rectangular grid solver
* `flow2t.c` : Unit test for the unstructured polyhedral mesh solver
* `flow3t.c` : Mesh file plotter for unstructured grids

I suspect that I usually used `flow2t` and `flow3t` for nearly all of my development, since I generally prefer unstructured meshes.  However, I don't really have any evidence that this was the case.

## Building

Of the programs, `flow2t` is the only one that successfully builds with modern `gcc`:

```
gcc flow2t.c cfd1.c -o flow2t -lm
```

The `flow1.c` and `cfd1.c` libraries build without problems in `gcc`.  The `meshpt.c` library requires `<graph.h>` and `<conio.h>`, so it will not build without modifications.

## Example data files

The `cfd1.c` library reads three kinds of data files that specify the volume and initial flow conditions.

* Mesh geometry files (three example files `mike?.dat` are included)
* Boundary conditions (example file `bnd1.dat` is included)
* Initial conditions (example file `init1.dat` is included)
