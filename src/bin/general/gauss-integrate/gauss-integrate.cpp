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
#include "gauss-integrate.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGaussIntegrate::CGaussIntegrate(void)
{
    OutputFile = NULL;
    OwnOutputFile = false;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CGaussIntegrate::Init(int argc,char* argv[])
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

    return(result);
}

//------------------------------------------------------------------------------

bool CGaussIntegrate::Run(void)
{
    if( (InputFile == NULL) && (OutputFile == NULL) ) return(false);   // files are not opened

    Abscissas.CreateVector(Options.GetArgNPoints());
    Weights.CreateVector(Options.GetArgNPoints());

    // generate abscissas and weights
    if( Options.GetOptPolynom() == "legendre" ) {
        LegendrePolynom();
    }

    double pi,psi;
    pi = 0;
    psi = 0;
    bool result = true;

    if( (Options.GetOptNoHeader() == false) && (Options.GetOptShowProgress() == true) ) {
        // shell we update header sizes according to used formats? - maybe in the next version?
        fprintf(OutputFile,"#               1                2              3               4              5\n");
        fprintf(OutputFile,"#               X                Y       Sigma(Y)               I       Sigma(I)\n");
        fprintf(OutputFile,"#  --------------- --------------- -------------- --------------- --------------\n");
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

    // integrate data
    for(int i=0; i < Options.GetArgNPoints(); i++) {

        int    px=0;
        double x,y,s;
        x = 0;
        y = 0;
        s = 0;

        // read whole line
        CSmallString data_line;
        if( data_line.ReadLineFromFile(InputFile) == false ) break; // no more data

        if( Options.GetOptNoSigma() == true ) {
            s = 0.0;
            int nr = sscanf(data_line,"%d %le",&px,&y);
            if( nr <= 0 ) break;    // no more data
            if( nr != 2 ) {
                fprintf(stderr,"%s: incosistent number of data records on line %d, requested: 2, found: %d\n",
                        (const char*)Options.GetProgramName(),i,nr);
                result = false;
                break;
            }
        } else {
            int nr = sscanf(data_line,"%d %le %le",&px,&y,&s);
            if( nr <= 0 ) break;    // no more data
            if( nr != 3 ) {
                fprintf(stderr,"%s: incosistent number of data records on line %d, requested: 3, found: %d\n",
                        (const char*)Options.GetProgramName(),i,nr);
                result = false;
                break;
            }
        }

        x = Abscissas[i];
        pi += y*Weights[i];
        psi += s*Weights[i];

        // print data --------------------------------------------------------------
        if( Options.GetOptShowProgress() == true ) {
            if( fprintf(OutputFile,OutputFormat,x,y,s,pi,psi) <= 0 ) {
                fprintf(stderr,"%s: unable to write to output file\n",
                        (const char*)Options.GetProgramName());
                result = false;
                break;
            }
        }

    }

    if( Options.GetOptShowProgress() == true ) {
        fprintf(OutputFile,"#\n");
    }

    if( Options.GetOptNoHeader() == false ) {
        // shell we update header sizes according to used formats? - maybe in the next version?
        fprintf(OutputFile,"#                1               2\n");
        fprintf(OutputFile,"#  Final         I Final  Sigma(I)\n");
        fprintf(OutputFile,"#  --------------- ---------------\n");
    }

    // complete output format -----------------------------------
    OutputFormat  = "   ";
    OutputFormat += Options.GetOptOIFormat();
    OutputFormat += " ";
    OutputFormat += Options.GetOptOEFormat();
    OutputFormat += "\n";

    if( fprintf(OutputFile,OutputFormat,pi,psi) <= 0 ) {
        fprintf(stderr,"%s: unable to write to output file\n",
                (const char*)Options.GetProgramName());
        result = false;
    }

    return(result);
}

//------------------------------------------------------------------------------

bool CGaussIntegrate::Finalize(void)
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

// from Numerical Recepies in C

void CGaussIntegrate::LegendrePolynom(void)
{
    int    n = Options.GetArgNPoints();
    double x1 = Options.GetOptStartValue();
    double x2 = Options.GetOptStopValue();
    int    m,j,i;
    double z1,z,xm,xl,pp,p3,p2,p1;
    double EPS = 3e-11;

    m=(n+1)/2;
    xm=0.5*(x2+x1);
    xl=0.5*(x2-x1);
    for(i=0; i < m; i++) {
        z=cos(M_PI*(i+0.75)/(n+0.5));
        do {
            p1=1.0;
            p2=0.0;
            for(j=1; j<=n; j++) {
                p3=p2;
                p2=p1;
                p1=((2.0*j-1.0)*z*p2-(j-1.0)*p3)/j;
            }
            pp=n*(z*p1-p2)/(z*z-1.0);
            z1=z;
            z=z1-p1/pp;
        } while(fabs(z-z1) > EPS);
        Abscissas[i]=xm-xl*z;
        Abscissas[n-i-1]=xm+xl*z;
        Weights[i]=2.0*xl/((1.0-z*z)*pp*pp);
        Weights[n-i-1]=Weights[i];
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGaussIntegrate::PrintProgHeader(FILE* fout)
{
    fprintf(stdout,"# ==============================================================================\n");
    fprintf(stdout,"# gauss-integrate (CATs utility)\n");
    fprintf(stdout,"# ==============================================================================\n");

    fprintf(stdout,"# Number of points : %d\n",Options.GetArgNPoints());
    if( Options.GetArgInput() == "-" ) {
        fprintf(stdout,"# Input file       : standard input stream (stdin)\n");
    } else {
        fprintf(stdout,"# Input file       : %s\n",(const char*)Options.GetArgInput());
    }
    if( Options.GetArgOutput() == "-" ) {
        fprintf(stdout,"# Output file      : standard output stream (stdout)\n");
    } else {
        fprintf(stdout,"# Output file      : %s\n",(const char*)Options.GetArgOutput());
    }
    fprintf(stdout,"# ------------------------------------------------------------------------------\n");
    fprintf(stdout,"# Start value      : %f\n",Options.GetOptStartValue());
    fprintf(stdout,"# Stop value       : %f\n",Options.GetOptStopValue());
    fprintf(stdout,"# Polynom type     : %s\n",(const char*)Options.GetOptPolynom());
    fprintf(stdout,"# ------------------------------------------------------------------------------\n");
    fprintf(stdout,"# Print progress   : %s\n",bool_to_str(Options.GetOptShowProgress()));
    fprintf(stdout,"# Print header     : %s\n",bool_to_str(!Options.GetOptNoHeader()));
    fprintf(stdout,"# Verbose          : %s\n",bool_to_str(Options.GetOptVerbose()));
    fprintf(stdout,"# ------------------------------------------------------------------------------\n");
    fprintf(stdout,"# X format         : %s\n",(const char*)Options.GetOptIXFormat());
    fprintf(stdout,"# Y format         : %s\n",(const char*)Options.GetOptIYFormat());
    fprintf(stdout,"# sigma(Y) format  : %s\n",(const char*)Options.GetOptISFormat());
    fprintf(stdout,"# I format         : %s\n",(const char*)Options.GetOptOIFormat());
    fprintf(stdout,"# sigma(I) format  : %s\n",(const char*)Options.GetOptOEFormat());
    fprintf(stdout,"# ------------------------------------------------------------------------------\n");
    fprintf(stdout,"#\n");
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================




