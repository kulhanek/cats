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
#include "CubeAssembly.hpp"
#include <boost/format.hpp>
#include <boost/algorithm/string/join.hpp>
#include <vector>
#include <PeriodicTable.hpp>

//------------------------------------------------------------------------------

using namespace std;
using namespace boost;
using namespace boost::algorithm;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCubeAssembly::CCubeAssembly(void)
{
    TotalPoints = 0;
    ReadPoints = 0;
    StoredPoints = 0;
    IllegalPoints = 0;
    Properties = NULL;
}

//------------------------------------------------------------------------------

CCubeAssembly::~CCubeAssembly(void)
{
    if( Properties ){
        delete[] Properties;
        Properties = NULL;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CCubeAssembly::Init(int argc,char* argv[])
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
    vout << "# cube-assembly started at " << dt.GetSDateAndTime() << endl;
    vout << "# ==============================================================================" << endl;
    if( Options.GetArgProperties() == "-" ){
    vout <<         "# Property file : " << " - (standard input)" << endl;
    } else {
    vout <<         "# Property file : " << Options.GetArgProperties() << endl;
    }
    if( Options.GetArgCubeFile() == "-" ){
    vout <<         "# Cube file     : " << " - (standard output)" << endl;
    } else {
    vout <<         "# Cube file     : " << Options.GetArgCubeFile() << endl;
    }
    vout << "# ------------------------------------------------------------------------------" << endl;
    vout << format("# NX,NY,NZ      : %d,%d,%d") % Options.GetOptNX() % Options.GetOptNY()% Options.GetOptNZ() << endl;
    vout << format("# SX,SY,SZ      : %.2f,%.2f,%.2f") % Options.GetOptSX() % Options.GetOptSY()% Options.GetOptSZ() << endl;

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

    return( result );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CCubeAssembly::Run(void)
{
    if( Structure.Load(Options.GetArgStructure()) == false ) {
        vout << "<red>>>> ERROR: Unable to load the XYZ structure: " << Options.GetArgStructure() << "</red>" << endl;
        return(false);
    }

    // initialize
    TotalPoints = Options.GetOptNX() * Options.GetOptNY()* Options.GetOptNZ();
    ReadPoints = 0;
    StoredPoints = 0;
    IllegalPoints = 0;

    // we can apply for floor(x)
    PosX = -(Options.GetOptNX()/2)*Options.GetOptSX();
    PosY = -(Options.GetOptNY()/2)*Options.GetOptSY();
    PosZ = -(Options.GetOptNZ()/2)*Options.GetOptSZ();

    // allocate array for data
    Properties = new double[TotalPoints];

    // process input file
    if( Options.GetArgProperties() != "-" ) {
        ifstream fin(Options.GetArgProperties());
        if( !fin ) {
            CSmallString error;
            error << "unable to open input file " << Options.GetArgProperties();
            error << " (" << strerror(errno) << ")";
            ES_ERROR(error);
            return(false);
        }

        if( ReadProperties(fin) == false ){
            ES_ERROR("unable to read property file");
            return(false);
        }
    } else {
        if( ReadProperties(cin) == false ){
            ES_ERROR("unable to read property file from standard input");
            return(false);
        }
    }

    // save cube file
    if( SaveCubeFile() == false ){
        ES_TRACE_ERROR("unable to save cube file");
        return(false);
    }

    // print statistics
    vout << format("# Total number of points   : %d")%TotalPoints << endl;
    vout << format("# Number of read points    : %d")%ReadPoints << endl;
    vout << format("# Number of stored points  : %d")%StoredPoints << endl;
    vout << format("# Number of illegal points : %d")%IllegalPoints << endl;

    return(true);
}

//------------------------------------------------------------------------------

bool CCubeAssembly::ReadProperties(istream& fin)
{
    std::string line;

    int lineno = 0;
    while( fin ){
        // read number of lines
        int npoints = 0;
        getline(fin,line);
        if( ! fin ){
            return(true);   // end of stream
        }

        stringstream str(line);
        str >> npoints;
        lineno++;
        if( npoints == 0 ){
            return(true);   // end of stream
        }

        // skip comment
        getline(fin,line);
        lineno++;

        for(int i=0; i < npoints; i++){
            getline(fin,line);
            lineno++;
            if( ! fin ){
                CSmallString error;
                error << "unable to read data line " << lineno;
                ES_TRACE_ERROR(error);
                return(false);
            }

            std::string symbol;
            double x,y,z,p;
            stringstream str(line);
            str >> symbol >> x >> y >> z >> p;
            if( ! str ){
                CSmallString error;
                error << "five records expected on line " << lineno;
                ES_TRACE_ERROR(error);
                return(false);
            }
            ReadPoints++;
            RegisterPoint(x,y,z,p);
        }
    }

    return(true);   // end of stream
}

//------------------------------------------------------------------------------

void CCubeAssembly::RegisterPoint(double x, double y, double z, double p)
{

    // calculate index
    int ix = floor( (x - PosX) / Options.GetOptSX() );
    // cout << (x - PosX)  << " " << x << " " << ix <<endl;
    if( (ix < 0) || (ix >= Options.GetOptNX() ) ){
        IllegalPoints++;
        return;
    }

    int iy = floor( (y - PosY) / Options.GetOptSY() );
    // cout << y << " " << iy <<endl;
    if( (iy < 0) || (iy >= Options.GetOptNY() ) ){
        IllegalPoints++;
        return;
    }
    int iz = floor( (z - PosZ) / Options.GetOptSZ() );
    // cout << z << " " << iz <<endl;
    if( (iz < 0) || (iz >= Options.GetOptNZ() ) ){
        IllegalPoints++;
        return;
    }
    // calculate global index and store property
    int glid = ix + iy*Options.GetOptNX() + iz*Options.GetOptNX()*Options.GetOptNY();
    Properties[glid] = p;
    StoredPoints++;

    // symmetry: x, y, z
    if( Options.GetOptSymmetryXY() ){
        int siz = Options.GetOptNZ() - iz - 1;
        if( siz != iz ){
            int glid = ix + iy*Options.GetOptNX() + siz*Options.GetOptNX()*Options.GetOptNY();
            Properties[glid] = p;
            StoredPoints++;
        }
    }
    if( Options.GetOptSymmetryYZ() ){
        int six = Options.GetOptNX() - ix - 1;
        if( six != ix ){
            int glid = six + iy*Options.GetOptNX() + iz*Options.GetOptNX()*Options.GetOptNY();
            Properties[glid] = p;
            StoredPoints++;
        }
    }
    if( Options.GetOptSymmetryXZ() ){
        int siy = Options.GetOptNY() - iy - 1;
        if( siy != iy ){
            int glid = ix + siy*Options.GetOptNX() + iz*Options.GetOptNX()*Options.GetOptNY();
            Properties[glid] = p;
            StoredPoints++;
        }
    }

    // symmetry: xy, yz, xz
    if( Options.GetOptSymmetryXY() && Options.GetOptSymmetryYZ() ){
        int siz = Options.GetOptNZ() - iz - 1;
        int six = Options.GetOptNX() - ix - 1;
        if( (siz != iz) && (six != ix) ){
            int glid = six + iy*Options.GetOptNX() + siz*Options.GetOptNX()*Options.GetOptNY();
            Properties[glid] = p;
            StoredPoints++;
        }
    }
    if( Options.GetOptSymmetryXY() && Options.GetOptSymmetryXZ() ){
        int siz = Options.GetOptNZ() - iz - 1;
        int siy = Options.GetOptNY() - iy - 1;
        if( (siz != iz) && (siy != iy) ){
            int glid = ix + siy*Options.GetOptNX() + siz*Options.GetOptNX()*Options.GetOptNY();
            Properties[glid] = p;
            StoredPoints++;
        }
    }
    if( Options.GetOptSymmetryYZ() && Options.GetOptSymmetryXZ() ){
        int six = Options.GetOptNX() - ix - 1;
        int siy = Options.GetOptNY() - iy - 1;
        if( (six != ix) && (siy != iy) ){
            int glid = six + siy*Options.GetOptNX() + iz*Options.GetOptNX()*Options.GetOptNY();
            Properties[glid] = p;
            StoredPoints++;
        }
    }

    // symmetry: xyz
    if( Options.GetOptSymmetryYZ() && Options.GetOptSymmetryXZ() && Options.GetOptSymmetryYZ() ){
        int six = Options.GetOptNX() - ix - 1;
        int siy = Options.GetOptNY() - iy - 1;
        int siz = Options.GetOptNZ() - iz - 1;
        if( (six != ix) && (siy != iy) && (siz != iz) ){
            int glid = six + siy*Options.GetOptNX() + siz*Options.GetOptNX()*Options.GetOptNY();
            Properties[glid] = p;
            StoredPoints++;
        }
    }

}

//------------------------------------------------------------------------------

bool CCubeAssembly::SaveCubeFile(void)
{
    // open output file
    FILE* p_fout;
    if( Options.GetArgCubeFile() != "-" ) {
        if( (p_fout = fopen(Options.GetArgCubeFile(),"w")) == NULL ) {
            CSmallString error;
            error << "unable to open output file " << Options.GetArgCubeFile();
            error << " (" << strerror(errno) << ")";
            ES_ERROR(error);
            return(false);
        }
    } else {
        p_fout = stdout;
    }

    // print comment
    fprintf(p_fout,"cube file\n");
    CSmallTimeAndDate ct;
    ct.GetActualTimeAndDate();
    fprintf(p_fout,"generated by cube-assembly at %s\n",(const char*)ct.GetSDateAndTime());

    // Ang -> bohr
    double conv = 1.8897259886;

    double dx = (Options.GetOptNX()-1)*Options.GetOptSX();
    double dy = (Options.GetOptNY()-1)*Options.GetOptSY();
    double dz = (Options.GetOptNZ()-1)*Options.GetOptSZ();

    CPoint corner(dx,dy,dz);
    corner /= -2.0;

    // print header
    fprintf(p_fout,"%5d %11.6f %11.6f %11.6f\n",Structure.GetNumberOfAtoms(),corner.x*conv,corner.y*conv,corner.z*conv);
    fprintf(p_fout,"%5d %11.6f %11.6f %11.6f\n",Options.GetOptNX(),Options.GetOptSX()*conv,0.0,0.0);
    fprintf(p_fout,"%5d %11.6f %11.6f %11.6f\n",Options.GetOptNY(),0.0,Options.GetOptSY()*conv,0.0);
    fprintf(p_fout,"%5d %11.6f %11.6f %11.6f\n",Options.GetOptNZ(),0.0,0.0,Options.GetOptSZ()*conv);

    // print structure
    for(int i=0; i < Structure.GetNumberOfAtoms(); i++){\
        int z = PeriodicTable.SearchZBySymbol(Structure.GetSymbol(i));
        CPoint pos = Structure.GetPosition(i);
        fprintf(p_fout,"%5d %11.6f %11.6f %11.6f %11.6f\n",z,(double)z,pos.x*conv,pos.y*conv,pos.z*conv);
    }

    // print data
    for(int ix=0; ix < Options.GetOptNX(); ix++) {
        for(int iy=0; iy < Options.GetOptNY(); iy++) {
            bool nl = false;
            for(int iz=0; iz < Options.GetOptNZ(); iz++) {
                int glid = ix + iy*Options.GetOptNX() + iz*Options.GetOptNX()*Options.GetOptNY();
                fprintf(p_fout,"% 12.5e",Properties[glid]);
                nl = false;
                if(iz % 6 == 5){
                    nl = true;
                    fprintf(p_fout,"\n");
                }
            }
            if( nl == false ) fprintf(p_fout,"\n");
        }
    }

    // close output file if necessary
    if( Options.GetArgCubeFile() != "-" ) {
        if( p_fout ) fclose(p_fout);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCubeAssembly::Finalize(void)
{
    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    vout << endl;
    vout << "# ==============================================================================" << endl;
    vout << "# cube-assembly terminated at " << dt.GetSDateAndTime() << endl;
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

