#ifndef ResultItemH
#define ResultItemH
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

#include <CATsMainHeader.hpp>
#include <SmallString.hpp>
#include <FormatSpec.hpp>

//------------------------------------------------------------------------------

class CXMLElement;

//------------------------------------------------------------------------------

enum EResultItemType {
    ERIT_NOT_DEFINED,
    ERIT_INTEGER,
    ERIT_REAL,
    ERIT_STRING
};

//------------------------------------------------------------------------------

class CATS_PACKAGE CResultItem {
public:
    CResultItem(void);

// template operations --------------------------------------------------------
    /// load item type, name, and unit
    bool LoadAsTemplate(CXMLElement* p_ele);

    /// check item type, name, and unit
    bool CheckAsTemplate(CXMLElement* p_ele);

    /// save item type, name, and unit
    bool SaveAsTemplate(CXMLElement* p_ele);

// data operations --------------------------------------------------------
    /// set data type and default format
    void SetType(EResultItemType type);

    /// load item data
    bool LoadAsData(CXMLElement* p_ele);

    /// save item name and data
    bool SaveAsData(CXMLElement* p_ele);

    /// set item data from file
    bool SetData(const CSmallString& name,const CSmallString& value);

    /// reset current data
    void ResetData(void);

    /// print item data
    bool PrintItem(FILE* p_fout);

    /// load item statistics
    bool LoadStatistics(CXMLElement* p_ele);

    /// save item statistics
    bool SaveStatistics(CXMLElement* p_ele);

// statistics -----------------------------------------------------------------
    /// do statistics on data
    void StatData(void);

    /// print formated average
    bool PrintItemAve(FILE* p_stat,int rec_number);

    /// print formated standard deviation
    bool PrintItemSDev(FILE* p_stat,int rec_number);

    /// print formated standard deviation of average
    bool PrintItemSDevAve(FILE* p_stat,int rec_number);

// section of public data ----------------------------------------------------
public:
    bool                Checked;

    EResultItemType     Type;

    CSmallString        Name;
    CSmallString        Unit;
    CFormatSpec         Format;
    CSmallString        Description;

// section of private data ----------------------------------------------------
private:
    // integer -------------------------
    int                 IData;
    // real ----------------------------
    double              RData;
    double              RDataSum;
    double              RDataSum2;
    // real ----------------------------
    CSmallString        SData;
};

//------------------------------------------------------------------------------

#endif
