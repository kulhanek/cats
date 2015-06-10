// =============================================================================
// CATs - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2008 Petr Kulhanek, kulhanek@enzim.hu
//    Copyright (C) 2005 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2004 Petr Kulhanek, kulhanek@chemi.muni.cz
//
//    This library is free software; you can redistribute it and/or
//    modify it under the terms of the GNU Lesser General Public
//    License as published by the Free Software Foundation; either
//    version 2.1 of the License, or (at your option) any later version.
//
//    This library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public
//    License along with this library; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin Street, Fifth Floor,
//    Boston, MA  02110-1301  USA
// =============================================================================

#include <ResultItem.hpp>
#include <XMLElement.hpp>
#include <ErrorSystem.hpp>
#include <math.h>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CResultItem::CResultItem(void)
{
    Checked = false;

    Type = ERIT_NOT_DEFINED;

    IData = 0;
    RData = 0.0;
    RDataSum = 0.0;
    RDataSum2 = 0.0;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CResultItem::LoadAsTemplate(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        ES_ERROR("p_ele is NULL");
        return(false);
    }

    if( p_ele->GetName() != "ITEM" ) {
        ES_ERROR("p_ele is not ITEM");
        return(false);
    }

    bool result = true;

    int type = ERIT_NOT_DEFINED;
    result &= p_ele->GetAttribute("type",type);
    Type = (EResultItemType)type;
    result &= p_ele->GetAttribute("name",Name);
    result &= p_ele->GetAttribute("unit",Unit);
    CSmallString format;
    result &= p_ele->GetAttribute("format",format);
    Format.SetFormat(format);
    result &= p_ele->GetAttribute("descr",Description);

    if( result == false ) {
        ES_ERROR("unable to get ITEM attributes");
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CResultItem::CheckAsTemplate(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        ES_ERROR("p_ele is NULL");
        return(false);
    }

    if( p_ele->GetName() != "ITEM" ) {
        ES_ERROR("p_ele is not ITEM");
        return(false);
    }

    bool result = true;

    int type = ERIT_NOT_DEFINED;
    result &= p_ele->GetAttribute("type",type);
    if( Type != (EResultItemType)type ) {
        CSmallString error;
        error << "type mismatch for item '" << Name << "'";
        ES_ERROR(error);
        result = false;
    }

    CSmallString   unit;

    result &= p_ele->GetAttribute("unit",unit);
    if( unit != Unit ) {
        CSmallString error;
        error << "unit mismatch for item '" << Name << "'";
        ES_ERROR(error);
        result = false;
    }

    CSmallString   format;
    result &= p_ele->GetAttribute("format",format);
    if( format != Format.GetFormat() ) {
        CSmallString error;
        error << "format mismatch for item '" << Name << "'";
        ES_ERROR(error);
        result = false;
    }

    CSmallString   descr;
    result &= p_ele->GetAttribute("descr",descr);
    if( descr != Description ) {
        CSmallString error;
        error << "description mismatch for item '" << Name << "'";
        ES_ERROR(error);
        result = false;
    }

    return(result);
}

//------------------------------------------------------------------------------

bool CResultItem::SaveAsTemplate(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        ES_ERROR("p_ele is NULL");
        return(false);
    }

    if( p_ele->GetName() != "ITEM" ) {
        ES_ERROR("p_ele is not ITEM");
        return(false);
    }

    bool result = true;

    int type = Type;
    p_ele->SetAttribute("type",type);
    p_ele->SetAttribute("name",Name);
    p_ele->SetAttribute("unit",Unit);
    p_ele->SetAttribute("format",Format.GetFormat());
    p_ele->SetAttribute("descr",Description);

    if( result == false ) {
        ES_ERROR("unable to set ITEM attributes");
    }

    return(result);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CResultItem::LoadAsData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        ES_ERROR("p_ele is NULL");
        return(false);
    }

    if( p_ele->GetName() != "ITEM" ) {
        ES_ERROR("p_ele is not ITEM");
        return(false);
    }

    bool result = true;
    CSmallString name,data;

    result &= p_ele->GetAttribute("name",name);
    result &= p_ele->GetAttribute("data",data);

    if( result == false ) {
        ES_ERROR("unable to get ITEM attributes");
        return(false);
    }

    // check name
    if( name != Name ) {
        ES_ERROR("item name mismatch");
        return(false);
    }

    // convert data
    switch(Type) {
    case ERIT_NOT_DEFINED:
        ES_ERROR("format is not defined - no conversion is possible");
        return(false);
    case ERIT_INTEGER:
        if( sscanf(data,"%d",&IData) != 1 ) {
            CSmallString error;
            error << "unable to convert data (" << data << ")"
                  << " to integer";
            ES_ERROR(error);
            return(false);
        }
        break;
    case ERIT_REAL:
        if( sscanf(data,"%lf",&RData) != 1 ) {
            CSmallString error;
            error << "unable to convert data (" << data << ")"
                  << " to double";
            ES_ERROR(error);
            return(false);
        }
        break;
    case ERIT_STRING:
        SData = data;
        break;
    default:
        ES_ERROR("not implemented");
        break;
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CResultItem::SaveAsData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        ES_ERROR("p_ele is NULL");
        return(false);
    }

    if( p_ele->GetName() != "ITEM" ) {
        ES_ERROR("p_ele is not ITEM");
        return(false);
    }

    bool result = true;

    p_ele->SetAttribute("name",Name);
    p_ele->SetAttribute("data",SData);

    if( result == false ) {
        ES_ERROR("unable to set ITEM attributes");
    }

    return(result);
}

//------------------------------------------------------------------------------

bool CResultItem::SetData(const CSmallString& name,const CSmallString& value)
{
    Name = name;
    SData = value;

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CResultItem::ResetData(void)
{
    Checked = false;
    IData = 0;
    RData = 0.0;
    SData = "";
}

//------------------------------------------------------------------------------

void CResultItem::StatData(void)
{
    RDataSum += RData;
    RDataSum2 += RData*RData;
}

//------------------------------------------------------------------------------

bool CResultItem::LoadStatistics(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        ES_ERROR("p_ele is NULL");
        return(false);
    }

    if( p_ele->GetName() != "ITEM" ) {
        ES_ERROR("p_ele is not ITEM");
        return(false);
    }

    bool result = true;

    int type = ERIT_NOT_DEFINED;
    result &= p_ele->GetAttribute("type",type);
    Type = (EResultItemType)type;

    result &= p_ele->GetAttribute("name",Name);
    result &= p_ele->GetAttribute("unit",Unit);

    CSmallString format;
    result &= p_ele->GetAttribute("format",format);
    Format.SetFormat(format);

    result &= p_ele->GetAttribute("descr",Description);

    result &= p_ele->GetAttribute("IData",IData);
    result &= p_ele->GetAttribute("RData",RData);
    result &= p_ele->GetAttribute("RDataSum",RDataSum);
    result &= p_ele->GetAttribute("RDataSum2",RDataSum2);
    result &= p_ele->GetAttribute("SData",SData);

    if( result == false ) {
        ES_ERROR("unable to set ITEM attributes");
    }

    return(result);
}

//------------------------------------------------------------------------------

bool CResultItem::SaveStatistics(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        ES_ERROR("p_ele is NULL");
        return(false);
    }

    if( p_ele->GetName() != "ITEM" ) {
        ES_ERROR("p_ele is not ITEM");
        return(false);
    }

    bool result = true;

    int type = Type;
    p_ele->SetAttribute("type",type);
    p_ele->SetAttribute("name",Name);
    p_ele->SetAttribute("unit",Unit);
    p_ele->SetAttribute("format",Format.GetFormat());
    p_ele->SetAttribute("descr",Description);

    p_ele->SetAttribute("IData",IData);
    p_ele->SetAttribute("RData",RData);
    p_ele->SetAttribute("RDataSum",RDataSum);
    p_ele->SetAttribute("RDataSum2",RDataSum2);
    p_ele->SetAttribute("SData",SData);

    if( result == false ) {
        ES_ERROR("unable to set ITEM attributes");
    }

    return(result);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CResultItem::SetType(EResultItemType type)
{
    Type = type;

    switch(Type) {
    case ERIT_NOT_DEFINED:
        Format.SetFormat("");
        break;
    case ERIT_INTEGER:
        Format.SetFormat("%6d");
        break;
    case ERIT_REAL:
        Format.SetFormat("%7f");
        break;
    case ERIT_STRING:
        Format.SetFormat("%7s");
        break;
    default:
        ES_ERROR("not implemented");
        break;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CResultItem::PrintItem(FILE* p_fout)
{
    unsigned int max_len = 0;
    if( max_len < Name.GetLength() ) max_len = Name.GetLength();
    if( max_len < Format.GetRecordLength() ) {
        max_len = Format.GetRecordLength();
    }
    switch(Type) {
    case ERIT_NOT_DEFINED:
        ES_ERROR("format is not specified");
        return(false);
    case ERIT_INTEGER:
        fprintf(p_fout,Format.GetFormat(),IData);
        break;
    case ERIT_REAL:
        fprintf(p_fout,Format.GetFormat(),RData);
        break;
    case ERIT_STRING:
        fprintf(p_fout,Format.GetFormat(),(const char*)SData);
        break;
    default:
        ES_ERROR("not implemented");
        return(false);
    }
    for(unsigned int i=0; i < max_len-Format.GetRecordLength(); i++) {
        fprintf(p_fout," ");
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CResultItem::PrintItemAve(FILE* p_stat,int rec_number)
{
    if( rec_number == 0 ) return(false);

    unsigned int max_len = 0;
    if( max_len < Name.GetLength() ) max_len = Name.GetLength();
    if( max_len < Format.GetRecordLength() ) {
        max_len = Format.GetRecordLength();
    }
    switch(Type) {
    case ERIT_NOT_DEFINED:
        ES_ERROR("format is not specified");
        return(false);
    case ERIT_INTEGER: {
        unsigned int smax_len = Format.GetRecordLength();
        for(unsigned int i=0; i < (smax_len-1)/2; i++) fprintf(p_stat," ");
        fprintf(p_stat,"-");
        for(unsigned int i=0; i < (smax_len-1)/2; i++) fprintf(p_stat," ");
        if( (smax_len-1) % 2 != 0 ) fprintf(p_stat," ");
    }
    break;
    case ERIT_REAL: {
        double average = RDataSum/(double)rec_number;
        fprintf(p_stat,Format.GetFormat(),average);
    }
    break;
    case ERIT_STRING: {
        unsigned int max_len = Format.GetRecordLength();
        for(unsigned int i=0; i < (max_len-1)/2; i++) fprintf(p_stat," ");
        fprintf(p_stat,"-");
        for(unsigned int i=0; i < (max_len-1)/2; i++) fprintf(p_stat," ");
        if( (max_len-1) % 2 != 0 ) fprintf(p_stat," ");
    }
    break;
    default:
        ES_ERROR("not implemented");
        return(false);
    }
    for(unsigned int i=0; i < max_len-Format.GetRecordLength(); i++) {
        fprintf(p_stat," ");
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CResultItem::PrintItemSDev(FILE* p_stat,int rec_number)
{
    if( rec_number == 0 ) return(false);

    unsigned int max_len = 0;
    if( max_len < Name.GetLength() ) max_len = Name.GetLength();
    if( max_len < Format.GetRecordLength() ) {
        max_len = Format.GetRecordLength();
    }
    switch(Type) {
    case ERIT_NOT_DEFINED:
        ES_ERROR("format is not specified");
        return(false);
    case ERIT_INTEGER: {
        unsigned int smax_len = Format.GetRecordLength();
        for(unsigned int i=0; i < (smax_len-1)/2; i++) fprintf(p_stat," ");
        fprintf(p_stat,"-");
        for(unsigned int i=0; i < (smax_len-1)/2; i++) fprintf(p_stat," ");
        if( (smax_len-1) % 2 != 0 ) fprintf(p_stat," ");
    }
    break;
    case ERIT_REAL: {
        double sdev = 0.0;
        if( rec_number*RDataSum2 - RDataSum*RDataSum > 0 ) {
            sdev = sqrt( rec_number*RDataSum2 - RDataSum*RDataSum )
                   / (double)rec_number;
        }
        fprintf(p_stat,Format.GetFormat(),sdev);
    }
    break;
    case ERIT_STRING: {
        unsigned int max_len = Format.GetRecordLength();
        for(unsigned int i=0; i < (max_len-1)/2; i++) fprintf(p_stat," ");
        fprintf(p_stat,"-");
        for(unsigned int i=0; i < (max_len-1)/2; i++) fprintf(p_stat," ");
        if( (max_len-1) % 2 != 0 ) fprintf(p_stat," ");
    }
    break;
    default:
        ES_ERROR("not implemented");
        return(false);
    }
    for(unsigned int i=0; i < max_len-Format.GetRecordLength(); i++) {
        fprintf(p_stat," ");
    }

    return(false);
}

//------------------------------------------------------------------------------

bool CResultItem::PrintItemSDevAve(FILE* p_stat,int rec_number)
{
    if( rec_number == 0 ) return(false);

    unsigned int max_len = 0;
    if( max_len < Name.GetLength() ) max_len = Name.GetLength();
    if( max_len < Format.GetRecordLength() ) {
        max_len = Format.GetRecordLength();
    }
    switch(Type) {
    case ERIT_NOT_DEFINED:
        ES_ERROR("format is not specified");
        return(false);
    case ERIT_INTEGER: {
        unsigned int smax_len = Format.GetRecordLength();
        for(unsigned int i=0; i < (smax_len-1)/2; i++) fprintf(p_stat," ");
        fprintf(p_stat,"-");
        for(unsigned int i=0; i < (smax_len-1)/2; i++) fprintf(p_stat," ");
        if( (smax_len-1) % 2 != 0 ) fprintf(p_stat," ");
    }
    break;
    case ERIT_REAL: {
        double sdev = 0.0, sdevx=0.0;
        if( rec_number*RDataSum2 - RDataSum*RDataSum > 0 ) {
            sdev = sqrt( rec_number*RDataSum2 - RDataSum*RDataSum )
                   / (double)rec_number;
        }
        sdevx = sdev / sqrt((double)rec_number);
        fprintf(p_stat,Format.GetFormat(),sdevx);
    }
    break;
    case ERIT_STRING: {
        unsigned int max_len = Format.GetRecordLength();
        for(unsigned int i=0; i < (max_len-1)/2; i++) fprintf(p_stat," ");
        fprintf(p_stat,"-");
        for(unsigned int i=0; i < (max_len-1)/2; i++) fprintf(p_stat," ");
        if( (max_len-1) % 2 != 0 ) fprintf(p_stat," ");
    }
    break;
    default:
        ES_ERROR("not implemented");
        return(false);
    }
    for(unsigned int i=0; i < max_len-Format.GetRecordLength(); i++) {
        fprintf(p_stat," ");
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

