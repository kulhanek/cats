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
#include <math.h>
#include "propsum.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPropSum::CPropSum(void)
{
    InputFile = NULL;
    OwnInputFile = false;
    OutputFile = NULL;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CPropSum::Init(int argc,char* argv[])
{
    // encode program options, all check procedures are done inside of CABFIntOpts
    int result = Options.ParseCmdLine(argc,argv);

    // should we exit or was it error?
    if( result != SO_CONTINUE ) return(result);

    // print header if requested --------------------
    if( Options.GetOptVerbose() == true ) PrintProgHeader();

    // open files -----------------------------------
    if( Options.GetArgInput() == "-" ) {
        InputFile = stdin;
        OwnInputFile = false;
    } else {
        InputFile = fopen(Options.GetArgInput(),"r");
        if( InputFile == NULL ) {
            fprintf(stderr,"%s: unable to open input file '%s'\n", (char*)Options.GetProgramName(),(const char*)Options.GetArgInput());
            return(SO_USER_ERROR);
        }
        OwnInputFile = true;
    }

    OutputFile = stdout;

    // complete output format -----------------------------------
    OutputFormat  = "   ";
    OutputFormat += Options.GetOptINFormat();
    if( Options.GetOptIntermediates() == true ) {
        OutputFormat += " ";
        OutputFormat += Options.GetOptIXFormat();
    }
    OutputFormat += " ";
    OutputFormat += Options.GetOptOSumFormat();
    OutputFormat += " ";
    OutputFormat += Options.GetOptOAveFormat();
    OutputFormat += " ";
    OutputFormat += Options.GetOptORMSDFormat();
    OutputFormat += " ";
    OutputFormat += Options.GetOptOSigmaFormat();
    OutputFormat += "\n";

    return(result);
}

//------------------------------------------------------------------------------

bool CPropSum::Run(void)
{
    if( (InputFile == NULL) && (OutputFile == NULL) ) return(false);   // files are not opened

    if( (Options.GetOptIntermediates() == true) && (Options.GetOptNoHeader() == false) ) {
        // shell we update header sizes according to used formats? - maybe in the next version?
        fprintf(OutputFile,"#               1                2              3              4               5              6\n");
        fprintf(OutputFile,"#               N                X         Sum(X)            <X>         RMSD(X)     Sigma(<X>)\n");
        fprintf(OutputFile,"#  --------------- --------------- -------------- -------------- --------------- --------------\n");
    }

    if( (Options.GetOptIntermediates() == false) && (Options.GetOptNoHeader() == false) ) {
        // shell we update header sizes according to used formats? - maybe in the next version?
        fprintf(OutputFile,"#               1              2              3               4              5\n");
        fprintf(OutputFile,"#               N          Sum(X)            <X>         RMSD(X)     Sigma(<X>)\n");
        fprintf(OutputFile,"#  --------------- -------------- -------------- --------------- --------------\n");
    }


    int    count   = 0;
    int    n       = 0;
    double x       = 0.0;
    double ave     = 0.0;
    double rmsd    = 0.0;
    double sigma   = 0.0;
    double ms      = 0.0;
    double sum     = 0.0;
    double sum2    = 0.0;

    // read data
    if( Data.ReadData(InputFile,
                      Options.GetOptSkipLines(),
                      Options.GetOptAnalLines(),
                      Options.GetOptPadLines(),
                      Options.GetOptColumnIndex()) == false ) {
        return(false);
    }

    bool result = true;

    // analyze data
    count = Data.GetSize();

    for(int i=0; i < count; i++) {
        n = i + 1;
        x = Data[i];
        sum += x;
        sum2 += x*x;
        ave = sum / n;
        ms = n*sum2 - sum*sum;
        if( ms > 0 ) {
            rmsd = sqrt(ms) / n;
            sigma = rmsd / sqrt(n);
        } else {
            rmsd = 0.0;
            sigma = 0.0;
        }

        // print data --------------------------------------------------------------
        if( Options.GetOptIntermediates() == true ) {
            if( fprintf(OutputFile,OutputFormat,n,x,sum,ave,rmsd,sigma) <= 0 ) {
                fprintf(stderr,"%s: unable to write to output file\n", (const char*)Options.GetProgramName());
                result = false;
                break;
            }
        }
    }

    if( Options.GetOptIntermediates() == false ) {
        if( fprintf(OutputFile,OutputFormat,n,sum,ave,rmsd,sigma) <= 0 ) {
            fprintf(stderr,"%s: unable to write to output file\n", (const char*)Options.GetProgramName());
            result = false;
        }
    }

    return(result);
}

//------------------------------------------------------------------------------

bool CPropSum::Finalize(void)
{
    // close files if they are own by program
    if( (OwnInputFile == true) && (InputFile != NULL) ) {
        fclose(InputFile);
        InputFile = NULL;
        OwnInputFile = false;
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPropSum::PrintProgHeader(FILE* fout)
{
    fprintf(stdout,"# ==============================================================================\n");
    fprintf(stdout,"# propsum (CATs utility)\n");
    fprintf(stdout,"# ==============================================================================\n");

    if( Options.GetArgInput() == "-" ) {
        fprintf(stdout,"# Input file      : standard input stream (stdin)\n");
    } else {
        fprintf(stdout,"# Input file      : %s\n",(const char*)Options.GetArgInput());
    }
    fprintf(stdout,"# Output file     : standard output stream (stdout)\n");
    fprintf(stdout,"# ------------------------------------------------------------------------------\n");
    fprintf(stdout,"# Skipped lines   : %10d\n",Options.GetOptSkipLines());
    fprintf(stdout,"# Analysed lines  : %10d\n",Options.GetOptAnalLines());
    fprintf(stdout,"# Padding lines   : %10d\n",Options.GetOptPadLines());
    fprintf(stdout,"# Analysed column : %10d\n",Options.GetOptColumnIndex());
    fprintf(stdout,"# ------------------------------------------------------------------------------\n");
    fprintf(stdout,"# Intermediates   : %s\n",bool_to_str(Options.GetOptIntermediates()));
    fprintf(stdout,"# Print header    : %s\n",bool_to_str(!Options.GetOptNoHeader()));
    fprintf(stdout,"# Verbose         : %s\n",bool_to_str(Options.GetOptVerbose()));
    fprintf(stdout,"# ------------------------------------------------------------------------------\n");
    fprintf(stdout,"# N format        : %s\n",(const char*)Options.GetOptINFormat());
    fprintf(stdout,"# X format        : %s\n",(const char*)Options.GetOptIXFormat());
    fprintf(stdout,"# sum(X) format   : %s\n",(const char*)Options.GetOptOSumFormat());
    fprintf(stdout,"# ave(X) format   : %s\n",(const char*)Options.GetOptOAveFormat());
    fprintf(stdout,"# RMSD(X) format  : %s\n",(const char*)Options.GetOptORMSDFormat());
    fprintf(stdout,"# Sigma(X) format : %s\n",(const char*)Options.GetOptOSigmaFormat());
    fprintf(stdout,"# ------------------------------------------------------------------------------\n");
    fprintf(stdout,"#\n");
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

