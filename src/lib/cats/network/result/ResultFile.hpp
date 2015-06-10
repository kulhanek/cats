#ifndef ResultFileH
#define ResultFileH
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
#include <ResultItem.hpp>
#include <SimpleList.hpp>
#include <SimpleMutex.hpp>

//------------------------------------------------------------------------------

class CXMLElement;

//------------------------------------------------------------------------------

class CATS_PACKAGE CResultFile {
public:
    CResultFile(void);

// setup methods --------------------------------------------------------------
    void SetResultFileName(const CSmallString& result_name);
    void SetStatisticsFileName(const CSmallString& stat_name);

// template operations --------------------------------------------------------
    /// load template defnition from a file
    bool LoadTemplate(const CSmallString& name,bool verbose=false);

    /// load or check template from XML stream - mutex protected
    bool LoadOrCheckTemplate(CXMLElement* p_ele);

    /// save template to XML stream
    bool SaveTemplate(CXMLElement* p_ele);

    /// print item specification
    bool PrintItemSpecs(FILE* p_fout=NULL);

// result file ----------------------------------------------------------------
    /// open result file - not protected by mutex
    bool OpenResultFile(void);

    /// close result file - not protected by mutex
    bool CloseResultFile(void);

    /// register data and write them to file if it is opened - mutex protected
    bool WriteData(CXMLElement* p_ele,int client_id);

    /// read data from file - not protected by mutex
    bool ReadData(const CSmallString& name,bool verbose=false);

    /// save data to XML stream
    bool SaveData(CXMLElement* p_ele);

    /// print item specification
    bool PrintDataHeader(FILE* p_fout=NULL);

    /// prit data to result file
    bool PrintData(FILE* p_fout,int client_id);

    /// get number of collected data
    int GetNumberOfCollectedData(void);

// statistics file ------------------------------------------------------------
    /// write statistics - mutex protected (used by flush command)
    bool WriteStatistics(void);

    /// load statistics from XML stream
    bool LoadStatistics(CXMLElement* p_ele);

    /// write statistics to XML stream
    bool WriteStatistics(CXMLElement* p_ele);

// maintanance ----------------------------------------------------------------
    /// flush all data
    bool FlushData(void);

// informational methods ------------------------------------------------------
    /// find result item
    CResultItem* FindItem(const CSmallString& name);

// section of private data ----------------------------------------------------
private:
    CSimpleMutex                ResultMutex;
    int                         RecordNumber;

    CSimpleList<CResultItem>    Items;
    CSmallString                ResultFileName;
    FILE*                       ResultFile;
    CSmallString                StatFileName;
};


//------------------------------------------------------------------------------

#endif
