#ifndef _PROPSUM_H
#define _PROPSUM_H
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

#include <stdio.h>
#include <DynamicArray.hpp>
#include "propsum-opts.hpp"
#include <VerboseStr.hpp>
#include <TerminalStr.hpp>

//------------------------------------------------------------------------------

class CPropSum {
public:
    // constructor
    CPropSum(void);

// main methods ---------------------------------------------------------------
    /// init options
    int Init(int argc,char* argv[]);

    /// main part of program
    bool Run(void);

    /// finalize program
    bool Finalize(void);

// section of private data ----------------------------------------------------
private:
    CPropSumOpts    Options;            // program options
    FILE*           InputFile;          // input file
    bool            OwnInputFile;       // do we own input file handle?
    FILE*           OutputFile;         // output file
    CDynamicArray   Data;
    CSmallString    OutputFormat;       // output format

    /// print program header and specified options
    void PrintProgHeader(FILE* fout=NULL);
};

//------------------------------------------------------------------------------

#endif
