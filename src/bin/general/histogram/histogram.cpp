// =============================================================================
// PMFLib - Library Supporting Potential of Mean Force Calculations
// -----------------------------------------------------------------------------
//    Copyright (C) 2008 Petr Kulhanek, kulhanek@enzim.hu
//    Copyright (C) 2007 Petr Kulhanek, kulhanek@enzim.hu
//    Copyright (C) 2006 Petr Kulhanek, kulhanek@chemi.muni.cz &
//                       Martin Petrek, petrek@chemi.muni.cz
//    Copyright (C) 2005 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include "histogram.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CHistogram::CHistogram(void)
{
    InputFile = NULL;
    OwnInputFile = false;
    OutputFile = NULL;
    OwnOutputFile = false;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CHistogram::Init(int argc,char* argv[])
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
    if( Options.GetOptNormalize() == true ) {
        OutputFormat += " ";
        OutputFormat += Options.GetOptORFormat();
        OutputFormat += "\n";
    } else {
        OutputFormat += " ";
        OutputFormat += Options.GetOptONFormat();
        OutputFormat += "\n";
    }

    return(result);
}

//------------------------------------------------------------------------------

bool CHistogram::Run(void)
{
    if( (InputFile == NULL) && (OutputFile == NULL) ) return(false);   // files are not opened

    // read data ------------------------------------
    if( Data.ReadData(InputFile,
                      Options.GetOptSkipLines(),
                      Options.GetOptAnalLines(),
                      Options.GetOptPadLines(),
                      Options.GetOptColumnIndex()) == false ) {
        return(false);
    }

    // do statistics --------------------------------
    int     tot_count=0;
    int     hist_count=0;
    double  maxv=0;
    double  minv=0;
    double  start = Options.GetOptStartValue();
    double  stop = Options.GetOptStopValue();
    double  dch = 0.0;

    tot_count = Data.GetSize();
    if( tot_count > 0 ) {
        maxv = Data[0];
        minv = Data[0];
    }

    for(int i=0; i<tot_count; i++) {
        if( maxv < Data[i] ) maxv = Data[i];
        if( minv > Data[i] ) minv = Data[i];
    }

    if( Options.IsOptStartValueSet() == false ) start = minv;
    if( Options.IsOptStartValueSet() == false ) stop = maxv;

    if( start >= stop ) {
        fprintf(stderr,"%s: start value '%f' must be less than stop value '%f'\n",
                (char*)Options.GetProgramName(),start,stop);
        return(false);
    }

    // calculate histogram --------------------------
    Histogram.CreateVector(Options.GetOptNBins());

    Histogram.SetZero();

    dch = (stop - start)/Options.GetOptNBins();

    for(int i=0; i<tot_count; i++) {
        double value = Data[i];
        // find channel
        int index = floor((value - start)/dch);
        if( (index >= 0)&&(index < Options.GetOptNBins()) ) {
            Histogram[index]++;
            hist_count++;
        } else {
            if( value == stop ) {
                Histogram[Options.GetOptNBins()-1]++;
                hist_count++;
            }
        }
    }

    // print data -----------------------------------

    if( Options.GetOptNoHeader() == false ) {
        fprintf(OutputFile,"# Data information\n");
        fprintf(OutputFile,"# ===================================\n");
        fprintf(OutputFile,"# Minimal value   : %f\n",minv);
        fprintf(OutputFile,"# Maximal value   : %f\n",maxv);
        fprintf(OutputFile,"# Number of values: %d\n",tot_count);
        fprintf(OutputFile,"#\n");
        fprintf(OutputFile,"# Histogram analysis\n");
        fprintf(OutputFile,"# ===================================\n");
        fprintf(OutputFile,"# Start value     : %f\n",start);
        fprintf(OutputFile,"# Stop  value     : %f\n",stop);
        fprintf(OutputFile,"# Number of values: %d\n",hist_count);
        fprintf(OutputFile,"# Number of bins  : %d\n",Options.GetOptNBins());
        fprintf(OutputFile,"# Bin width       : %f\n",dch);
        fprintf(OutputFile,"# \n");

        // shell we update header sizes according to used formats? - maybe in the next version?
        fprintf(OutputFile,"#               1                2\n");
        fprintf(OutputFile,"#               X             N(X)\n");
        fprintf(OutputFile,"#  --------------- ---------------\n");
    }

    bool result = true;

    for(int i=0; i<Options.GetOptNBins(); i++) {
        double x_value = start + dch*(i + 0.5);
        int    y_value = Histogram[i];
        double norm_value = y_value / (hist_count * dch);
        if( Options.GetOptNormalize() ) {
            if( fprintf(OutputFile,OutputFormat,x_value,norm_value) <= 0 ) {
                fprintf(stderr,"%s: unable to write to output file\n",
                        (const char*)Options.GetProgramName());
                result = false;
                break;
            }
        } else {
            if( fprintf(OutputFile,OutputFormat,x_value,y_value) <= 0 ) {
                fprintf(stderr,"%s: unable to write to output file\n",
                        (const char*)Options.GetProgramName());
                result = false;
                break;
            }
        }
    }

    return(result);
}

//------------------------------------------------------------------------------

bool CHistogram::Finalize(void)
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

void CHistogram::PrintProgHeader(FILE* fout)
{
    fprintf(stdout,"# ==============================================================================\n");
    fprintf(stdout,"# histogram (CATs utility)\n");
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
    fprintf(stdout,"# Analysed column : %10d\n",Options.GetOptColumnIndex());
    fprintf(stdout,"# ------------------------------------------------------------------------------\n");
    fprintf(stdout,"# Start value     : %f\n",Options.GetOptStartValue());
    fprintf(stdout,"# Stop value      : %f\n",Options.GetOptStopValue());
    fprintf(stdout,"# Number of bins  : %10d\n",Options.GetOptNBins());
    fprintf(stdout,"# Normalize       : %s\n",bool_to_str(Options.GetOptNormalize()));
    fprintf(stdout,"# ------------------------------------------------------------------------------\n");
    fprintf(stdout,"# Print header    : %s\n",bool_to_str(!Options.GetOptNoHeader()));
    fprintf(stdout,"# Verbose         : %s\n",bool_to_str(Options.GetOptVerbose()));
    fprintf(stdout,"# ------------------------------------------------------------------------------\n");
    fprintf(stdout,"# X format        : %s\n",(const char*)Options.GetOptIXFormat());
    fprintf(stdout,"# N(X) format     : %s\n",(const char*)Options.GetOptONFormat());
    fprintf(stdout,"# r(X) format     : %s\n",(const char*)Options.GetOptORFormat());
    fprintf(stdout,"# ------------------------------------------------------------------------------\n");
    fprintf(stdout,"#\n");
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

