# CATs - Conversion And Analysis Tools
The package CATs contains several utilities for AMBER topology and coordinate manipulation. Operations can be performed on the whole structure or on a subset defined by a mask. The mask syntax is compatible with the AMBER mask syntax, and additionally it contains several extra selection operators. Parallel processing of AMBER trajectories is supported, together with a simple collection of the results scattered throughout the output files. The package also contains a simple scripting environment that can combine several individual tasks in a more complicated fashion, including cycles and conditional structures.

## Main Features
* processing data from simulations performed in the [AMBER](https://ambermd.org) package
* various CLI utilitis for manipulation with topology, coordinates, and trajectories
* parallel analysis of trajectories
* scripting in the cats interpreter based on JavaScript
* [PMFLib](https://github.com/kulhanek/pmflib) collective variables available in the cats interpreter

## Building Package
The procedure is decribed [here](https://github.com/kulhanek/cats-build).

## Documentation
The documentation is available [here](https://lcc.ncbr.muni.cz/whitezone/development/cats/).

