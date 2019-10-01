# 5-point stencil

This code implements a weighted 5-point stencil on a rectangular grid.  The
value in each cell of the grid is updated based on an average of the values in
the neighbouring North, South, East and West cells.

The grid is initialised into a checkerboard pattern, with each square of the
checkerboard being 64x64 pixels. The stencil operation reads from one grid and
writes to a temporary grid.  The stencil is run twice for every iteration, with
the final result being held in the original array.  The results are quantised to
integers in the range [0,255] and output as a binary image file that may be
viewed graphically.

The only output of each run is the runtime of the iteration loop of the program.
Initialisation and output are not timed.

## Building and running the code

A simple `Makefile` is provided to build the code using the GCC compiler.  Just
type `make` to build the code.  A binary named `stencil` is produced on a
successful build.

A job script for BlueCrystal Phase 4 is also provided in `stencil.job`.  This
will request time on 1 node of the `teaching` queue and execute the `stencil`
binary on one of the input problems.  Submit this to the queue with the
following command:

    sbatch stencil.job

There are *three* input problems tested, representing different grid sizes.  The
inputs are all set as command line arguments to the executable in the following
format:

    ./stencil nx ny niters

The inputs required are:

| nx   | ny   | niters | Command                   | Serial Time on BCp4 |
| ---- | ---- | ------ | ------------------------- | ------------------- |
| 1024 | 1024 | 100    | `./stencil 1024 1024 100` |           5.908341s |
| 4096 | 4096 | 100    | `./stencil 4096 4096 100` |         130.196475s |
| 8000 | 8000 | 100    | `./stencil 8000 8000 100` |         561.118133s |

## Checking results

The program will have executed correctly if the output image matches the
provided reference output images with a small tolerance of +/- 1.  A Python
check script is provided to check the values. 

    python check.py --ref-stencil-file stencil_1024_1024_100.pgm --stencil-file stencil.pgm

If any errors are found, the script can be rerun with the addition of the
`--verbose` flag to provide information about where the errors occur in the
grid.

The reference input files for the different problems are named:

| nx   | ny   | niters | Reference file              |
| ---- | ---- | ------ | --------------------------- |
| 1024 | 1024 | 100    | `stencil_1024_1024_100.pgm` |
| 4096 | 4096 | 100    | `stencil_4096_4096_100.pgm` |
| 8000 | 8000 | 100    | `stencil_8000_8000_100.pgm` |

## MPI Advice

For the MPI assignment you'll want to start running stencil over multiple cores
and over multiple nodes. To do this on BCp4 you need to change the parameters
and the top of the `stencil.job` job file.

    --nodes: controls the number of nodes you want to run on
    --ntasks-per-node: controls the number of MPI processes per node

If these values are correctly set, then `srun` will pick up on them and run the
required number of processes for your job.

*FOR INTEL MPI YOU NEED THE FOLLOWING LINE IN THE JOB SUBMIT SCRIPT*

    export I_MPI_PMI_LIBRARY=/usr/lib64/libpmi.so

More info: 
<https://www.acrc.bris.ac.uk/protected/bc4-docs/scheduler/index.html#running-parallel>


