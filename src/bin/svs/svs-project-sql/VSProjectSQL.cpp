// =============================================================================
// VScreen - Virtual Screening Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include "VSProjectSQL.hpp"
#include "VSProjectSQLOptions.hpp"

using namespace std;

//------------------------------------------------------------------------------

CVSProjectSQL VSProjectSQL;

//------------------------------------------------------------------------------

extern "C" {
int my_main(int argc, char **argv);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CVSProjectSQL::CVSProjectSQL(void)
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CVSProjectSQL::Init(int argc,char* argv[])
{
    // encode program options, all check procedures are done inside of CVSProjectSQLOptions
    int result = Options.ParseCmdLine(argc,argv);

    // should we exit or was it error?
    if(result != SO_CONTINUE) return(result);

    // set output stream
    MsgOut.Attach(Console);
    if(Options.GetOptVerbose()) MsgOut.Verbosity(CVerboseStr::high);

    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();
    MsgOut << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# svs-project-sql started at " << dt.GetSDateAndTime() << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << endl;

    // run the CLI
    my_main(argc,argv);

    return(SO_CONTINUE);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CVSProjectSQL::Run(void)
{
    // nothing to be here
    return(true);
}

//------------------------------------------------------------------------------

bool CVSProjectSQL::Finalize(void)
{
    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    MsgOut << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# svs-project-sql terminated at " << dt.GetSDateAndTime() << endl;
    MsgOut << "# ==============================================================================" << endl;

    if(ErrorSystem.IsError() || Options.GetOptVerbose()) {
        ErrorSystem.PrintErrors(stderr);
    }

    MsgOut << endl;

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

