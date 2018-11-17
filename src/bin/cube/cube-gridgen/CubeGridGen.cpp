// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2018 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <SmallTimeAndDate.hpp>
#include "CubeGridGen.hpp"
#include <boost/format.hpp>
#include <boost/algorithm/string/join.hpp>
#include <vector>

//------------------------------------------------------------------------------

using namespace std;
using namespace boost;
using namespace boost::algorithm;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCubeGridGen::CCubeGridGen(void)
{
    TotalNumber = 0;
    ExcludedNumber = 0;
    NumberOfBatches = 0;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CCubeGridGen::Init(int argc,char* argv[])
{
    // encode program options
    int result = Options.ParseCmdLine(argc,argv);

    // should we exit or was it error?
    if( result != SO_CONTINUE ) return(result);

// attach verbose stream to terminal stream and set desired verbosity level
    vout.Attach(Console);
    if( Options.GetOptVerbose() ) {
        vout.Verbosity(CVerboseStr::debug);
    } else {
        vout.Verbosity(CVerboseStr::high);
    }

    // print header --------------------------------------------------------------
    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    vout << endl;
    vout << "# ==============================================================================" << endl;
    vout << "# cube-gridgen started at " << dt.GetSDateAndTime() << endl;
    vout << "# ==============================================================================" << endl;
    if( Options.GetArgGridPoints() == "-" ){
    vout <<         "# Grid points   : " << " - (standard output)" << endl;
    } else {
    vout <<         "# Grid points   : " << Options.GetArgGridPoints() << endl;
    }
    vout << "# ------------------------------------------------------------------------------" << endl;
    vout << format("# NX,NY,NZ      : %d,%d,%d") % Options.GetOptNX() % Options.GetOptNY()% Options.GetOptNZ() << endl;
    vout << format("# SX,SY,SZ      : %.2f,%.2f,%.2f") % Options.GetOptSX() % Options.GetOptSY()% Options.GetOptSZ() << endl;

    vout <<        "# Batch size    : " << Options.GetOptBatchSize() << endl;
    vout <<        "# Symmetry      : ";
    std::vector<std::string>    symm;
    if( Options.GetOptSymmetryXY() ) symm.push_back("XY");
    if( Options.GetOptSymmetryYZ() ) symm.push_back("YZ");
    if( Options.GetOptSymmetryXZ() ) symm.push_back("XZ");
    if( symm.size() == 0 ){
    vout << "none" << endl;
    } else {
        vout << join(symm,",") << endl;
    }
    vout << "# ------------------------------------------------------------------------------" << endl;
    if( Options.IsOptStructureSet() ){
    vout <<        "# Structure     : " << Options.GetOptStructure() <<  endl;
    vout << format("# Treshold      : %.2f") % Options.GetOptTreshold() << endl;
    vout << "# ------------------------------------------------------------------------------" << endl;
    }
    return( result );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CCubeGridGen::Run(void)
{
    // optionaly load the structure for grid point filtering
    if( Options.IsOptStructureSet() ) {
        if( Structure.Load(Options.GetOptStructure()) == false ) {
            vout << "<red>>>> ERROR: Unable to load the XYZ structure: " << Options.GetOptStructure() << "</red>" << endl;
            return(false);
        }
    }

    // generate grid points
    TotalNumber = Options.GetOptNX() * Options.GetOptNY()* Options.GetOptNZ();
    GridPoints.reserve(TotalNumber);

    double dx = (Options.GetOptNX()-1)*Options.GetOptSX();
    double dy = (Options.GetOptNY()-1)*Options.GetOptSY();
    double dz = (Options.GetOptNZ()-1)*Options.GetOptSZ();

    CPoint corner(dx,dy,dz);
    corner /= 2.0;
    CPoint pos;

    int nx,ny,nz;
    if( Options.GetOptSymmetryYZ() ){
        nx = Options.GetOptNX()/2 + Options.GetOptNX() % 2;
    } else {
        nx = Options.GetOptNX();
    }
    if( Options.GetOptSymmetryXZ() ){
        ny = Options.GetOptNY()/2 + Options.GetOptNY() % 2;
    } else {
        ny = Options.GetOptNY();
    }
    if( Options.GetOptSymmetryXY() ){
        nz = Options.GetOptNZ()/2 + Options.GetOptNZ() % 2;
    } else {
        nz = Options.GetOptNZ();
    }

    for(int i = 0; i < nx; i++ ){
        for(int j = 0; j < ny; j++ ){
            for(int k = 0; k < nz; k++ ){
                pos = corner - CPoint(i*Options.GetOptSX(),j*Options.GetOptSY(),k*Options.GetOptSZ());
                if( Options.IsOptStructureSet() ){
                    if( IsPointValid(pos) ) GridPoints.push_back(pos);
                } else {
                    GridPoints.push_back(pos);
                }
            }
        }
    }

    // save grid points
    if( SaveGridPoints() == false ){
        ES_TRACE_ERROR("unable to save grid points");
        return(false);
    }

    // print statistics
    vout << format("# Total number of points   : %d")%TotalNumber << endl;
    vout << format("# Excluded number of points: %d")%ExcludedNumber << endl;
    vout << format("# Final number of points   : %d")%GridPoints.size() << endl;
    vout << format("# Number of batches        : %d")%NumberOfBatches << endl;

    return(true);
}

//------------------------------------------------------------------------------

bool CCubeGridGen::IsPointValid(const CPoint& pos)
{
    double thr = Options.GetOptTreshold();
    for(int i=0; i < Structure.GetNumberOfAtoms(); i++){
        CPoint apos = Structure.GetPosition(i);
        if( Size(apos-pos) < thr ) return(false);
    }
    return(true);
}

//------------------------------------------------------------------------------

bool CCubeGridGen::SaveGridPoints(void)
{
    // open output file
    FILE* p_fout;
    if( Options.GetArgGridPoints() != "-" ) {
        if( (p_fout = fopen(Options.GetArgGridPoints(),"w")) == NULL ) {
            CSmallString error;
            error << "unable to open output file " << Options.GetArgGridPoints();
            error << " (" << strerror(errno) << ")";
            ES_ERROR(error);
            return(false);
        }
    } else {
        p_fout = stdout;
    }

    int remnpoints = GridPoints.size();
    NumberOfBatches = 0;
    for(size_t i=0; i < GridPoints.size(); i++){
        if( Options.GetOptBatchSize() == 0 ){
            if( i == 0 ){
                fprintf(p_fout,"%d\n",remnpoints);
                fprintf(p_fout,"single batch\n");
                NumberOfBatches++;
            }
        } else {
            if( (i % Options.GetOptBatchSize()) == 0 ) {
                if( remnpoints < Options.GetOptBatchSize() ){
                    fprintf(p_fout,"%d\n",remnpoints);
                } else {
                    fprintf(p_fout,"%d\n",Options.GetOptBatchSize());
                }
                NumberOfBatches++;
                fprintf(p_fout,"batch %d\n",NumberOfBatches);
            }
        }
        CPoint pos = GridPoints[i];
        fprintf(p_fout,"%2s %12.6f %12.6f %12.6f\n",(const char*)Options.GetOptSymbol(),pos.x,pos.y,pos.z);
        remnpoints--;
    }

    // close output file if necessary
    if( Options.GetArgGridPoints() != "-" ) {
        if( p_fout ) fclose(p_fout);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCubeGridGen::Finalize(void)
{
    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    vout << endl;
    vout << "# ==============================================================================" << endl;
    vout << "# cube-gridgen terminated at " << dt.GetSDateAndTime() << endl;
    vout << "# ==============================================================================" << endl;

    if( ErrorSystem.IsError() ){
        vout << high;
        ErrorSystem.PrintErrors(vout);
    }
    vout << endl;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

