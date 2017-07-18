#ifndef GAUSS_AW_GEN_H
#define GAUSS_AW_GEN_H
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

#include <CATsMainHeader.hpp>
#include <stdio.h>
#include <SimpleVector.hpp>
#include "gauss-aw-gen-opts.hpp"
#include <VerboseStr.hpp>
#include <TerminalStr.hpp>

//------------------------------------------------------------------------------

class CGaussAWGen {
public:
    // constructor
    CGaussAWGen(void);

// main methods ---------------------------------------------------------------
    /// init options
    int Init(int argc,char* argv[]);

    /// main part of program
    bool Run(void);

    /// finalize program
    bool Finalize(void);

// section of private data ----------------------------------------------------
private:
    CGaussAWGenOpts         Options;            // program options
    FILE*                   OutputFile;         // output file
    bool                    OwnOutputFile;      // do we own output file handle?
    CSmallString            OutputFormat;
    CSimpleVector<double>   Abscissas;
    CSimpleVector<double>   Weights;

    // abscissas and weights from Legendre polynoms
    void LegendrePolynom(void);

    /// print program header and specified options
    void PrintProgHeader(FILE* fout=NULL);
};

//------------------------------------------------------------------------------

#endif
