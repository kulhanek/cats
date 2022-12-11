// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2022 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <errno.h>
#include <string.h>
#include <ErrorSystem.hpp>
#include <PeriodicTable.hpp>
#include <FileName.hpp>
#include <FortranIO.hpp>

#include <SmallTimeAndDate.hpp>
#include <AmberMaskAtoms.hpp>
#include <AmberMaskResidues.hpp>
#include <AmberTrajectory.hpp>
#include <XYZStructure.hpp>

#include "TopCrd2Mdl.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTopCrd2Mdl::CTopCrd2Mdl(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CTopCrd2Mdl::Init(int argc,char* argv[])
{
    // encode program options
    int result = Options.ParseCmdLine(argc,argv);

    // should we exit or was it error?
    if( result != SO_CONTINUE ) return(result);

    // print header --------------------------------------------------------------
    if( Options.GetOptVerbose() ) {
        CSmallTimeAndDate dt;
        dt.GetActualTimeAndDate();

        printf("\n");
        printf("# ==============================================================================\n");
        printf("# topcrd2mdl (CATs utility) started at %s\n",(const char*)dt.GetSDateAndTime());
        printf("# ==============================================================================\n");
        printf("#\n");
        printf("# Topology name      : %s\n",(const char*)Options.GetArgTopologyName());
        if( Options.GetArgCrdName() != "-" ) {
            printf("# Input coordinates  : %s\n",(const char*)Options.GetArgCrdName());
        } else {
            printf("# Input coordinates  : - (standard input)\n");
        }
        if( Options.GetArgMDLName() != "-" ) {
            printf("# Output MDL file    : %s\n",(const char*)Options.GetArgMDLName());
        } else {
            printf("# Output MDL file    : - (standard output)\n");
        }
        printf("# ------------------------------------------------------------------------------\n");
            printf("# Embedded sites     : %s\n",(const char*)Options.GetOptEmbeddedSites());
        printf("# ------------------------------------------------------------------------------\n");
        printf("\n");
    }

    return( result );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopCrd2Mdl::Run(void)
{
    // load topology
    if( Topology.Load(Options.GetArgTopologyName()) == false ) {
        CSmallString error;
        error << "unable to load specified topology: " << Options.GetArgTopologyName();
        ES_ERROR(error);
        return(false);
    }

    // open input file
    FILE* p_fin = NULL;
    if( Options.GetArgCrdName() != "-" ) {
        if( (p_fin = fopen(Options.GetArgCrdName(),"r")) == NULL ) {
            CSmallString error;
            error << "unable to open input file " << Options.GetArgCrdName();
            error << " (" << strerror(errno) << ")";
            ES_ERROR(error);
            return(false);
        }
    } else {
        p_fin = stdin;
    }

    // load coordinates
    Topology.BuidListOfNeighbourAtoms();
    Coordinates.AssignTopology(&Topology);

    bool result = false;
    if( p_fin == stdin ) {
        result = ReadCRD(p_fin);
    } else {
        fclose(p_fin);
        p_fin = NULL;
        result = ReadCRD(Options.GetArgCrdName()); // NetCDF requires to use a file directly
    }

    if( p_fin != NULL ){
        fclose(p_fin);
        p_fin = NULL;
    }

    if( result == false ) {
        CSmallString error;
        error << "an error occured in reading of input coordinates";
        ES_ERROR(error);
        return(false);
    }

    // open output file
    FILE* p_fout;
    if( Options.GetArgMDLName() != "-" ) {
        if( (p_fout = fopen(Options.GetArgMDLName(),"w")) == NULL ) {
            CSmallString error;
            error << "unable to open output file " << Options.GetArgMDLName();
            error << " (" << strerror(errno) << ")";
            ES_ERROR(error);
            return(false);
        }
    } else {
        p_fout = stdout;
    }

    // write output file
    result = WriteMDL(p_fout);

    // close output file if necessary
    if( Options.GetArgMDLName() != "-" ) {
        if( p_fout ) fclose(p_fout);
    }

    return(result);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopCrd2Mdl::Finalize(void)
{
    if( Options.GetOptVerbose() ) {
        CSmallTimeAndDate dt;
        dt.GetActualTimeAndDate();

        fprintf(stdout,"\n");
        fprintf(stdout,"# ==============================================================================\n");
        fprintf(stdout,"# %s terminated at %s\n",(const char*)Options.GetProgramName(),(const char*)dt.GetSDateAndTime());
        fprintf(stdout,"# ==============================================================================\n");
    }

    if( Options.GetOptVerbose() || ErrorSystem.IsError() ) {
        ErrorSystem.PrintErrors(stderr);
        fprintf(stdout,"\n");
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopCrd2Mdl::ReadCRD(FILE* p_fin)
{
    if( Coordinates.Load(p_fin) == false ) {
        CSmallString error;
        error << "unable to load input coordinates (crd): " << Options.GetArgCrdName();
        ES_ERROR(error);
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Mdl::ReadCRD(const CSmallString& name)
{
    if( Coordinates.Load(name,false,AMBER_RST_UNKNOWN) == false ) {
        CSmallString error;
        error << "unable to load input coordinates (crd): " << Options.GetArgCrdName();
        ES_ERROR(error);
        return(false);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopCrd2Mdl::WriteMDL(FILE* p_fout)
{
    CSmallString        fTITLE      = "20A4";
    CSmallString        fPOINTERS   = "10I8";
    CSmallString        fATMTYP     = "10I8";
    CSmallString        fATMNAME    = "20A4";
    CSmallString        fMASS       = "5e16.8";
    CSmallString        fCHG        = "5e16.8";
    CSmallString        fLJEPSILON  = "5e16.8";
    CSmallString        fLJSIGMA    = "5e16.8";
    CSmallString        fMULTI      = "10I8";
    CSmallString        fCOORD      = "5e16.8";

    //-----------------------------------

    if( InitTypes() == false ) return(false);

    //-----------------------------------

    CSmallTimeAndDate ct;
    ct.GetActualTimeAndDate();

    int outputlen;
    if( (outputlen = fprintf(p_fout,
                             "%%VERSION  VERSION_STAMP = V0001.000  DATE = %02d/%02d/%02d  %02d:%02d:%02d",
                             ct.GetMonth(),ct.GetDay(),ct.GetYear()%100,ct.GetHour(),ct.GetMinute(),ct.GetSecond())) <= 0 ) {
        ES_ERROR("unable write version stamp");
        return(false);
    }
    for(int i = outputlen; i < 80; i++) fputc(' ',p_fout);
    fputc('\n',p_fout);

    //-----------------------------------

    if( SaveSectionHeader(p_fout,"TITLE",fTITLE) == false ) return(false);
    CSmallString title = Topology.GetTitle();
    title.Trim();
    title += " [topcrd2mdl: ";
    title += LibVersion_CATs;
    title += "]";
    // trim from left
    int last = title.Verify(" ",-1,-1,true);
    if( last != -1 ){
        if( last > 80 ) last = 80;
        title = title.GetSubStringFromTo(0,last);
    }
    if( title == NULL ) title = "default_title";

    if( (outputlen = fprintf(p_fout,"%s",(const char*)title)) <= 0 ) {
        ES_ERROR("unable write title");
        return(false);
    }
    for(int i = outputlen; i < 80; i++) fputc(' ',p_fout);
    fputc('\n',p_fout);

    //-----------------------------------
    if( SaveSectionHeader(p_fout,"POINTERS",fPOINTERS) == false ) return(false);
    if( SavePointers(p_fout,fPOINTERS) == false ) return(false);

    //-----------------------------------
    if( SaveSectionHeader(p_fout,"ATMTYP",fATMTYP) == false ) return(false);
    if( SaveAtomType(p_fout,fATMTYP) == false ) return(false);

    //-----------------------------------
    if( SaveSectionHeader(p_fout,"ATMNAME",fATMNAME) == false ) return(false);
    if( SaveAtomName(p_fout,fATMNAME) == false ) return(false);

    //-----------------------------------
    if( SaveSectionHeader(p_fout,"MASS",fMASS) == false ) return(false);
    if( SaveAtomMass(p_fout,fMASS) == false ) return(false);

    //-----------------------------------
    if( SaveSectionHeader(p_fout,"CHG",fCHG) == false ) return(false);
    if( SaveAtomCharge(p_fout,fCHG) == false ) return(false);

    //-----------------------------------
    if( SaveSectionHeader(p_fout,"LJEPSILON",fLJEPSILON) == false ) return(false);
    if( SaveAtomEpsilon(p_fout,fLJEPSILON) == false ) return(false);

    //-----------------------------------
    if( SaveSectionHeader(p_fout,"LJSIGMA",fLJSIGMA) == false ) return(false);
    if( SaveAtomSigma(p_fout,fLJSIGMA) == false ) return(false);

    //-----------------------------------
    if( SaveSectionHeader(p_fout,"MULTI",fMULTI) == false ) return(false);
    if( SaveAtomMulti(p_fout,fMULTI) == false ) return(false);

    //-----------------------------------
    if( SaveSectionHeader(p_fout,"COORD",fCOORD) == false ) return(false);
    if( SaveAtomCoords(p_fout,fCOORD) == false ) return(false);

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Mdl::InitTypes(void)
{
    NTypes = 0;
    int ATypes = 0;

    AtomTypes.resize(Topology.AtomList.GetNumberOfAtoms());

    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++){
        CAmberAtom* p_atom = Topology.AtomList.GetAtom(i);
        CRISMType type;
        type.Charge = p_atom->GetStandardCharge() * 18.2223;
        type.Type   = p_atom->GetType();
        type.Mass   = p_atom->GetMass();
        type.Name   = p_atom->GetName();

        double a = Topology.NonBondedList.GetAParam(p_atom,p_atom);
        double b = Topology.NonBondedList.GetBParam(p_atom,p_atom);

        double eps = 0.0;
        double rii = 0.0;
        if( a != 0.0 ) {
            eps = b*b / (4.0 * a);
            rii = pow(2*a/b,1.0/6.0) * 0.5;
        }

        if( Options.GetOptEmbeddedSites() == "keep" ){
            type.Epsilon    = eps;
            type.Sigma      = rii;
        } else if( Options.GetOptEmbeddedSites() == "coincident" ){

            if( (eps == 0.0) && (rii == 0.0) ){
                // this is an embedded site
                CAmberAtom* p_hatom = NULL;
                int         nhosts = 0;
                for(int j=0; j < p_atom->GetNumberOfNeighbourAtoms(); j++){
                    CAmberAtom* p_oatom = Topology.AtomList.GetAtom( p_atom->GetNeighbourAtomIndex(j) );
                    double a = Topology.NonBondedList.GetAParam(p_oatom,p_oatom);
                    double b = Topology.NonBondedList.GetBParam(p_oatom,p_oatom);
                    if( (a != 0.0) && (b != 0.0) ){
                        nhosts++;
                        p_hatom = p_oatom;
                    }
                }

                if( nhosts != 1 ){
                    CSmallString error;
                    error << "embedded site '" << p_atom->GetAtomIndex() << "' has more then one connector";
                    ES_ERROR(error);
                    return(false);
                }
                CPoint p1 = Coordinates.GetPosition(p_atom->GetAtomIndex());
                CPoint p2 = Coordinates.GetPosition(p_hatom->GetAtomIndex());
                double d = Size(p1-p2);

                double a = Topology.NonBondedList.GetAParam(p_hatom,p_hatom);
                double b = Topology.NonBondedList.GetBParam(p_hatom,p_hatom);

                double eps = 0.0;
                double rii = 0.0;
                if( a != 0.0 ) {
                    eps = b*b / (4.0 * a);
                    rii = pow(2*a/b,1.0/6.0) * 0.5;
                }

                // see 10.1021/ct900460m
                type.Epsilon    = eps * 0.1;
                // switch to sigma and back
                type.Sigma      = (rii / pow(2.0,1.0/6.0)   - d) * pow(2.0,1.0/6.0);
            } else {
                // this is a regular site
                type.Epsilon    = eps;
                type.Sigma      = rii;
            }

        } else {
            CSmallString error;
            error << "unsupported embeddedsite mode: '" << Options.GetOptEmbeddedSites() << "'";
            ES_ERROR(error);
            return(false);
        }

        int rism_type;
        if( RISMTypes.count(type) == 0 ){
            NTypes++;
            rism_type = NTypes;
            RISMTypes[type] = rism_type;
        } else {
            rism_type = RISMTypes[type];
        }
        AtomTypes[i] = rism_type;

        if( AMBTypes.count(type.Type) == 0 ){
            ATypes++;
            AMBTypes[type.Type] = ATypes;
        }
    }
    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Mdl::SavePointers(FILE* p_file,const char* p_format)
{
    CFortranIO fortranio(p_file);
    fortranio.SetFormat(p_format);

    bool result = true;

    result &= fortranio.WriteInt(Topology.AtomList.GetNumberOfAtoms());
    result &= fortranio.WriteInt(NTypes);

    if( result == false ){
        ES_ERROR("unable to save POINTERS item");
        return(false);
    }

    fortranio.WriteEndOfSection();
    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Mdl::SaveAtomType(FILE* p_file,const char* p_format)
{
    CFortranIO fortranio(p_file);
    fortranio.SetFormat(p_format);

    bool result = true;

    std::map<CRISMType,int>::iterator   it = RISMTypes.begin();
    std::map<CRISMType,int>::iterator   ie = RISMTypes.end();

    while( it != ie ){
        CRISMType type = it->first;
        int atype = AMBTypes[ type.Type ];
        result &= fortranio.WriteInt(atype);
        it++;
    }

    if( result == false ){
        ES_ERROR("unable to save ATMTYP item");
        return(false);
    }

    fortranio.WriteEndOfSection();
    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Mdl::SaveAtomName(FILE* p_file,const char* p_format)
{
    CFortranIO fortranio(p_file);
    fortranio.SetFormat(p_format);

    bool result = true;

    std::map<CRISMType,int>::iterator   it = RISMTypes.begin();
    std::map<CRISMType,int>::iterator   ie = RISMTypes.end();

    while( it != ie ){
        CRISMType type = it->first;
        result &= fortranio.WriteString(type.Name);
        it++;
    }

    if( result == false ){
        ES_ERROR("unable to save ATMNAME item");
        return(false);
    }

    fortranio.WriteEndOfSection();
    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Mdl::SaveAtomMass(FILE* p_file,const char* p_format)
{
    CFortranIO fortranio(p_file);
    fortranio.SetFormat(p_format);

    bool result = true;

    std::map<CRISMType,int>::iterator   it = RISMTypes.begin();
    std::map<CRISMType,int>::iterator   ie = RISMTypes.end();

    while( it != ie ){
        CRISMType type = it->first;
        result &= fortranio.WriteReal(type.Mass);
        it++;
    }

    if( result == false ){
        ES_ERROR("unable to save MASS item");
        return(false);
    }

    fortranio.WriteEndOfSection();
    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Mdl::SaveAtomCharge(FILE* p_file,const char* p_format)
{
    CFortranIO fortranio(p_file);
    fortranio.SetFormat(p_format);

    bool result = true;

    std::map<CRISMType,int>::iterator   it = RISMTypes.begin();
    std::map<CRISMType,int>::iterator   ie = RISMTypes.end();

    while( it != ie ){
        CRISMType type = it->first;
        result &= fortranio.WriteReal(type.Charge);
        it++;
    }

    if( result == false ){
        ES_ERROR("unable to save CHG item");
        return(false);
    }

    fortranio.WriteEndOfSection();
    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Mdl::SaveAtomEpsilon(FILE* p_file,const char* p_format)
{
    CFortranIO fortranio(p_file);
    fortranio.SetFormat(p_format);

    bool result = true;

    std::map<CRISMType,int>::iterator   it = RISMTypes.begin();
    std::map<CRISMType,int>::iterator   ie = RISMTypes.end();

    while( it != ie ){
        CRISMType type = it->first;
        result &= fortranio.WriteReal(type.Epsilon);
        it++;
    }

    if( result == false ){
        ES_ERROR("unable to save LJEPSILON item");
        return(false);
    }

    fortranio.WriteEndOfSection();
    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Mdl::SaveAtomSigma(FILE* p_file,const char* p_format)
{
    CFortranIO fortranio(p_file);
    fortranio.SetFormat(p_format);

    bool result = true;

    std::map<CRISMType,int>::iterator   it = RISMTypes.begin();
    std::map<CRISMType,int>::iterator   ie = RISMTypes.end();

    while( it != ie ){
        CRISMType type = it->first;
        result &= fortranio.WriteReal(type.Sigma);
        it++;
    }

    if( result == false ){
        ES_ERROR("unable to save LJSIGMA item");
        return(false);
    }

    fortranio.WriteEndOfSection();
    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Mdl::SaveAtomMulti(FILE* p_file,const char* p_format)
{
    CFortranIO fortranio(p_file);
    fortranio.SetFormat(p_format);

    bool result = true;

    std::map<CRISMType,int>::iterator   it = RISMTypes.begin();
    std::map<CRISMType,int>::iterator   ie = RISMTypes.end();

    while( it != ie ){
        int rism_type = it->second;

        int count = 0;
        for(int i = 0; i < Topology.AtomList.GetNumberOfAtoms(); i++){
            if( AtomTypes[i] == rism_type ) count++;
        }

        result &= fortranio.WriteInt(count);
        it++;
    }

    if( result == false ){
        ES_ERROR("unable to save MULTI item");
        return(false);
    }

    fortranio.WriteEndOfSection();
    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Mdl::SaveAtomCoords(FILE* p_file,const char* p_format)
{
    CFortranIO fortranio(p_file);
    fortranio.SetFormat(p_format);

    bool result = true;

    std::map<CRISMType,int>::iterator   it = RISMTypes.begin();
    std::map<CRISMType,int>::iterator   ie = RISMTypes.end();

    while( it != ie ){
        int rism_type = it->second;

        for(int i = 0; i < Topology.AtomList.GetNumberOfAtoms(); i++){
            if( AtomTypes[i] == rism_type ){
                CPoint pos = Coordinates.GetPosition(i);
                result &= fortranio.WriteReal(pos.x);
                result &= fortranio.WriteReal(pos.y);
                result &= fortranio.WriteReal(pos.z);
            }
        }

        it++;
    }

    if( result == false ){
        ES_ERROR("unable to save COORD item");
        return(false);
    }

    fortranio.WriteEndOfSection();
    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Mdl::SaveSectionHeader(FILE* p_top,const char* p_section_name,
                                    const char* p_section_format)
{
    int outputlen;

    if( (outputlen = fprintf(p_top,"%%FLAG %s",p_section_name)) <= 0 ) {
        CSmallString    error;
        error << "unable write header of %%FLAG " << p_section_name << " section";
        ES_ERROR(error);
        return(false);
    }

    for(int i = outputlen; i < 80; i++) fputc(' ',p_top);
    fputc('\n',p_top);

    if( (outputlen = fprintf(p_top,"%%FORMAT(%s)",(char*)p_section_format)) <= 0 ) {
        CSmallString    error;
        error << "unable write format of %%FLAG " << p_section_name << " section";
        ES_ERROR(error);
        return(false);
    }

    for(int i = outputlen; i < 80; i++) fputc(' ',p_top);
    fputc('\n',p_top);

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

