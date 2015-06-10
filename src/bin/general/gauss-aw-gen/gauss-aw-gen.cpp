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
#include "gauss-aw-gen.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGaussAWGen::CGaussAWGen(void)
{
    OutputFile = NULL;
    OwnOutputFile = false;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CGaussAWGen::Init(int argc,char* argv[])
{
    // encode program options, all check procedures are done inside of CABFIntOpts
    int result = Options.ParseCmdLine(argc,argv);

    // should we exit or was it error?
    if( result != SO_CONTINUE ) return(result);

    // print header if requested --------------------
    if( Options.GetOptVerbose() == true ) PrintProgHeader();

    // open files -----------------------------------
    if( Options.GetArgOutput() == "-" ) {
        OutputFile = stdout;
        OwnOutputFile = false;
    } else {
        OutputFile = fopen(Options.GetArgOutput(),"w");
        if( OutputFile == NULL ) {
            fprintf(stderr,"%s: unable to open output file '%s'\n",
                    (const char*)Options.GetProgramName(),(const char*)Options.GetArgOutput());
            return(SO_USER_ERROR);
        }
        OwnOutputFile = true;
    }

    // complete output format -----------------------------------
    if( Options.GetOptOutput() == "both" ) {
        OutputFormat  = "   ";
        OutputFormat += Options.GetOptOAFormat();
        OutputFormat += " ";
        OutputFormat += Options.GetOptOWFormat();
        OutputFormat += "\n";
    }
    if( Options.GetOptOutput() == "abscissas" ) {
        OutputFormat  = "   ";
        OutputFormat += Options.GetOptOAFormat();
        OutputFormat += "\n";
    }
    if( Options.GetOptOutput() == "weights" ) {
        OutputFormat  = "   ";
        OutputFormat += Options.GetOptOWFormat();
        OutputFormat += "\n";
    }

    return(result);
}

//------------------------------------------------------------------------------

bool CGaussAWGen::Run(void)
{
    if( OutputFile == NULL ) return(false);   // files are not opened

    if( Options.GetOptNoHeader() == false ) {
        // shell we update header sizes according to used formats? - maybe in the next version?
        if( Options.GetOptOutput() == "both" ) {
            fprintf(OutputFile,"#               1                2\n");
            fprintf(OutputFile,"#        abscissas         weigths\n");
            fprintf(OutputFile,"#  --------------- ---------------\n");
        }
        if( Options.GetOptOutput() == "abscissas" ) {
            fprintf(OutputFile,"#               1 \n");
            fprintf(OutputFile,"#        abscissas\n");
            fprintf(OutputFile,"#  ---------------\n");
        }
        if( Options.GetOptOutput() == "weights" ) {
            fprintf(OutputFile,"#                1\n");
            fprintf(OutputFile,"#          weigths\n");
            fprintf(OutputFile,"#  ---------------\n");
        }
    }

    Abscissas.CreateVector(Options.GetArgNPoints());
    Weights.CreateVector(Options.GetArgNPoints());

    // generate abscissas and weights
    if( Options.GetOptPolynom() == "legendre" ) {
        LegendrePolynom();
    }

    // print them
    if( Options.GetOptReverse() == true ) {
        for(int i=Options.GetArgNPoints()-1; i >=0 ; i-- ) {
            if( Options.GetOptOutput() == "both" ) {
                fprintf(OutputFile,OutputFormat,Abscissas[i],Weights[i]);
            }
            if( Options.GetOptOutput() == "abscissas" ) {
                fprintf(OutputFile,OutputFormat,Abscissas[i]);
            }
            if( Options.GetOptOutput() == "weights" ) {
                fprintf(OutputFile,OutputFormat,Weights[i]);
            }
        }
    } else {
        for(int i=0; i < Options.GetArgNPoints(); i++ ) {
            if( Options.GetOptOutput() == "both" ) {
                fprintf(OutputFile,OutputFormat,Abscissas[i],Weights[i]);
            }
            if( Options.GetOptOutput() == "abscissas" ) {
                fprintf(OutputFile,OutputFormat,Abscissas[i]);
            }
            if( Options.GetOptOutput() == "weights" ) {
                fprintf(OutputFile,OutputFormat,Weights[i]);
            }
        }
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CGaussAWGen::Finalize(void)
{
    // close files if they are own by program
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

void CGaussAWGen::LegendrePolynom(void)
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

void CGaussAWGen::PrintProgHeader(FILE* fout)
{
    fprintf(stdout,"# ==============================================================================\n");
    fprintf(stdout,"# gauss-aw-gen (CATs utility)\n");
    fprintf(stdout,"# ==============================================================================\n");

    fprintf(stdout,"# Number of points : %d\n",Options.GetArgNPoints());
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
    fprintf(stdout,"# Print header     : %s\n",bool_to_str(!Options.GetOptNoHeader()));
    fprintf(stdout,"# Print reversly   : %s\n",bool_to_str(Options.GetOptReverse()));
    fprintf(stdout,"# Verbose          : %s\n",bool_to_str(Options.GetOptVerbose()));
    fprintf(stdout,"# ------------------------------------------------------------------------------\n");
    fprintf(stdout,"# Output format    : %s\n",(const char*)Options.GetOptOutput());
    fprintf(stdout,"# Abscissas format : %s\n",(const char*)Options.GetOptOAFormat());
    fprintf(stdout,"# Weights format   : %s\n",(const char*)Options.GetOptOWFormat());
    fprintf(stdout,"# ------------------------------------------------------------------------------\n");
    fprintf(stdout,"#\n");
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================




