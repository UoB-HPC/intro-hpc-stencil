#!/usr/bin/env python2
from __future__ import print_function
import sys
import argparse

# Intermediate class to parse arguments
class InputParser(argparse.ArgumentParser):
    def __init__(self):
        super(InputParser, self).__init__(
            description="Testing script for HPC Stencil coursework",
            fromfile_prefix_chars='@',
            formatter_class=argparse.ArgumentDefaultsHelpFormatter,
            )

        # verbose output
        self.add_argument("--verbose",
          action='store_true'
        )

        # % tolerance
        self.add_argument("--tolerance",
            default=1,
            type=int,
            help="""Absolute tolerance to match against reference results""")

        # Reference results
        self.add_argument("--ref-stencil-file",
            required=True,
            help="""reference stencil.pgm results file""")

        # Calculated results
        self.add_argument("--stencil-file",
            required=True,
            help="""calculated stencil.pgm results file""")

parser = InputParser()
parsed_args = parser.parse_args()

# Open reference and input files
stencil_ref = open(parsed_args.ref_stencil_file, "rb")
stencil_res = open(parsed_args.stencil_file, "rb")

# Check image header
ref_format, ref_nx, ref_ny, ref_depth = stencil_ref.readline().decode('ascii').split()
format, nx, ny, depth = stencil_res.readline().decode('ascii').split()
if not (format == ref_format == "P5"): 
    print("Error: incorrect file format")
    sys.exit()
if not (depth == ref_depth == "255"):
    print("Error: incorrect depth value")
    sys.exit()
if not (nx == ref_nx and ny == ref_ny):
    print("Error: image sizes do not match")
    sys.exit()

# Compare images
passed = True
for j in range(int(ref_ny)):
    for i in range(int(ref_nx)):
        ref_val = ord(stencil_ref.read(1))
        val = ord(stencil_res.read(1))
        if abs(ref_val - val) > parsed_args.tolerance:
            passed = False
            if (parsed_args.verbose):
                print("Values differ at ("+str(i)+", "+str(j)+"): ref="+str(ref_val)+", res="+str(val))

# Print summary
print(80*"-")
if passed:
    print("Comparison passed: images match with tolerance +/- {}".format(parsed_args.tolerance))
else:
    print("Comparison failed")
print(80*"-")
