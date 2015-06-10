#ifndef TopSolSolOptionsH
#define TopSolSolOptionsH
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

//------------------------------------------------------------------------------

class CTopSolSolOptions : public CSimpleOptions {
public:
    // constructor - tune option setup
    CTopSolSolOptions(void);

// program name and description -----------------------------------------------
    CSO_PROG_NAME_BEGIN
    "topsolsol"
    CSO_PROG_NAME_END

    CSO_PROG_DESC_BEGIN
    "Analyse solute/solvent contacts according to Delaunay triangulation among the system atoms. The solvent residues are discrimated into directly bound, first, second, third, and fourth solvation layers. The tringulation is created as follows:\n\n"
    "topcrd2crd --output rbox topology.parm7 coords.rst7 coords.rbox\n"
    "cat coords.rbox | qdelaunay i TO output\n"
    CSO_PROG_DESC_END

    CSO_PROG_VERS_BEGIN
    CATS_VERSION
    CSO_PROG_VERS_END

// list of all options and arguments ------------------------------------------
    CSO_LIST_BEGIN
    // arguments ----------------------------
    CSO_ARG(CSmallString,TopologyName)
    CSO_ARG(CSmallString,DelaunayTriangulation)
    CSO_ARG(CSmallString,SoluteMask)
    CSO_ARG(CSmallString,SolventMask)
    CSO_ARG(CSmallString,OutputMasks)
    // options ------------------------------
    CSO_OPT(double,DirectlyBoundSolventRatio)
    CSO_OPT(bool,DoNotUseDirectAsSolvent)
    CSO_OPT(bool,Help)
    CSO_OPT(bool,Version)
    CSO_OPT(bool,Verbose)
    CSO_LIST_END

    CSO_MAP_BEGIN
// description of arguments ---------------------------------------------------
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                TopologyName,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "PARM",                           /* parametr name */
                "topology file name")   /* argument description */
    //----------------------------------------------------------------------
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                DelaunayTriangulation,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "TRIANGULATION",                           /* parametr name */
                "file name with Delaunay triangulation of the system created by qdelaunay command from QHull package")   /* argument description */
    //----------------------------------------------------------------------
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                SoluteMask,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "SOLUTE",                           /* parametr name */
                "solute definition in ASL mask format")   /* argument description */
    //----------------------------------------------------------------------
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                SolventMask,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "SOLVENT",                           /* parametr name */
                "solvent definition in ASL mask format")   /* argument description */
    //----------------------------------------------------------------------
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                OutputMasks,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "OUTPUT",                           /* parametr name */
                "file name with output masks defining solvent layers around solute")   /* argument description */
// description of options -----------------------------------------------------
    CSO_MAP_OPT(double,                           /* option type */
                DirectlyBoundSolventRatio,                        /* option name */
                0.2,                          /* default value */
                false,                          /* is option mandatory */
                'r',                           /* short option name */
                "ratio",                      /* long option name */
                "DOUBLE",                           /* parametr name */
                "directly bound solvent ratio")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                DoNotUseDirectAsSolvent,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "directnotsolvent",                      /* long option name */
                NULL,                           /* parametr name */
                "do not use direct as solvent")   /* option description */
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
