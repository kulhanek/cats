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
#include "blur.hpp"
#include <ErrorSystem.hpp>

//------------------------------------------------------------------------------

#define GIDX(i,j,k) (Nz*(Ny*(i)+(j))+(k))
#define sqrt_2_PI 2.5066282746310002416123552393401041627

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CBlur::CBlur(void)
{
    InputFile = NULL;
    OwnInputFile = false;
    OutputFile = NULL;
    OwnOutputFile = false;

    Nx = 0;
    Ny = 0;
    Nz = 0;
    N = 0;

    startX = 0;
    stopX = 0;
    startY = 0;
    stopY = 0;
    startZ = 0;
    stopZ = 0;

    minX = 0;
    maxX = 0;
    minY = 0;
    maxY = 0;
    minZ = 0;
    maxZ = 0;
    rX = 0;
    rY = 0;
    rZ = 0;

    FirstArray = NULL;
    SecondArray = NULL;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CBlur::Init(int argc,char* argv[])
{
    if( (InputFile != NULL) || (OutputFile != NULL) ) return(SO_USER_ERROR);   // files already opened

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
            fprintf(stderr,"%s: unable to open input file '%s'\n",
                    (char*)Options.GetProgramName(),(const char*)Options.GetArgInput());
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
            fprintf(stderr,"%s: unable to open output file '%s'\n",
                    (char*)Options.GetProgramName(),(const char*)Options.GetArgOutput());
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

bool CBlur::Run(void)
{
    if( (InputFile == NULL) && (OutputFile == NULL) ) return(false);   // files are not opened

    // read input data
    if( ReadDataXPLOR(InputFile) == false ) {
        fprintf(stderr,"%s: unable to read input file '%s'\n",
                (char*)Options.GetProgramName(),(const char*)Options.GetArgInput());
        return(false);
    }


    fftw_plan       plan;
    int             F_PLAN;

    // perform forward FFT transformation ----------------------------------------
    F_PLAN=FFTW_FORWARD;
    plan = fftw_plan_dft_3d(Nx,Ny,Nz,FirstArray,SecondArray,F_PLAN,FFTW_ESTIMATE);
    fftw_execute(plan);

    // filter map ----------------------------------------------------------------
    if( Options.GetOptFilterType() == "low-pass" ) {
        LowPassFilter(SecondArray);
    } else if( Options.GetOptFilterType() == "gauss" ) {
        GaussFilter(SecondArray);
    } else {
        fprintf(stderr,"%s: not implemented filter '%s'\n",
                (char*)Options.GetProgramName(),(const char*)Options.GetOptFilterType());
        return(false);
    }

    // perform reverse FFT transformation ----------------------------------------
    F_PLAN=FFTW_BACKWARD;
    plan = fftw_plan_dft_3d(Nx,Ny,Nz,SecondArray,FirstArray,F_PLAN,FFTW_ESTIMATE);
    fftw_execute (plan);

    // data has to be normalized - only real part is considered
    for(int i=0; i<Nx; i++) {
        for(int j=0; j < Ny; j++) {
            for(int k=0; k<Nz; k++) {
                FirstArray[GIDX(i,j,k)][0] /= N;
            }
        }
    }

    // write data
    if( WriteDataXPLOR(OutputFile) == false ) {
        fprintf(stderr,"%s: unable to write blured data into file '%s'\n",
                (char*)Options.GetProgramName(),(const char*)Options.GetArgOutput());
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CBlur::Finalize(void)
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

    if( FirstArray != NULL ) fftw_free(FirstArray);
    if( SecondArray != NULL ) fftw_free(SecondArray);

    FirstArray = NULL;
    SecondArray = NULL;

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CBlur::PrintProgHeader(FILE* fout)
{
    fprintf(stdout,"# ==============================================================================\n");
    fprintf(stdout,"# blur (CATs utility)\n");
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
    fprintf(stdout,"# Filter type     : %s\n",(const char*)Options.GetOptFilterType());
    fprintf(stdout,"# Treshold value  : %f\n",Options.GetArgFilterValue());
    fprintf(stdout,"# ------------------------------------------------------------------------------\n");
    fprintf(stdout,"# Verbose         : %s\n",bool_to_str(Options.GetOptVerbose()));
    fprintf(stdout,"# ------------------------------------------------------------------------------\n");
    fprintf(stdout,"#\n");
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CBlur::ReadDataXPLOR(FILE *fin)
{
    if( fin == NULL ) {
        ES_ERROR("fin is NULL");
        return(false);
    }

    char buf[1000];

    // skip 3 lines
    char* cret;
    cret = fgets(buf, sizeof(buf), fin);
    cret = fgets(buf, sizeof(buf), fin);
    cret = fgets(buf, sizeof(buf), fin);
    if( cret == NULL ) return(false);

    // read header
    cret = fgets(buf, sizeof(buf), fin);
    if( cret == NULL ) return(false);
    if( sscanf(buf, "%d%d%d%d%d%d%d%d%d", &Nx, &startX, &stopX, &Ny, &startY, &stopY, &Nz, &startZ, &stopZ) != 9 ) {
        ES_ERROR("incorrect number of items in header of input xplor file or stream");
        return(false);
    }

    cret = fgets(buf, sizeof(buf), fin);
    if( cret == NULL ) return(false);
    if( sscanf(buf, "%lf%lf%lf%lf%lf%lf",&minX, &minY, &minZ, &maxX, &maxY, &maxZ) != 6 ) {
        ES_ERROR("incorrect number of items in header of input xplor file or stream");
        return(false);
    }

    // init data array
    N=Nx*Ny*Nz;

    // allocate arrays
    FirstArray = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
    SecondArray = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);

    if( (FirstArray == NULL) || (SecondArray == NULL) ) {
        if( FirstArray != NULL ) fftw_free(FirstArray);
        if( SecondArray != NULL ) fftw_free(SecondArray);
        ES_ERROR("unable to allocate memory for 3D-density map arrays");
        return(false);
    }

    // skip line
    cret = fgets(buf, sizeof(buf), fin);
    if( cret == NULL ) return(false);

    for(int k=0; k<Nz; k++) {
        int idxZ;
        if( fscanf(fin,"%d ",&idxZ) != 1 ) {
            ES_ERROR("unable to read idxZ item from input xplor file or stream");
            if( FirstArray != NULL ) fftw_free(FirstArray);
            if( SecondArray != NULL ) fftw_free(SecondArray);
            FirstArray = NULL;
            SecondArray = NULL;
            return(false);
        }
        for(int j=0; j<Ny; j++) {
            for(int i=0; i<Nx; i++) {
                if( fscanf(fin,"%lf ",&FirstArray[GIDX(i,j,k)][0]) != 1 ) {
                    ES_ERROR("Unable to read data item from input xplor file or stream");
                    if( FirstArray != NULL ) fftw_free(FirstArray);
                    if( SecondArray != NULL ) fftw_free(SecondArray);
                    return(false);
                }
                FirstArray[GIDX(i,j,k)][1]=0.0;
            }
        }
    }

    return(true);
}

//---------------------------------------------------------------------------

bool CBlur::WriteDataXPLOR(FILE *fout)
{
    if( fout == NULL ) {
        ES_ERROR("fout is NULL");
        return(false);
    }

    fprintf(fout,"First line\n%8d\nData modified by Fourier filter\n",1);
    fprintf(fout,"%8d%8d%8d%8d%8d%8d%8d%8d%8d\n",Nx,startX,stopX,Ny,startY,stopY,Nz,startZ,stopZ);
    fprintf(fout,"%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f\n",minX,minY,minZ,maxX,maxY,maxZ);
    fprintf(fout,"ZYX\n");

    for(int k=0; k<Nz; k++) {
        fprintf(fout,"%8d\n",startZ-k);
        int iColumnInd = 0;

        for(int j=0; j<Ny; j++) {
            for(int i=0; i<Nx; i++) {
                fprintf(fout,"%12.5f",FirstArray[GIDX(i,j,k)][0]);
                iColumnInd++;
                if(iColumnInd==6) {
                    iColumnInd=0;
                    fprintf(fout,"\n");
                }
            }
        }
        if(iColumnInd!=0) {
            iColumnInd=0;
            fprintf(fout,"\n");
        }
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CBlur::LowPassFilter(fftw_complex* in)
{
    if( Options.GetOptVerbose() ) {
        printf(" Low-pass filter with treshold at : %f\n",Options.GetArgFilterValue());
    }

    for(int i=0; i<Nx; i++) {
        for(int j=0; j<Ny; j++) {
            for(int k=0; k<Nz; k++) {
                // i,j,k represent coordinates in virtual box,
                // in which zero frequency point is situated in the box center
                // we use this box to find such grid point that pass filter rules

                double ix,iy,iz,r;
                ix=i-Nx/2.0;
                iy=j-Ny/2.0;
                iz=k-Nz/2.0;
                r=sqrt(ix*ix+iy*iy+iz*iz);

                if( r > Options.GetArgFilterValue() ) {
                    int mi,mj,mk;
                    // mi,mj,mk represent coordinates in original box,
                    // in which zero frequency point is spread over box corners

                    // move by 1/2 of box
                    mi = i - Nx/2;
                    mj = j - Ny/2;
                    mk = k - Nz/2;
                    // image back to box
                    if( mi < 0 ) mi += Nx;
                    if( mj < 0 ) mj += Ny;
                    if( mk < 0 ) mk += Nz;
                    in[GIDX(mi,mj,mk)][0] = 0.0;
                    in[GIDX(mi,mj,mk)][1] = 0.0;
                }
            }
        }
    }

    return(true);
}

//---------------------------------------------------------------------------

double CBlur::Gauss(double dx, double o)
{
    return( 1.0 / (o * sqrt_2_PI) * exp (-dx * dx / (2 * o * o)));
}

//---------------------------------------------------------------------------

bool CBlur::GaussFilter(fftw_complex* in)
{
    if( Options.GetOptVerbose() ) {
        printf(" Gauss filter with sigma : %f\n",Options.GetArgFilterValue());
    }

    for(int i=0; i<Nx; i++) {
        for(int j=0; j<Ny; j++) {
            for(int k=0; k<Nz; k++) {
                // i,j,k represent coordinates in virtual box,
                // in which zero frequency point is situated in the box center
                // we use this box to calculate distances between grid points and box center
                // such distances are then used to calculate filter multiplicator

                double ix,iy,iz,r;
                ix=i-Nx/2.0;
                iy=j-Ny/2.0;
                iz=k-Nz/2.0;
                r=sqrt(ix*ix+iy*iy+iz*iz);

                double mult = Gauss(r,Options.GetArgFilterValue());

                int mi,mj,mk;
                // mi,mj,mk represent coordinates in original box,
                // in which zero frequency point is spread over box corners

                // move by 1/2 of box
                mi = i - Nx/2;
                mj = j - Ny/2;
                mk = k - Nz/2;
                // image back to box
                if( mi < 0 ) mi += Nx;
                if( mj < 0 ) mj += Ny;
                if( mk < 0 ) mk += Nz;
                in[GIDX(mi,mj,mk)][0] *= mult;
                in[GIDX(mi,mj,mk)][1] *= mult;

            }
        }
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================





// /******************************************************************************
//  * Copyright 2005    Martin Petrek, petrek@chemi.muni.cz                     *
//  *                   Petr Kulhanek, kulhanek@chemi.muni.cz                   *
//  *                                                                           *
//  *  This file is part of CATs collection programs.                           *
//  *                                                                           *
//  *  This is free software; you can redistribute it and/or modify             *
//  *  it under the terms of the GNU General Public License as published by     *
//  *  the Free Software Foundation; either version 2 of the License, or        *
//  *  (at your option) any later version.                                      *
//  *                                                                           *
//  *  This is distributed in the hope that it will be useful,                  *
//  *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
//  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
//  *  GNU General Public License for more details.                             *
//  *                                                                           *
//  *  You should have received a copy of the GNU General Public License        *
//  *  along with this software; if not, write to                               *
//  *  the Free Software Foundation, Inc.,                                      *
//  *  59 Temple Place, Suite 330, Boston, MA  02111-1307  USA                  *
// ******************************************************************************/
//
// #include "CopyrightCATs.h"
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <math.h>
// #include <fftw3.h>
//
// // ----------------------------------------------------------------------------
//
//
// #define double double

//
// // command line options
//
// bool             SilentMode = false;
// bool             UseLowPassFilter = true;
// double            FilterParam = 0.0;
//
// int          Nx = 0;
// int          Ny = 0;
// int          Nz = 0;
// int          N = 0;
//
// int startX,stopX,startY,stopY,startZ,stopZ;
// double minX,maxX,minY,maxY,minZ,maxZ,rX,rY,rZ;
//
// fftw_complex*    FirstArray = NULL;
// fftw_complex*    SecondArray = NULL;
//
// // ----------------------------------------------------------------------------
//
// void PrintCopyright(bool include_usage);
// bool ProcessOptions(int argc, char* argv[]);
//
// bool ReadDataXPLOR(char *name);
// bool WriteDataXPLOR(char *name);
//
// bool ProceedFourier(void);
// bool LowPassFilter(fftw_complex* in);
// bool GaussFilter(fftw_complex* in);
//
// // ----------------------------------------------------------------------------
//
// int main (int argc, char ** argv)
// {
//  // process command options and print program copyright -----------------------
//
//  if( ProcessOptions(argc,argv) == false ) return(1);
//  PrintCopyright(false);
//
//  // decode filter param
//
//  FilterParam = atof(argv[argc-2]);
//
//  // read input data -----------------------------------------------------------
//  if( SilentMode == false ) printf("Loading input 3D-density map, please wait ...\n");
//  if( ReadDataXPLOR(argv[argc-3]) == false ){
//      return(1);
//  }
//  if( SilentMode == false ) printf("Input 3D-density map was successfully loaded !\n\n");
//
//  if( SilentMode == false ) {
//      printf(" 3D-density map info:\n");
//  printf("======================\n");
//  printf(" Nx : %d\n",Nx);
//     printf(" Ny : %d\n",Ny);
//  printf(" Nz : %d\n",Nz);
//  printf("\n");
//  }
//
//  if( SilentMode == false ) printf("Performing FFT filtering, please wait ...\n");
//  if( ProceedFourier() == false ){
//      if( FirstArray != NULL ) fftw_free(FirstArray);
//  if( SecondArray != NULL ) fftw_free(SecondArray);
//  return(1);
//  }
//  if( SilentMode == false ) printf("FFT filtering was successfully performed !\n\n");
//
//  if( SilentMode == false ) printf("Saving processed 3D-density map, please wait ...\n");
//  if( WriteDataXPLOR(argv[argc-1]) == false ){
//      if( FirstArray != NULL ) fftw_free(FirstArray);
//  if( SecondArray != NULL ) fftw_free(SecondArray);
//      return(1);
//  }
//  if( SilentMode == false ) printf("Processed 3D-density map was successfully saved !\n\n");
//
//  if( FirstArray != NULL ) fftw_free(FirstArray);
//  if( SecondArray != NULL ) fftw_free(SecondArray);
//
//  return 0;
// }
//
// //---------------------------------------------------------------------------
//
// void PrintCopyright(bool include_usage)
// {
//  if( SilentMode == true ) return;
//
//  printf("\n");
//  printf("blur\n");
//  printf("   - blur 3D-density map\n\n");
//  printf(TopHeaderCATs);
//  printf(VersionCATs);
//  printf(BottomHeaderCATs);
//  printf("\n");
//
//  if( include_usage == true ){
//     printf("Usage: blur [-h -s -gauss] <input> <filter_param> <output>\n");
//     printf("\n");
//  printf("Options:\n");
//     printf(" -h     : print help\n");
//     printf(" -s     : silent mode - skip informative messages\n");
//  printf(" -gauss : use gauss filter instead of low-pass one\n");
//  printf("\n");
//  printf("Parameters:\n");
//     printf("<input>        : input 3D-density map (x-plor format)\n");
//  printf("<filter_param> : filter param\n");
//     printf("<output>       : output 3D-density map (x-plor format)\n");
//     printf("\n");
//     }
// }
//
// //---------------------------------------------------------------------------
//
// bool ProcessOptions(int argc, char* argv[])
// {
//  int i=1;
//  for(i=1;i<argc;i++){
//     if( strcmp(argv[i],"-h") == 0 ){
//      SilentMode = false;
//         PrintCopyright(true);
//         return(false);
//         }
//     if( strcmp(argv[i],"-s") == 0 ){
//         SilentMode = true;
//         continue;
//         }
//     if( strcmp(argv[i],"-gauss") == 0 ){
//         UseLowPassFilter = false;
//         continue;
//         }
//
//     if( argc - i == 3 ) break;
//
//     fprintf(stderr,"\n");
//     fprintf(stderr," ERROR: Illegal option : %s\n",argv[i]);
//  fprintf(stderr,"\n");
//     return(false);
//     }
//
//  if( argc - i == 3 ) return(true);
//
//  fprintf(stderr,"\n");
//  fprintf(stderr," ERROR: Incorrect number of parameters is specified !\n");
//  fprintf(stderr,"\n");
//
//  return(false);
// }
//
// //---------------------------------------------------------------------------
//
// bool ReadDataXPLOR(char *name)
// {
//  FILE *fin;
//
//  fin=fopen(name,"r");
//
//  if( fin == NULL ){
//     fprintf(stderr,"\n");
//     fprintf(stderr," ERROR: Unable to open input 3D-density map file '%s' !\n",name);
//  fprintf(stderr,"\n");
//  return(false);
//  }
//
//  int n = 0;
//  char buf[1000];
//
//  // skip 3 lines
//  fgets(buf, sizeof(buf), fin);
//  fgets(buf, sizeof(buf), fin);
//  fgets(buf, sizeof(buf), fin);
//
//  // read header
//  fgets(buf, sizeof(buf), fin);
//  if( sscanf(buf, "%d%d%d%d%d%d%d%d%d", &Nx, &startX, &stopX, &Ny, &startY, &stopY, &Nz, &startZ, &stopZ) != 9 ){
//     fprintf(stderr,"\n");
//     fprintf(stderr," ERROR: Incorrect number of items in header of input xplor file or stream !\n");
//  fprintf(stderr,"\n");
//  return(false);
//  }
//
//  fgets(buf, sizeof(buf), fin);
//  if( sscanf(buf, "%lf%lf%lf%lf%lf%lf",&minX, &minY, &minZ, &maxX, &maxY, &maxZ) != 6 ){
//     fprintf(stderr,"\n");
//     fprintf(stderr," ERROR: Incorrect number of items in header of input xplor file or stream !\n");
//  fprintf(stderr,"\n");
//  return(false);
//  }
//
//  // init data array
//  N=Nx*Ny*Nz;
//
//  // allocate arrays
//  FirstArray = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
//  SecondArray = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
//
//  if( (FirstArray == NULL) || (SecondArray == NULL) ){
//      if( FirstArray != NULL ) fftw_free(FirstArray);
//  if( SecondArray != NULL ) fftw_free(SecondArray);
//  fprintf(stderr,"\n");
//  fprintf(stderr," ERROR: Unable to allocate memory for 3D-density map arrays.\n");
//  fprintf(stderr,"\n");
//  return(false);
//  }
//
//  // skip line
//  fgets(buf, sizeof(buf), fin);
//
//  for(int k=0;k<Nz;k++){
//      int idxZ;
//  if( fscanf(fin,"%d ",&idxZ) != 1 ){
//      fprintf(stderr,"\n");
//      fprintf(stderr," ERROR: Unable to read idxZ item from input xplor file or stream !\n");
//      fprintf(stderr,"\n");
//          if( FirstArray != NULL ) fftw_free(FirstArray);
//      if( SecondArray != NULL ) fftw_free(SecondArray);
//      FirstArray = NULL;
//      SecondArray = NULL;
//      return(false);
//      }
//  for(int j=0;j<Ny;j++){
//      for(int i=0;i<Nx;i++){
//          if( fscanf(fin,"%lf ",&FirstArray[GIDX(i,j,k)][0]) != 1 ){
//              fprintf(stderr,"\n");
//              fprintf(stderr," ERROR: Unable to read data item from input xplor file or stream !\n");
//              fprintf(stderr,"\n");
//              if( FirstArray != NULL ) fftw_free(FirstArray);
//              if( SecondArray != NULL ) fftw_free(SecondArray);
//              return(false);
//              }
//          FirstArray[GIDX(i,j,k)][1]=0.0;
//          }
//      }
//  }
//
//  // close input file
//  fclose(fin);
//
//  return(true);
// }
//
// //---------------------------------------------------------------------------
//
// bool WriteDataXPLOR(char *name)
// {
//  FILE *fout=fopen(name,"w");
//
//  if( fout == NULL ){
//     fprintf(stderr,"\n");
//     printf("ERROR: Unable to open output 3D-density map file '%s' !\n",name);
//  fprintf(stderr,"\n");
//  return(false);
//  }
//
//  fprintf(fout,"First line\n%8d\nData modified by fourier filter\n",1);
//  fprintf(fout,"%8d%8d%8d%8d%8d%8d%8d%8d%8d\n",Nx,startX,stopX,Ny,startY,stopY,Nz,startZ,stopZ);
//  fprintf(fout,"%12.3lf%12.3lf%12.3lf%12.3lf%12.3lf%12.3lf\n",minX,minY,minZ,maxX,maxY,maxZ);
//  fprintf(fout,"ZYX\n");
//
//  for(int k=0; k<Nz; k++){
//  fprintf(fout,"%8d\n",startZ-k);
//  int iColumnInd;
//  iColumnInd=0;
//
//      for(int j=0; j<Ny; j++){
//          for(int i=0;i<Nx;i++){
//              fprintf(fout,"%12.5lf",FirstArray[GIDX(i,j,k)][0]);
//          iColumnInd++;
//          if(iColumnInd==6){
//              iColumnInd=0;
//              fprintf(fout,"\n");
//              }
//          }
//          }
//  if(iColumnInd!=0){
//      iColumnInd=0;
//      fprintf(fout,"\n");
//      }
//      }
//
//  fclose(fout);
//
//  return(true);
// }
//
// // ----------------------------------------------------------------------------
//
// bool ProceedFourier(void)
// {
//  fftw_plan       plan;
//  int             F_PLAN;
//
//  // perform forward FFT transformation ----------------------------------------
//
//  F_PLAN=FFTW_FORWARD;
//  plan = fftw_plan_dft_3d(Nx,Ny,Nz,FirstArray,SecondArray,F_PLAN,FFTW_ESTIMATE);
//  fftw_execute(plan);
//
//  // filter map ----------------------------------------------------------------
//
//  if( UseLowPassFilter == true ){
//      LowPassFilter(SecondArray);
//  }
//   else{
//     GaussFilter(SecondArray);
//      }
//
//  // perform reverse FFT transformation ----------------------------------------
//
//  F_PLAN=FFTW_BACKWARD;
//  plan = fftw_plan_dft_3d(Nx,Ny,Nz,SecondArray,FirstArray,F_PLAN,FFTW_ESTIMATE);
//  fftw_execute (plan);
//
//  // data has to be normalized - only real part is considered
//  for(int i=0; i<Nx; i++){
//      for(int j=0;j < Ny; j++){
//          for(int k=0; k<Nz; k++){
//          FirstArray[GIDX(i,j,k)][0] /= N;
//          }
//      }
//  }
//
//  return(true);
// }
//
// //---------------------------------------------------------------------------
//
// bool LowPassFilter(fftw_complex* in)
// {
//  if( SilentMode == false ){
//  printf(" Low-pass filter with treshold at : %lf\n",FilterParam);
//  }
//
//  for(int i=0; i<Nx; i++){
//  for(int j=0; j<Ny; j++){
//          for(int k=0; k<Nz; k++){
//          // i,j,k represent coordinates in virtual box,
//          // in which zero frequency point is situated in the box center
//          // we use this box to find such grid point that pass filter rules
//
//          double ix,iy,iz,r;
//          ix=i-Nx/2.0;
//          iy=j-Ny/2.0;
//          iz=k-Nz/2.0;
//          r=sqrt(ix*ix+iy*iy+iz*iz);
//
//          if( r > FilterParam ){
//              int mi,mj,mk;
//              // mi,mj,mk represent coordinates in original box,
//              // in which zero frequency point is spread over box corners
//
//              // move by 1/2 of box
//              mi = i - Nx/2;
//              mj = j - Ny/2;
//              mk = k - Nz/2;
//              // image back to box
//              if( mi < 0 ) mi += Nx;
//              if( mj < 0 ) mj += Ny;
//              if( mk < 0 ) mk += Nz;
//              in[GIDX(mi,mj,mk)][0] = 0.0;
//              in[GIDX(mi,mj,mk)][1] = 0.0;
//              }
//          }
//      }
//  }
//
//  return(true);
// }
//
// //---------------------------------------------------------------------------
//
// double Gauss(double dx, double o)
// {
//  return( 1.0 / (o * sqrt_2_PI) * exp (-dx * dx / (2 * o * o)));
// }
//
// //---------------------------------------------------------------------------
//
// bool GaussFilter(fftw_complex* in)
// {
//  if( SilentMode == false ){
//  printf(" Gauss filter with sigma : %lf\n",FilterParam);
//  }
//
//  for(int i=0; i<Nx; i++){
//  for(int j=0; j<Ny; j++){
//          for(int k=0; k<Nz; k++){
//          // i,j,k represent coordinates in virtual box,
//          // in which zero frequency point is situated in the box center
//          // we use this box to calculate distances between grid points and box center
//          // such distances are then used to calculate filter multiplicator
//
//          double ix,iy,iz,r;
//          ix=i-Nx/2.0;
//          iy=j-Ny/2.0;
//          iz=k-Nz/2.0;
//          r=sqrt(ix*ix+iy*iy+iz*iz);
//
//          double mult = Gauss(r,FilterParam);
//
//          int mi,mj,mk;
//          // mi,mj,mk represent coordinates in original box,
//          // in which zero frequency point is spread over box corners
//
//          // move by 1/2 of box
//          mi = i - Nx/2;
//          mj = j - Ny/2;
//          mk = k - Nz/2;
//          // image back to box
//          if( mi < 0 ) mi += Nx;
//          if( mj < 0 ) mj += Ny;
//          if( mk < 0 ) mk += Nz;
//          in[GIDX(mi,mj,mk)][0] *= mult;
//          in[GIDX(mi,mj,mk)][1] *= mult;
//
//          }
//      }
//  }
//
//  return(true);
// }
//
// //---------------------------------------------------------------------------










