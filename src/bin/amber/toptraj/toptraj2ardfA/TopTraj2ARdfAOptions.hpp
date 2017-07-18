#ifndef TopTraj2ARdfAOptionsH
#define TopTraj2ARdfAOptionsH
// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2008 Petr Kulhanek, kulhanek@enzim.hu
//    Copyright (C) 2005 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2004 Petr Kulhanek, kulhanek@chemi.muni.cz
//
//     This program is free software; you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation; either version 2 of the License, or
//     (at your option) any later version.
//
//     This program is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
//
//     You should have received a copy of the GNU General Public License along
//     with this program; if not, write to the Free Software Foundation, Inc.,
//     51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// =============================================================================

#include <SimpleOptions.hpp>
#include <CATsMainHeader.hpp>
#include <math.h>

//------------------------------------------------------------------------------

class CTopTraj2ARdfAOptions : public CSimpleOptions {
public:
    // constructor - tune option setup
    CTopTraj2ARdfAOptions(void);

// program name and description -----------------------------------------------
    CSO_PROG_NAME_BEGIN
    "toptra2ardfA"
    CSO_PROG_NAME_END

    CSO_PROG_DESC_BEGIN
    "<red>toptra2ardfA</red> calculates angular radial distribution function g(<u>r</u>,<u>a</u>). A distance <u>r</u> is calculated "
    "between <u>SOLUTE</u> and <u>SOLVENT</u> residue centers of masses. An angle <u>a</u> is calculated between "
    "a vector between <u>SOLUTE</u> and <u>SOLVENT</u> residue centers of masses and a normal vector of best fit plane of <u>SOLVENT</u> residue."
    CSO_PROG_DESC_END

    CSO_PROG_VERS_BEGIN
    LibBuildVersion_CATs
    CSO_PROG_VERS_END

// list of all options and arguments ------------------------------------------
    CSO_LIST_BEGIN
    // arguments ----------------------------
    CSO_ARG(CSmallString,TopName)
    CSO_ARG(CSmallString,TrajName)
    CSO_ARG(CSmallString,OutputName)
    CSO_ARG(CSmallString,SoluteMask)
    CSO_ARG(CSmallString,SolventMask)
    // options ------------------------------
    CSO_OPT(double,StartValue1)
    CSO_OPT(double,StopValue1)
    CSO_OPT(int,NBins1)
    CSO_OPT(double,StartValue2)
    CSO_OPT(double,StopValue2)
    CSO_OPT(int,NBins2)
    CSO_OPT(double,Density)
    CSO_OPT(bool,Histogram)
    CSO_OPT(int,Frequency)
    CSO_OPT(bool,Help)
    CSO_OPT(bool,Version)
    CSO_OPT(bool,Verbose)
    CSO_LIST_END

    CSO_MAP_BEGIN
    // description of options -----------------------------------------------------
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                TopName,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "TOP",                           /* parametr name */
                "topology file name")   /* argument description */
    //----------------------------------------------------------------------
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                TrajName,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "TRAJ",                           /* parametr name */
                "trajectory file name")   /* argument description */
    //----------------------------------------------------------------------
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                OutputName,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "OUT",                           /* parametr name */
                "output name")   /* argument description */
    //----------------------------------------------------------------------
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                SoluteMask,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "SOLUTE",                           /* parametr name */
                "solute mask or 'none' keyword")   /* argument description */
    //----------------------------------------------------------------------
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                SolventMask,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "SOLVENT",                           /* parametr name */
                "solvent mask")   /* argument description */
// description of options -----------------------------------------------------
    CSO_MAP_OPT(double,                           /* option type */
                StartValue1,                        /* option name */
                0,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "start1",                      /* long option name */
                "REAL",                           /* parametr name */
                "REAL is the left value of the histogram interval (distance domain)")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(double,                           /* option type */
                StopValue1,                        /* option name */
                15.0,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "stop1",                      /* long option name */
                "REAL",                           /* parametr name */
                "REAL is the right value of the histogram interval (distance domain)")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(int,                           /* option type */
                NBins1,                        /* option name */
                60,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "nbins1",                      /* long option name */
                "NUM",                           /* parametr name */
                "NUM is the number of histogram bins (distance domain)")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(double,                           /* option type */
                StartValue2,                        /* option name */
                0,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "start2",                      /* long option name */
                "REAL",                           /* parametr name */
                "REAL is the left value of the histogram interval (angle domain)")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(double,                           /* option type */
                StopValue2,                        /* option name */
                M_PI/2.0,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "stop2",                      /* long option name */
                "REAL",                           /* parametr name */
                "REAL is the right value of the histogram interval (angle domain)")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(int,                           /* option type */
                NBins2,                        /* option name */
                45,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "nbins2",                      /* long option name */
                "NUM",                           /* parametr name */
                "NUM is the number of histogram bins (angle domain)")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(double,                           /* option type */
                Density,                        /* option name */
                1.0,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "density",                      /* long option name */
                "REAL",                           /* parametr name */
                "number density [number of solute(solvent) molecules per unit volume (A^3)]")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                Histogram,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "histogram",                      /* long option name */
                NULL,                           /* parametr name */
                "print only histogram not RDF")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(int,                           /* option type */
                Frequency,                        /* option name */
                1,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "frequency",                      /* long option name */
                "INT",                           /* parametr name */
                "INT is the frequency of snapshots to be analyzed")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                Verbose,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                'v',                           /* short option name */
                "verbose",                      /* long option name */
                NULL,                           /* parametr name */
                "increase output verbosity")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                Version,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "version",                      /* long option name */
                NULL,                           /* parametr name */
                "output version information and exit")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                Help,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                'h',                           /* short option name */
                "help",                      /* long option name */
                NULL,                           /* parametr name */
                "display this help and exit")   /* option description */
    CSO_MAP_END

// final operation with options ------------------------------------------------
private:
    virtual int CheckOptions(void);
    virtual int FinalizeOptions(void);
    virtual int CheckArguments(void);
};

//------------------------------------------------------------------------------

#endif
