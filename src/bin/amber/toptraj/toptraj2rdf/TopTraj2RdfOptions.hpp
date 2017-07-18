#ifndef TopTraj2RdfOptionsH
#define TopTraj2RdfOptionsH
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

class CTopTraj2RdfOptions : public CSimpleOptions {
public:
    // constructor - tune option setup
    CTopTraj2RdfOptions(void);

// program name and description -----------------------------------------------
    CSO_PROG_NAME_BEGIN
    "toptraj2rdf"
    CSO_PROG_NAME_END

    CSO_PROG_DESC_BEGIN
    "<red>toptraj2rdf</red> calculates a radial distribution function (RDF). The RDF describes how the atomic <u>SOLVENT</u> density "
    "varies as a function of the distance from one particular <u>SOLUTE</u> atom. If more than one <u>SOLUTE</u> atom is provided then the calculated "
    "RDF is avarged over all <u>SOLUTE</u> atoms. The program offers two distinct "
    "functionalities: a) atom based RDF and b) center of mass based RDF.\n\n"
    "<u><red>Atom based RDF</red></u>\nThis is the default functionality of the program. A distance is calculated between solute "
    "and solvent atoms. Solute solvent atom pairs belonging to the same residue can be excluded by <b><blue>excludewithinresidues</blue></b> option.\n\n"
    "<u><red>Center of mass based RDF</red></u>\nThis functionality is enabled by <b><blue>residues</blue></b> option. A distance is calculated "
    "between center of masses of solute and solvent residues."
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
    CSO_OPT(double,StartValue)
    CSO_OPT(double,StopValue)
    CSO_OPT(int,NBins)
    CSO_OPT(double,Density)
    CSO_OPT(bool,Histogram)
    CSO_OPT(int,Frequency)
    CSO_OPT(bool,Residues)
    CSO_OPT(bool,ExcludeWithinResidues)
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
                "solute mask")   /* argument description */
    //----------------------------------------------------------------------
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                SolventMask,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "SOLVENT",                           /* parametr name */
                "solvent mask")   /* argument description */
// description of options -----------------------------------------------------
    CSO_MAP_OPT(double,                           /* option type */
                StartValue,                        /* option name */
                0,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "start",                      /* long option name */
                "REAL",                           /* parametr name */
                "REAL is the left value of the histogram interval (distance domain)")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(double,                           /* option type */
                StopValue,                        /* option name */
                15.0,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "stop",                      /* long option name */
                "REAL",                           /* parametr name */
                "REAL is the right value of the histogram interval (distance domain)")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(int,                           /* option type */
                NBins,                        /* option name */
                60,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "nbins",                      /* long option name */
                "NUM",                           /* parametr name */
                "NUM is the number of histogram bins (distance domain)")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(double,                           /* option type */
                Density,                        /* option name */
                1.0,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "density",                      /* long option name */
                "REAL",                           /* parametr name */
                "solvent number density [number of solvent atoms/residues per unit volume (A^3)]")   /* option description */
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
                Residues,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "residues",                      /* long option name */
                NULL,                           /* parametr name */
                "RDF among residues COMs")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                ExcludeWithinResidues,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "excludewithinresidues",                      /* long option name */
                NULL,                           /* parametr name */
                "exclude atom pairs within residues")   /* option description */
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
