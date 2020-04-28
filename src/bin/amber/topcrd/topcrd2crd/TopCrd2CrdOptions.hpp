#ifndef TopCrd2CrdOptionsH
#define TopCrd2CrdOptionsH
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

class CTopCrd2CrdOptions : public CSimpleOptions {
public:
    // constructor - tune option setup
    CTopCrd2CrdOptions(void);

// program name and description -----------------------------------------------
    CSO_PROG_NAME_BEGIN
    "topcrd2crd"
    CSO_PROG_NAME_END

    CSO_PROG_DESC_BEGIN
    "Converts AMBER topology and coordinates to coordinates in other format."
    CSO_PROG_DESC_END

    CSO_PROG_VERS_BEGIN
    LibBuildVersion_CATs
    CSO_PROG_VERS_END

// list of all options and arguments ------------------------------------------
    CSO_LIST_BEGIN
    // arguments ----------------------------
    CSO_ARG(CSmallString,TopologyName)
    CSO_ARG(CSmallString,CrdName)
    CSO_ARG(CSmallString,OutName)
    // options ------------------------------
    CSO_OPT(CSmallString,InputFormat)
    CSO_OPT(CSmallString,OutputFormat)
    CSO_OPT(CSmallString,MaskSpec)
    CSO_OPT(CSmallString,MaskFile)
    CSO_OPT(CSmallString,ChrgMaskSpec)
    CSO_OPT(CSmallString,ChrgMaskFile)
    CSO_OPT(CSmallString,MangleNames)
    CSO_OPT(double,RScale)
    CSO_OPT(double,ROffset)
    CSO_OPT(bool,Reindex)
    CSO_OPT(bool,CopyBox)
    CSO_OPT(bool,NoVelocities)
    CSO_OPT(bool,NoChains)
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
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                CrdName,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "CRD",                           /* parametr name */
                "input coordinates file name or - for standard input")   /* argument description */
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                OutName,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "OUT",                           /* parametr name */
                "output coordinates file name or - for standard output")   /* argument description */

// description of options -----------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                InputFormat,                        /* option name */
                "crd",                          /* default value */
                false,                          /* is option mandatory */
                'i',                           /* short option name */
                "input",                      /* long option name */
                "FORMAT",                           /* parametr name */
                "specify input format for coordinates:\n"
                "   <green>crd</green>  - amber restart file\n"
                "   <green>xyz</green>  - xyz file\n")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                OutputFormat,                        /* option name */
                "pdb",                          /* default value */
                false,                          /* is option mandatory */
                'o',                           /* short option name */
                "output",                      /* long option name */
                "FORMAT",                           /* parametr name */
                "specify output format for coordinates:\n"
                "   <green>crd</green>        - amber restart file (ASCII)\n"
                "   <green>bcrd</green>       - amber restart file (NetCDF)\n"
                "   <green>pdb</green>        - PDB file\n"
                "   <green>pqr</green>        - PDB file with atomic radii and charges\n"
                "   <green>xyz</green>        - xyz file\n"
                "   <green>xyzr</green>       - xyz file with atomic radii\n"
                "   <green>com</green>        - gaussian input file\n"
                "   <green>orca</green>       - orca input file\n"
                "   <green>pc</green>         - point charges file\n"
                "   <green>rbox</green>       - rbox format for qhull\n"
                "   <green>traj</green>       - amber trajectory file\n"
                "   <green>adf</green>        - plain ADF input\n"
                "   <green>adfgeo</green>     - plain ADF input, only geometry\n"
                "   <green>adffrag</green>    - plain ADF input with one fragment\n"
                "   <green>adf3drism</green>  - plain ADF input for 3D-RISM"
                )   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                MaskSpec,                        /* option name */
                "",                          /* default value */
                false,                          /* is option mandatory */
                'm',                           /* short option name */
                "mask",                      /* long option name */
                "MASK",                           /* parametr name */
                "only atoms selected according to MASK will be converted otherwise all atoms are used. Mutually exclusive with 'maskfile' option.")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                MaskFile,                        /* option name */
                NULL,                          /* default value */
                false,                          /* is option mandatory */
                'f',                           /* short option name */
                "maskfile",                      /* long option name */
                "MASKFILE",                           /* parametr name */
                "only atoms selected according to the mask will be converted otherwise all atoms are used. The mask specification is read from the first line of the file of name MASKFILE. This option is suitable for the specification of very long masks. Mutually exclusive with 'mask' option.")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                ChrgMaskSpec,                        /* option name */
                "",                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "chrgmask",                      /* long option name */
                "MASK",                           /* parametr name */
                "only atoms selected according to MASK will be included as point charges. Mutually exclusive with 'chrgmaskfile' option.")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                ChrgMaskFile,                        /* option name */
                NULL,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "chrgmaskfile",                      /* long option name */
                "MASKFILE",                           /* parametr name */
                "only atoms selected according to the mask will be included as point charges. The mask specification is read from the first line of the file of name MASKFILE. This option is suitable for the specification of very long masks. Mutually exclusive with 'chrgmask' option.")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                MangleNames,                        /* option name */
                "pdb",                          /* default value */
                false,                          /* is option mandatory */
                't',                           /* short option name */
                "mangle",                      /* long option name */
                "FORMAT",                           /* parametr name */
                "specify atom names mangling scheme:\n"
                "   <green>none</green>   - no translation (1234)\n"
                "   <green>pdb</green>    - pdb scheme (4123), keep amber names\n"
                "   <green>charmm</green> - charmm name scheme (1234), mangle parm99 names\n"
                )   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(double,                           /* option type */
                RScale,                        /* option name */
                1.0,                          /* default value */
                false,                          /* is option mandatory */
                0,                           /* short option name */
                "rscale",                      /* long option name */
                NULL,                           /* parametr name */
                "scale factor for atomic radii (r_new = r * scale + offset)")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(double,                           /* option type */
                ROffset,                        /* option name */
                0.0,                          /* default value */
                false,                          /* is option mandatory */
                0,                           /* short option name */
                "roffset",                      /* long option name */
                NULL,                           /* parametr name */
                "offset for atomic radii (r_new = r * scale + offset)")   /* option description */

    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                Reindex,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                'r',                           /* short option name */
                "reindex",                      /* long option name */
                NULL,                           /* parametr name */
                "reindex atom and residue numbers counting from 1")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                CopyBox,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                0,                           /* short option name */
                "copybox",                      /* long option name */
                NULL,                           /* parametr name */
                "copy box information to output file")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                NoVelocities,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                0,                           /* short option name */
                "novelocities",                      /* long option name */
                NULL,                           /* parametr name */
                "do not transfer velocities")   /* option description */

    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                NoChains,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                0,                           /* short option name */
                "nochains",                      /* long option name */
                NULL,                           /* parametr name */
                "do not print chain IDs into pdb files")   /* option description */
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
