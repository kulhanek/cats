#ifndef CubeGridGenOptionsH
#define CubeGridGenOptionsH
// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2018 Petr Kulhanek, kulhanek@chemi.muni.cz
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

class CCubeGridGenOptions : public CSimpleOptions {
public:
    // constructor - tune option setup
    CCubeGridGenOptions(void);

// program name and description -----------------------------------------------
    CSO_PROG_NAME_BEGIN
    "cube-gridgen"
    CSO_PROG_NAME_END

    CSO_PROG_DESC_BEGIN
    "Generate grid points for a CUBE file suitable for calculation of properties at their positions. The cube file has rectangular axes and it is centered at the origin."
    CSO_PROG_DESC_END

    CSO_PROG_VERS_BEGIN
    LibBuildVersion_CATs
    CSO_PROG_VERS_END

// list of all options and arguments ------------------------------------------
    CSO_LIST_BEGIN
    // arguments ----------------------------
    CSO_ARG(CSmallString,GridPoints)
    // options ------------------------------
    CSO_OPT(double,SX)
    CSO_OPT(double,SY)
    CSO_OPT(double,SZ)
    CSO_OPT(int,NX)
    CSO_OPT(int,NY)
    CSO_OPT(int,NZ)
    CSO_OPT(int,BatchSize)
    CSO_OPT(bool,SymmetryXY)
    CSO_OPT(bool,SymmetryYZ)
    CSO_OPT(bool,SymmetryXZ)
    CSO_OPT(CSmallString,Symbol)
    CSO_OPT(CSmallString,Structure)
    CSO_OPT(double,Treshold)
    CSO_OPT(bool,Help)
    CSO_OPT(bool,Version)
    CSO_OPT(bool,Verbose)     
    CSO_LIST_END

    CSO_MAP_BEGIN
// description of arguments ---------------------------------------------------
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                GridPoints,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "POINTS",                           /* parametr name */
                "file name for grid points in the xyz file format or - for the standard output")   /* argument description */
// description of options -----------------------------------------------------
    CSO_MAP_OPT(double,                           /* option type */
                SX,                        /* option name */
                0.25,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "sx",                      /* long option name */
                "REAL",                           /* parametr name */
                "grid spacing in the x-direction")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(double,                           /* option type */
                SY,                        /* option name */
                0.25,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "sy",                      /* long option name */
                "REAL",                           /* parametr name */
                "grid spacing in the y-direction")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(double,                           /* option type */
                SZ,                        /* option name */
                0.25,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "sz",                      /* long option name */
                "REAL",                           /* parametr name */
                "grid spacing in the z-direction")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(int,                           /* option type */
                NX,                        /* option name */
                100,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "nx",                      /* long option name */
                "INT",                           /* parametr name */
                "number of grid points in the x-direction")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(int,                           /* option type */
                NY,                        /* option name */
                100,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "ny",                      /* long option name */
                "INT",                           /* parametr name */
                "number of grid points in the y-direction")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(int,                           /* option type */
                NZ,                        /* option name */
                100,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "nz",                      /* long option name */
                "INT",                           /* parametr name */
                "number of grid points in the z-direction")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(int,                           /* option type */
                BatchSize,                        /* option name */
                0,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "batchsize",                      /* long option name */
                "INT",                           /* parametr name */
                "number of grid points per a batch (0 requests for a single batch)")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                SymmetryXY,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "xysymm",                      /* long option name */
                NULL,                           /* parametr name */
                "enable symmetry in the xy-plane")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                SymmetryYZ,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "yzsymm",                      /* long option name */
                NULL,                           /* parametr name */
                "enable symmetry in the yz-plane")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                SymmetryXZ,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "xzsymm",                      /* long option name */
                NULL,                           /* parametr name */
                "enable symmetry in the xz-plane")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                Symbol,                        /* option name */
                "Bq",                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "symbol",                      /* long option name */
                "NAME",                           /* parametr name */
                "atom symbol for the xyz file")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                Structure,                        /* option name */
                NULL,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "structure",                      /* long option name */
                "NAME",                           /* parametr name */
                "structure in the xyz format for grid point filtereing")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(double,                           /* option type */
                Treshold,                        /* option name */
                0.5,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "treshold",                      /* long option name */
                "REAL",                           /* parametr name */
                "minimum allowed distance between grid point and any atom in the structure")   /* option description */
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
