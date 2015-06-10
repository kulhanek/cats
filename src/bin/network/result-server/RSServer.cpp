// =============================================================================
// PMFLib - Library Supporting Potential of Mean Force Calculations
// -----------------------------------------------------------------------------
//    Copyright (C) 2008 Petr Kulhanek, kulhanek@enzim.hu
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
#include <signal.h>
#include <ErrorSystem.hpp>
#include <SmallTime.hpp>
#include <CmdProcessorList.hpp>
#include <CATsOperation.hpp>
#include <PrmUtils.hpp>
#include "RSServer.h"
#include "RSProcessor.h"
#include "RSFactory.h"
#include <ExtraOperation.hpp>

//------------------------------------------------------------------------------

CRSServer RSServer;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CRSServer::CRSServer(void)
{
    Statistics = true;
    StatisticsFileName = "statistics.log";
    SetProtocolName("res");
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CRSServer::Init(int argc,char* argv[])
{
    // encode program options, all check procedures are done inside of CRSIntOpts
    int result = Options.ParseCmdLine(argc,argv);

    // should we exit or was it error?
    if( result != SO_CONTINUE ) return(result);

    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    printf("\n");
    printf("# ==============================================================================\n");
    printf("# result-server started at %s\n",(const char*)dt.GetSDateAndTime());
    printf("# ==============================================================================\n");
    printf("#\n");
    if( Options.GetArgControlFile() != "-" ) {
        printf("# Control file name: %s\n",(const char*)Options.GetArgControlFile());
    } else {
        printf("# Control file name: - (standard input)\n");
    }

    // set SIGINT hadler to cleanly shutdown server ----------
    signal(SIGINT,CtrlCSignalHandler);

    // process control file ----------------------------------
    if( Options.GetArgControlFile() != "-" ) {
        if( Controls.Read(Options.GetArgControlFile()) == false ) {
            CSmallString error;
            error << ">>> ERROR: Unable to open and read control file!\n";
            fprintf(stderr,"%s",(const char*)error);
            return( SO_USER_ERROR );
        }
    } else {
        if( Controls.Parse(stdin) == false ) {
            CSmallString error;
            error << ">>> ERROR: Unable to read control file from standard input!\n";
            fprintf(stderr,"%s",(const char*)error);
            return( SO_USER_ERROR );
        }
    }

    if( ProcessServerControl(Controls) == false ) {
        return( SO_USER_ERROR );
    }

    if( ProcessFileControl(Controls) == false ) {
        return( SO_USER_ERROR );
    }

    if( Controls.CountULines() > 0 ) {
        printf("\n");
        printf("# Following lines from control file were not understood:\n");
        printf("# ------------------------------------------------------\n");
        Controls.Dump(stdout,true);
        printf("\n");
        CSmallString error;
        error << ">>> ERROR: Check your control file for misspelling keywords!\n";
        fprintf(stderr,"%s",(const char*)error);
        return(false);
    }

    return( SO_CONTINUE );
}

//------------------------------------------------------------------------------

bool CRSServer::ProcessFileControl(CPrmFile& conflie)
{
    // files setup ---------------------------------

    printf("\n");
    printf("=== [files] ====================================================================\n");
    if( conflie.OpenSection("files") == false ) {
        CSmallString error;
        error << ">>> ERROR: Unable to open [files] section in the control file.\n";
        fprintf(stderr,"%s",(const char*)error);
        return(false);
    }

    if( conflie.GetStringByKey("results",ResultsFileName) == true ) {
        printf("results                          = %s\n",(const char*)ResultsFileName);
    } else {
        printf("results                          = none                                (default)\n");
    }

    if( Statistics == true ) {
        if( conflie.GetStringByKey("statistics",StatisticsFileName) == true ) {
            printf("statistics                       = %s\n",(const char*)StatisticsFileName);
        } else {
            printf("statistics                       = %-15s                     (default)\n",
                   (const char*)StatisticsFileName);
        }
    } else {
        printf("statistics                       = not performed                       (default)\n");
    }

    if( (ResultsFileName == NULL) && (Statistics == false) ) {
        CSmallString error;
        error << ">>> ERROR: No result file and no statistics!\n";
        fprintf(stderr,"%s",(const char*)error);
        return(false);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CRSServer::Run(void)
{
    printf("\n");
    printf("::::::::::::::::::::::::::::::::: Result Server ::::::::::::::::::::::::::::::::\n");

    // register operations
    CmdProcessorList.RegisterProcessor(Operation_RegisterClient,&RSFactory);
    CmdProcessorList.RegisterProcessor(Operation_FlushServerData,&RSFactory);
    CmdProcessorList.RegisterProcessor(Operation_WriteData,&RSFactory);
    CmdProcessorList.RegisterProcessor(Operation_GetStatistics,&RSFactory);

    ResultFile.SetResultFileName(ResultsFileName);
    if( Statistics ) ResultFile.SetStatisticsFileName(StatisticsFileName);

    if( StartServer() == false ) {
        return(false);
    }

    printf("\n");
    printf(":::::::::::::::::::::::::::::::: Server utilization ::::::::::::::::::::::::::::\n");

    printf("\n");
    printf("Number of processed transactions: %d\n",GetNumberOfTransactions());
    printf("Number of illegal transactions  : %d\n",GetNumberOfIllegalTransactions());

    RegClients.PrintInfo();

    printf("\n");
    printf("::::::::::::::::::::::::::::::::::: Output data ::::::::::::::::::::::::::::::::\n");

    bool result = true;

    if( ResultsFileName != NULL ) {
        if( ResultFile.GetNumberOfCollectedData() > 0 ) {
            printf("Result file closed ...\n");
            if( ResultFile.CloseResultFile() == false ) {
                fprintf(stderr,"\n");
                fprintf(stderr,">>> ERROR: Unable to close result file!");
                fprintf(stderr,"\n");
                result = false;
            }
        } else {
            printf("No results were collected.\n");
        }
    } else {
        printf("No result file requested.\n");
    }

    if( Statistics && (StatisticsFileName != NULL) ) {
        printf("Statistics file written ...\n");
        if( ResultFile.WriteStatistics() == false ) {
            fprintf(stderr,"\n");
            fprintf(stderr,">>> ERROR: Unable to write statistics file!");
            fprintf(stderr,"\n");
            result = false;
        }
    } else {
        printf("No statistics file requested.\n");
    }

    return(result);
}

//------------------------------------------------------------------------------

bool CRSServer::Finalize(void)
{
    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    fprintf(stdout,"\n");
    fprintf(stdout,"# ==============================================================================\n");
    fprintf(stdout,"# %s terminated at %s\n",(const char*)Options.GetProgramName(),(const char*)dt.GetSDateAndTime());
    fprintf(stdout,"# ==============================================================================\n");

    if( Options.GetOptVerbose() || ErrorSystem.IsError() ){
        ErrorSystem.PrintErrors(stderr);
    }

    fprintf(stdout,"\n");

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CRSServer::CtrlCSignalHandler(int signal)
{
    printf("\nCtrl+C signal recieved.\n   Initiating server shutdown ... \n");
    if( RSServer.Options.GetOptVerbose() ) printf("Waiting for server finalization ... \n");
    fflush(stdout);
    RSServer.TerminateServer();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
