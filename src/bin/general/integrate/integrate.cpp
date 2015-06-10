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
#include "integrate.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CIntegrate::CIntegrate(void)
{
    InputFile = NULL;
    OwnInputFile = false;
    OutputFile = NULL;
    OwnOutputFile = false;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CIntegrate::Init(int argc,char* argv[])
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

    if( Options.GetArgOutput() == "-" ) {
        OutputFile = stdout;
        OwnOutputFile = false;
    } else {
        OutputFile = fopen(Options.GetArgOutput(),"w");
        if( OutputFile == NULL ) {
            fprintf(stderr,"%s: unable to open output file '%s'\n", (char*)Options.GetProgramName(),(const char*)Options.GetArgOutput());
            if( (OwnInputFile == true) && (InputFile != NULL) ) {
                fclose(InputFile);
                InputFile = NULL;
                OwnInputFile = false;
            }
            return(SO_USER_ERROR);
        }
        OwnOutputFile = true;
    }

    // complete output format -----------------------------------
    OutputFormat  = "   ";
    OutputFormat += Options.GetOptIXFormat();
    OutputFormat += " ";
    OutputFormat += Options.GetOptIYFormat();
    OutputFormat += " ";
    OutputFormat += Options.GetOptISFormat();
    OutputFormat += " ";
    OutputFormat += Options.GetOptOIFormat();
    OutputFormat += " ";
    OutputFormat += Options.GetOptOEFormat();
    OutputFormat += "\n";

    return(result);
}

//------------------------------------------------------------------------------

bool CIntegrate::Run(void)
{
    if( (InputFile == NULL) && (OutputFile == NULL) ) return(false);   // files are not opened

    if( Options.GetOptNoHeader() == false ) {
        // shell we update header sizes according to used formats? - maybe in the next version?
        fprintf(OutputFile,"#               1                2              3               4              5\n");
        fprintf(OutputFile,"#               X                Y       Sigma(Y)               I       Sigma(I)\n");
        fprintf(OutputFile,"#  --------------- --------------- -------------- --------------- --------------\n");
    }

    // skip lines from input stream
    int line = 1;
    int c;
    int sl =  Options.GetOptSkipLines();
    while( (sl != 0) && (feof(InputFile) == false) ) {
        c = fgetc(InputFile);
        if( c == '\n' ) sl--;
    }

    // integrate remaining data
    double px,x;
    double py,y;
    double ps,s=0.0;
    double dx,dy,ds;
    double pi=0.0;
    double psi=0.0;

    bool result = true;
    int  numofdata = 0;

    while( feof(InputFile) == false ) {

        // read data line ----------------------------------------------------------

        // read whole line
        CSmallString data_line;
        if( data_line.ReadLineFromFile(InputFile) == false ) break; // no more data

        if( Options.GetOptNoSigma() == true ) {
            s = 0.0;
            int nr = sscanf(data_line,"%le %le",&x,&y);
            if( nr <= 0 ) break;    // no more data
            if( nr != 2 ) {
                fprintf(stderr,"%s: incosistent number of data records on line %d, requested: 2, found: %d\n", (const char*)Options.GetProgramName(),line,nr);
                result = false;
                break;
            }
        } else {
            int nr = sscanf(data_line,"%le %le %le",&x,&y,&s);
            if( nr <= 0 ) break;    // no more data
            if( nr != 3 ) {
                fprintf(stderr,"%s: incosistent number of data records on line %d, requested: 3, found: %d\n", (const char*)Options.GetProgramName(),line,nr);
                result = false;
                break;
            }
        }

        // integrate data ----------------------------------------------------------
        numofdata++;

        if( numofdata > 1 ) {
            dx = x - px;
            dy = y + py;
            ds = s + ps;
            pi = 0.5*dx*dy + pi;
            psi = sqrt(0.5*dx*ds*ds + psi*psi);
        } else {
            pi = Options.GetOptIntOffset();
            psi = 0.0;
        }

        px = x;
        py = y;
        ps = s;

        // print data --------------------------------------------------------------
        if( fprintf(OutputFile,OutputFormat,x,y,s,pi,psi) <= 0 ) {
            fprintf(stderr,"%s: unable to write to output file\n", (const char*)Options.GetProgramName());
            result = false;
            break;
        }

        // did we analyzed requested number of lines?
        if( numofdata == Options.GetOptAnalLines() ) break;

        // skip requested number of lines -----------------------------------------
        int sl =  Options.GetOptPadLines();
        while( (sl != 0) && (feof(InputFile) == false) ) {
            c = fgetc(InputFile);
            if( c == '\n' ) sl--;
        }
    }

    return(result);
}

//------------------------------------------------------------------------------

bool CIntegrate::Finalize(void)
{
    // close files if they are own by program
    if( (OwnInputFile == true) && (InputFile != NULL) ) {
        fclose(InputFile);
        InputFile = NULL;
        OwnInputFile = false;
    }
    if( (OwnOutputFile == true) && (OutputFile != NULL) ) {
        fclose(OutputFile);
        OutputFile = NULL;
        OwnOutputFile = false;
    }
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CIntegrate::PrintProgHeader(FILE* fout)
{
    fprintf(stdout,"# ==============================================================================\n");
    fprintf(stdout,"# integrate (CATs utility)\n");
    fprintf(stdout,"# ==============================================================================\n");

    if( Options.GetArgInput() == "-" ) {
        fprintf(stdout,"# Input file      : standard input stream (stdin)\n");
    } else {
        fprintf(stdout,"# Input file      : %s\n",(const char*)Options.GetArgInput());
    }
    if( Options.GetArgOutput() == "-" ) {
        fprintf(stdout,"# Output file     : standard output stream (stdout)\n");
    } else {
        fprintf(stdout,"# Output file     : %s\n",(const char*)Options.GetArgOutput());
    }
    fprintf(stdout,"# ------------------------------------------------------------------------------\n");
    fprintf(stdout,"# Skipped lines   : %10d\n",Options.GetOptSkipLines());
    fprintf(stdout,"# Analysed lines  : %10d\n",Options.GetOptAnalLines());
    fprintf(stdout,"# Padding lines   : %10d\n",Options.GetOptPadLines());
    fprintf(stdout,"# Int. constant   : %f\n",Options.GetOptIntOffset());
    fprintf(stdout,"# ------------------------------------------------------------------------------\n");
    fprintf(stdout,"# Sigma values    : %s\n",bool_to_str(!Options.GetOptNoSigma()));
    fprintf(stdout,"# Print header    : %s\n",bool_to_str(!Options.GetOptNoHeader()));
    fprintf(stdout,"# Verbose         : %s\n",bool_to_str(Options.GetOptVerbose()));
    fprintf(stdout,"# ------------------------------------------------------------------------------\n");
    fprintf(stdout,"# X format        : %s\n",(const char*)Options.GetOptIXFormat());
    fprintf(stdout,"# Y format        : %s\n",(const char*)Options.GetOptIYFormat());
    fprintf(stdout,"# sigma(Y) format : %s\n",(const char*)Options.GetOptISFormat());
    fprintf(stdout,"# I format        : %s\n",(const char*)Options.GetOptOIFormat());
    fprintf(stdout,"# sigma(I) format : %s\n",(const char*)Options.GetOptOEFormat());
    fprintf(stdout,"# ------------------------------------------------------------------------------\n");
    fprintf(stdout,"#\n");
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================




