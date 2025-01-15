// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2025 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <ErrorSystem.hpp>
#include <SmallTime.hpp>
#include <SmallTimeAndDate.hpp>
#include <FileSystem.hpp>
#include <FileName.hpp>
#include <sstream>
#include <Transformation.hpp>
#include <iomanip>

#include "InOutFmt.hpp"
#include "InOutFmtOptions.hpp"

// openbabel
#include "openbabel/mol.h"
#include "openbabel/atom.h"
#include "openbabel/residue.h"
#include "openbabel/obconversion.h"
#include "openbabel/generic.h"
#include "openbabel/obiter.h"


using namespace std;
using namespace OpenBabel;

//------------------------------------------------------------------------------

CInOutFmt InOutFmt;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CInOutFmt::CInOutFmt(void)
{
}

//------------------------------------------------------------------------------

CInOutFmt::~CInOutFmt(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CInOutFmt::Init(int argc,char* argv[])
{
    // encode program options, all check procedures are done inside of CInOutFmtOptions
    int result = Options.ParseCmdLine(argc,argv);

    // should we exit or was it error?
    if(result != SO_CONTINUE) return(result);

    // set output stream
    MsgOut.Attach(cout);
    MsgOut.Verbosity(CVerboseStr::low);
    if( Options.GetOptVerbose() ) MsgOut.Verbosity(CVerboseStr::high);

    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();
    MsgOut << high;
    MsgOut << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# inoutfmt started at " << dt.GetSDateAndTime() << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# Format name : " << Options.GetArgFormatName() << endl;

    return(SO_CONTINUE);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CInOutFmt::Run(void)
{
    MsgOut << endl;
    MsgOut << "::::::::::::::::::::::::::::::: Processing Data ::::::::::::::::::::::::::::::" << endl;

    OpenBabel::OBConversion conv;

    if( Options.GetArgFormatName() == "ALL" ){
        OpenBabel::OBConversion conv;

        // Containers to hold the lists of supported formats
        std::vector<std::string> inputFormats = conv.GetSupportedInputFormat();
        std::vector<std::string> outputFormats = conv.GetSupportedOutputFormat();

        // List input formats
        std::cout << std::endl;
        std::cout << "Supported Input Formats:" << std::endl;
        for (const auto &fmt : inputFormats) {
            std::cout << "  " << fmt << std::endl;
        }

        // List output formats
        std::cout << std::endl;
        std::cout << "Supported Output Formats:" << std::endl;
        for (const auto &fmt : outputFormats) {
            std::cout << "  " << fmt << std::endl;
        }
        std::cout << std::endl;
    } else {

        OpenBabel::OBFormat* obFormat = conv.FindFormat(Options.GetArgFormatName());
        if( obFormat == NULL ){
            ES_ERROR("Unable to find the specified format");
            return(false);
        }
        std::cout << std::endl;
        std::cout << "Description of '" << Options.GetArgFormatName() << "' format:" << std::endl;
        std::cout << std::endl;
        std::cout << obFormat->Description() << std::endl;
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CInOutFmt::Finalize(void)
{
    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# inoutfmt terminated at " << dt.GetSDateAndTime() << endl;
    MsgOut << "# ==============================================================================" << endl;

    if( ErrorSystem.IsError() || Options.GetOptVerbose() ){
        MsgOut << low;
        ErrorSystem.PrintErrors(stderr);
    }

    MsgOut << endl;
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

