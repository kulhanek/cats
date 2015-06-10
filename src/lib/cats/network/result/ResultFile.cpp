// =============================================================================
// CATs - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <ResultFile.hpp>
#include <ErrorSystem.hpp>
#include <SimpleIterator.hpp>
#include <PrmFile.hpp>
#include <ResultItem.hpp>
#include <XMLElement.hpp>
#include <errno.h>
#include <string.h>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CResultFile::CResultFile(void)
{
    ResultFile = NULL;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CResultFile::SetResultFileName(const CSmallString& result_name)
{
    ResultFileName = result_name;
}

//------------------------------------------------------------------------------

void CResultFile::SetStatisticsFileName(const CSmallString& stat_name)
{
    StatFileName = stat_name;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CResultFile::LoadTemplate(const CSmallString& name,bool verbose)
{
    CPrmFile template_file;

    // load template file
    if( template_file.Read(name) == false ) {
        ES_ERROR("unable to load template file");
        return(false);
    }

    // analyse types
    if( template_file.OpenSection("types") == false ) {
        ES_ERROR("[types] section is mandatory");
        return(false);
    }

    bool line_found = template_file.FirstLine();

    while( line_found ) {
        CSmallString key,value;

        bool result = true;

        result &= template_file.GetLineKey(key);
        result &= template_file.GetLineValue(value);

        if( result == false ) {
            ES_ERROR("result item name and/or type is/are not specified");
            return(false);
        }

        if( (value != "integer" ) &&
                (value != "int" ) &&
                (value != "real" ) &&
                (value != "double" ) &&
                (value != "string" ) ) {
            CSmallString error;
            error << "type '" << value << "' is not supported (use integer, real or string)";
            ES_ERROR(error);
            return(false);
        }

        // create record
        CResultItem* p_item;

        try {
            p_item = new CResultItem;
        } catch(...) {
            ES_ERROR("unable to allocate result item");
            return(false);
        }

        p_item->Name = key;

        if( (value == "int") || (value == "integer") ) {
            p_item->SetType(ERIT_INTEGER);
        } else if( (value == "real") || (value == "double") ) {
            p_item->SetType(ERIT_REAL);
        } else if( value == "string" ) {
            p_item->SetType(ERIT_STRING);
        } else {
            ES_ERROR("not implemented");
            return(false);
        }

        Items.InsertToEnd(p_item,0,true);

        line_found = template_file.NextLine();
    }

    template_file.SetSecAsProcessed();

    // units --------------------------
    if( template_file.OpenSection("units") == true ) {
        // units are optional
        CSimpleIterator<CResultItem> I(Items);
        CResultItem* p_item;

        while( (p_item = I.Current()) != NULL ) {
            template_file.GetStringByKey(p_item->Name,p_item->Unit);
            I++;
        }
    }

    // formats --------------------------
    if( template_file.OpenSection("formats") == true ) {
        // formats are optional
        CSimpleIterator<CResultItem> I(Items);
        CResultItem* p_item;

        while( (p_item = I.Current()) != NULL ) {
            CSmallString format;
            if( template_file.GetStringByKey(p_item->Name,format) == true ) {
                p_item->Format.SetFormat(format);

                bool valid_format = true;

                // validate format
                switch(p_item->Type) {
                case ERIT_NOT_DEFINED:
                    valid_format = false;
                    break;
                case ERIT_INTEGER:
                    valid_format = p_item->Format.IsFormatValid(EFT_INTEGER);
                    break;
                case ERIT_REAL:
                    valid_format = p_item->Format.IsFormatValid(EFT_DOUBLE);
                    break;
                case ERIT_STRING:
                    valid_format = p_item->Format.IsFormatValid(EFT_STRING);
                    break;
                default:
                    ES_ERROR("not implemented");
                    break;
                }

                if( valid_format == false ) {
                    CSmallString error;
                    error << "specified format (" << format << ") is not valid";
                    ES_ERROR(error);
                    return(false);
                }
            }

            I++;
        }
    }

    // descriptions --------------------------
    if( template_file.OpenSection("descriptions") == true ) {
        // descriptions are optional
        CSimpleIterator<CResultItem> I(Items);
        CResultItem* p_item;

        while( (p_item = I.Current()) != NULL ) {
            template_file.GetStringByKey(p_item->Name,p_item->Description);
            I++;
        }
    }

    if( verbose ) {
        printf("\n");
        printf("Following template file was read:\n");
        printf("\n");
        PrintItemSpecs(stdout);
        printf("\n");
    }

    if( template_file.CountULines() > 0 ) {
        if( verbose ) {
            printf("Following lines from template file were not understood:\n");
            printf("-------------------------------------------------------\n");
            template_file.Dump(stdout,true);
            printf("\n");
        }
        ES_ERROR("template file contains lines that were not understood");
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CResultFile::LoadOrCheckTemplate(CXMLElement* p_ele)
{
    ResultMutex.Lock();

    if( p_ele == NULL ) {
        ES_ERROR("p_ele is NULL");
        ResultMutex.Unlock();
        return(false);
    }

    if( p_ele->GetName() != "TEMPLATE" ) {
        ES_ERROR("p_ele is not TEMPLATE");
        ResultMutex.Unlock();
        return(false);
    }

    // load or check?

    if( Items.NumOfMembers() <= 0 ) {
        // load data

        CXMLElement* p_iele = p_ele->GetFirstChildElement("ITEM");

        while( p_iele != NULL ) {
            CResultItem* p_item;

            try {
                p_item = new CResultItem;
            } catch(...) {
                ES_ERROR("unable to allocate result item");
                Items.RemoveAll();
                ResultMutex.Unlock();
                return(false);
            }

            if( p_item->LoadAsTemplate(p_iele) == false ) {
                ES_ERROR("unable to load item");
                Items.RemoveAll();
                ResultMutex.Unlock();
                return(false);
            }

            Items.InsertToEnd(p_item,0,true);

            p_iele = p_iele->GetNextSiblingElement("ITEM");
        }

        // now we have data template - so open result file
        if( ResultFileName != NULL ) {
            if( OpenResultFile() == false ) {
                ES_ERROR("unable to open result file");
                Items.RemoveAll();
                ResultMutex.Unlock();
                return(false);
            }
        }
    } else {
        // reset checked flag
        CSimpleIterator<CResultItem> I(Items);
        CResultItem* p_item;

        while( (p_item = I.Current()) != NULL ) {
            p_item->Checked = false;
            I++;
        }

        // check data
        CXMLElement* p_iele = p_ele->GetFirstChildElement("ITEM");

        while( p_iele != NULL ) {
            CSmallString name;
            if( p_iele->GetAttribute("name",name) == false ) {
                ES_ERROR("unable to get name attribute from ITEM");
                ResultMutex.Unlock();
                return(false);
            }

            p_item = FindItem(name);
            if( p_item == NULL ) {
                CSmallString error;
                error << "unable to find item with name '" << name << "'";
                ES_ERROR(error);
                ResultMutex.Unlock();
                return(false);
            }

            if( p_item->Checked == true ) {
                CSmallString error;
                error << "item '" << name
                      << "' was already checked (do you not have duplicit items?)";
                ES_ERROR(error);
                ResultMutex.Unlock();
                return(false);
            }

            if( p_item->CheckAsTemplate(p_iele) == false ) {
                ES_ERROR("unable to check item");
                ResultMutex.Unlock();
                return(false);
            }

            p_item->Checked = true;

            p_iele = p_iele->GetNextSiblingElement("ITEM");
        }

        I.SetToBegin();

        while( (p_item = I.Current()) != NULL ) {
            if( p_item->Checked == false ) {
                CSmallString error;
                error << "item '" << p_item->Name
                      << "' was not checked (do you not have misconfigured clients?)";
                ES_ERROR(error);
                ResultMutex.Unlock();
                return(false);
            }
            I++;
        }

    }

    ResultMutex.Unlock();

    return(true);
}

//------------------------------------------------------------------------------

bool CResultFile::SaveTemplate(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        ES_ERROR("p_ele is NULL");
        return(false);
    }

    if( p_ele->GetName() != "TEMPLATE" ) {
        ES_ERROR("p_ele is not TEMPLATE");
        return(false);
    }

    CSimpleIterator<CResultItem> I(Items);
    CResultItem* p_item;

    while( (p_item = I.Current()) != NULL ) {
        CXMLElement* p_iele = p_ele->CreateChildElement("ITEM");
        if( p_iele == NULL ) {
            ES_ERROR("unable to create ITEM");
            return(false);
        }
        if( p_item->SaveAsTemplate(p_iele) == false ) {
            ES_ERROR("unable to save item as template");
            return(false);
        }
        I++;
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CResultFile::PrintItemSpecs(FILE* p_fout)
{
    // use stdout as default
    if( p_fout == NULL ) {
        p_fout = stdout;
    }

    unsigned int max_name = 0;
    unsigned int max_unit = 0;
    unsigned int max_form = 0;
    unsigned int max_desc = 0;

    CSimpleIterator<CResultItem> I(Items);
    CResultItem* p_item;

    // calculate max lengths for name and unit
    while( (p_item = I.Current()) != NULL ) {
        if( max_name < p_item->Name.GetLength() ) max_name = p_item->Name.GetLength();
        if( max_unit < p_item->Unit.GetLength() ) max_unit = p_item->Unit.GetLength();
        if( max_form < p_item->Format.GetFormat().GetLength() ) {
            max_form = p_item->Format.GetFormat().GetLength();
        }
        if( max_desc < p_item->Description.GetLength() ) max_desc = p_item->Description.GetLength();
        I++;
    }

    if( max_name < 4 ) max_name = 4; // Name
    if( max_unit < 4 ) max_unit = 4; // Unit
    if( max_form < 6 ) max_form = 6; // Format
    if( max_desc < 11 ) max_desc = 11; // Description

    // print header
    fprintf(p_fout,"# ");
    // name
    for(unsigned int i=0; i < (max_name-4)/2; i++) fprintf(p_fout," ");
    fprintf(p_fout,"Name");
    for(unsigned int i=0; i < (max_name-4)/2; i++) fprintf(p_fout," ");
    if( (max_name-4) % 2 != 0 ) fprintf(p_fout," ");
    fprintf(p_fout," ");

    // type
    fprintf(p_fout," Type  ");
    fprintf(p_fout," ");

    // unit
    for(unsigned int i=0; i < (max_unit-4)/2; i++) fprintf(p_fout," ");
    fprintf(p_fout,"Unit");
    for(unsigned int i=0; i < (max_unit-4)/2; i++) fprintf(p_fout," ");
    if( (max_unit-4) % 2 != 0 ) fprintf(p_fout," ");
    fprintf(p_fout," ");

    // format
    for(unsigned int i=0; i < (max_form-6)/2; i++) fprintf(p_fout," ");
    fprintf(p_fout,"Format");
    for(unsigned int i=0; i < (max_form-6)/2; i++) fprintf(p_fout," ");
    if( (max_form-6) % 2 != 0 ) fprintf(p_fout," ");
    fprintf(p_fout," ");

    // description
    for(unsigned int i=0; i < (max_desc-11)/2; i++) fprintf(p_fout," ");
    fprintf(p_fout,"Description");
    for(unsigned int i=0; i < (max_desc-11)/2; i++) fprintf(p_fout," ");
    if( (max_desc-11) % 2 != 0 ) fprintf(p_fout," ");
    fprintf(p_fout,"\n");


    fprintf(p_fout,"# ");
    // name
    for(unsigned int i=0; i < max_name; i++) fprintf(p_fout,"-");
    fprintf(p_fout," ");

    // type
    for(unsigned int i=0; i < 7; i++) fprintf(p_fout,"-");
    fprintf(p_fout," ");

    // unit
    for(unsigned int i=0; i < max_unit; i++) fprintf(p_fout,"-");
    fprintf(p_fout," ");

    // format
    for(unsigned int i=0; i < max_form; i++) fprintf(p_fout,"-");
    fprintf(p_fout," ");

    // description
    for(unsigned int i=0; i < max_desc; i++) fprintf(p_fout,"-");
    fprintf(p_fout," ");
    fprintf(p_fout,"\n");

    I.SetToBegin();

    while( (p_item = I.Current()) != NULL ) {
        fprintf(p_fout,"# ");
        fprintf(p_fout,"%-*s ",max_name,(const char*)p_item->Name);
        switch(p_item->Type) {
        case ERIT_INTEGER:
            fprintf(p_fout,"integer ");
            break;
        case ERIT_REAL:
            fprintf(p_fout,"real    ");
            break;
        case ERIT_STRING:
            fprintf(p_fout,"string  ");
            break;
        default:
            ES_ERROR("not implemented");
            return(false);
        }
        fprintf(p_fout,"%-*s ",max_unit,(const char*)p_item->Unit);
        fprintf(p_fout,"%-*s ",max_form,(const char*)p_item->Format.GetFormat());
        fprintf(p_fout,"%-*s\n",max_desc,(const char*)p_item->Description);
        I++;
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CResultFile::OpenResultFile(void)
{
    if( ResultFile != NULL ) {
        ES_ERROR("file is already opened");
        return(false);
    }

    if( Items.NumOfMembers() == 0 ) {
        ES_ERROR("no items are registered");
        return(false);
    }

    ResultFile = fopen(ResultFileName,"w");
    if( ResultFile == NULL ) {
        CSmallString error;
        error << "unable to open file '" << ResultFileName << "' (" << strerror(errno) << ")";
        ES_ERROR(error);
        return(false);
    }

    fprintf(ResultFile,"# Results collected by result-server.\n");
    fprintf(ResultFile,"#\n");

    fprintf(ResultFile,"# Data item description:\n");
    fprintf(ResultFile,"# ----------------------\n");
    fprintf(ResultFile,"#\n");

    // write header
    if( PrintItemSpecs(ResultFile) == false ) {
        ES_ERROR("unable to write data description");
        return(false);
    }

    fprintf(ResultFile,"#\n");
    fprintf(ResultFile,"# Collected data:\n");
    fprintf(ResultFile,"# ---------------\n");

    if( PrintDataHeader(ResultFile) == false ) {
        ES_ERROR("unable to write data item header to result file");
        Items.RemoveAll();
        ResultMutex.Unlock();
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CResultFile::CloseResultFile(void)
{
    if( ResultFile == NULL ) {
        ES_ERROR("file is not opened");
        return(false);
    }

    fclose(ResultFile);
    ResultFile = NULL;

    return(true);
}

//------------------------------------------------------------------------------

bool CResultFile::WriteData(CXMLElement* p_ele,int client_id)
{
    ResultMutex.Lock();

    if( p_ele == NULL ) {
        ES_ERROR("p_ele is NULL");
        ResultMutex.Unlock();
        return(false);
    }

    if( p_ele->GetName() != "DATA" ) {
        ES_ERROR("p_ele is not DATA");
        ResultMutex.Unlock();
        return(false);
    }

    // reset all data
    CSimpleIterator<CResultItem> I(Items);
    CResultItem* p_item;

    while( (p_item = I.Current()) != NULL ) {
        p_item->ResetData();
        I++;
    }

    // load data
    CXMLElement* p_iele = p_ele->GetFirstChildElement("ITEM");

    while( p_iele != NULL ) {
        CSmallString name;
        if( p_iele->GetAttribute("name",name) == false ) {
            ES_ERROR("unable to get name attribute from ITEM");
            ResultMutex.Unlock();
            return(false);
        }

        p_item = FindItem(name);
        if( p_item == NULL ) {
            CSmallString error;
            error << "unable to find item with name '" << name << "'";
            ES_ERROR(error);
            ResultMutex.Unlock();
            return(false);
        }

        if( p_item->Checked == true ) {
            CSmallString error;
            error << "data were already assigned to item '" << name
                  << "' (do you not have duplicit items?)";
            ES_ERROR(error);
            ResultMutex.Unlock();
            return(false);
        }

        if( p_item->LoadAsData(p_iele) == false ) {
            ES_ERROR("unable to load item");
            ResultMutex.Unlock();
            return(false);
        }

        p_item->Checked = true;

        p_iele = p_iele->GetNextSiblingElement("ITEM");
    }

    RecordNumber++;

    // write data to file
    if( ResultFile != NULL ) {
        if( PrintData(ResultFile,client_id) == false ) {
            RecordNumber--; // data are not valid
            ES_ERROR("unable to print data to result file");
            ResultMutex.Unlock();
            return(false);
        }
    }

    // do statistics
    I.SetToBegin();

    while( (p_item = I.Current()) != NULL ) {
        p_item->StatData();
        I++;
    }

    ResultMutex.Unlock();
    return(true);
}

//------------------------------------------------------------------------------

bool CResultFile::ReadData(const CSmallString& name,bool verbose)
{
    CPrmFile data_file;

    // load template file
    if( data_file.Read(name) == false ) {
        ES_ERROR("unable to load data file");
        return(false);
    }

    // analyse types
    if( data_file.OpenSection("data") == false ) {
        ES_ERROR("[data] section is mandatory");
        return(false);
    }

    bool line_found = data_file.FirstLine();

    while( line_found ) {
        CSmallString key,value;

        bool result = true;

        result &= data_file.GetLineKey(key);
        result &= data_file.GetLineValue(value);

        if( result == false ) {
            ES_ERROR("result item name and/or value is/are not specified");
            return(false);
        }

        // create record
        CResultItem* p_item;

        try {
            p_item = new CResultItem;
        } catch(...) {
            ES_ERROR("unable to allocate result item");
            return(false);
        }

        // all data are considered as strings during transfer
        p_item->SetData(key,value);

        Items.InsertToEnd(p_item,0,true);

        line_found = data_file.NextLine();
    }

    data_file.SetSecAsProcessed();

    return(true);
}

//------------------------------------------------------------------------------

bool CResultFile::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        ES_ERROR("p_ele is NULL");
        return(false);
    }

    if( p_ele->GetName() != "DATA" ) {
        ES_ERROR("p_ele is not DATA");
        return(false);
    }

    CSimpleIterator<CResultItem> I(Items);
    CResultItem* p_item;

    while( (p_item = I.Current()) != NULL ) {
        CXMLElement* p_iele = p_ele->CreateChildElement("ITEM");
        if( p_iele == NULL ) {
            ES_ERROR("unable to create ITEM");
            return(false);
        }
        if( p_item->SaveAsData(p_iele) == false ) {
            ES_ERROR("unable to save item as data");
            return(false);
        }
        I++;
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CResultFile::PrintDataHeader(FILE* p_fout)
{
    // use stdout as default
    if( p_fout == NULL ) {
        p_fout = stdout;
    }

    fprintf(p_fout,"#\n");
    fprintf(p_fout,"#   Number    ID  ");

    // write names
    CSimpleIterator<CResultItem> I(Items);
    CResultItem* p_item;

    while( (p_item = I.Current()) != NULL ) {
        unsigned int max_len = 0;
        if( max_len < p_item->Name.GetLength() ) max_len = p_item->Name.GetLength();
        if( max_len < p_item->Format.GetRecordLength() ) {
            max_len = p_item->Format.GetRecordLength();
        }
        for(unsigned int i=0; i < (max_len-p_item->Name.GetLength())/2; i++) {
            fprintf(p_fout," ");
        }
        fprintf(p_fout,"%s",(const char*)p_item->Name);
        for(unsigned int i=0; i < (max_len-p_item->Name.GetLength())/2; i++) {
            fprintf(p_fout," ");
        }
        if( (max_len-p_item->Name.GetLength()) % 2 != 0 ) {
            fprintf(p_fout," ");
        }
        fprintf(p_fout," ");
        I++;
    }
    fprintf(p_fout,"\n");

    // lines
    fprintf(p_fout,"# ---------- ---- ");

    I.SetToBegin();

    while( (p_item = I.Current()) != NULL ) {
        unsigned int max_len = 0;
        if( max_len < p_item->Name.GetLength() ) max_len = p_item->Name.GetLength();
        if( max_len < p_item->Format.GetRecordLength() ) {
            max_len = p_item->Format.GetRecordLength();
        }
        for(unsigned int i=0; i < max_len; i++) fprintf(p_fout,"-");
        fprintf(p_fout," ");
        I++;
    }
    fprintf(p_fout,"\n");

    return(true);
}

//------------------------------------------------------------------------------

bool CResultFile::PrintData(FILE* p_fout,int client_id)
{
    // use stdout as default
    if( p_fout == NULL ) {
        p_fout = stdout;
    }

    // lines
    fprintf(p_fout,"  %10d %4d ",RecordNumber,client_id);

    CSimpleIterator<CResultItem> I(Items);
    CResultItem* p_item;

    while( (p_item = I.Current()) != NULL ) {
        p_item->PrintItem(p_fout);
        fprintf(p_fout," ");
        I++;
    }
    fprintf(p_fout,"\n");

    return(true);
}

//------------------------------------------------------------------------------

int CResultFile::GetNumberOfCollectedData(void)
{
    return(RecordNumber);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CResultFile::WriteStatistics(void)
{
    if( StatFileName == NULL ) {
        ES_ERROR("file for statistics was not specified");
        return(false);
    }

    ResultMutex.Lock();

    FILE* p_stat = fopen(StatFileName,"w");
    if( p_stat == NULL ) {
        CSmallString error;
        error << "unable to open file for statistics '"
              << StatFileName << "' (" << strerror(errno) << ")";
        ES_ERROR(error);
        ResultMutex.Unlock();
        return(false);
    }

    fprintf(p_stat,"# The statistics of results collected by result-server.\n");
    fprintf(p_stat,"#\n");
    fprintf(p_stat,"# Number of data points: %d\n",RecordNumber);
    fprintf(p_stat,"#\n");
    fprintf(p_stat,"# Data item description:\n");
    fprintf(p_stat,"# ----------------------\n");
    fprintf(p_stat,"#\n");

    // write data description
    if( PrintItemSpecs(p_stat) == false ) {
        ES_ERROR("unable to write data specification");
        fclose(p_stat);
        ResultMutex.Unlock();
        return(false);
    }

    fprintf(p_stat,"#\n");
    fprintf(p_stat,"# Data statistics:\n");
    fprintf(p_stat,"# ----------------\n");

    // write header
    fprintf(p_stat,"#\n");
    fprintf(p_stat,"#  Type   ");

    // write names
    CSimpleIterator<CResultItem> I(Items);
    CResultItem* p_item;

    while( (p_item = I.Current()) != NULL ) {
        unsigned int max_len = 0;
        if( max_len < p_item->Name.GetLength() ) max_len = p_item->Name.GetLength();
        if( max_len < p_item->Format.GetRecordLength() ) {
            max_len = p_item->Format.GetRecordLength();
        }
        for(unsigned int i=0; i < (max_len-p_item->Name.GetLength())/2; i++) {
            fprintf(p_stat," ");
        }
        fprintf(p_stat,"%s",(const char*)p_item->Name);
        for(unsigned int i=0; i < (max_len-p_item->Name.GetLength())/2; i++) {
            fprintf(p_stat," ");
        }
        if( (max_len-p_item->Name.GetLength()) % 2 != 0 ) {
            fprintf(p_stat," ");
        }
        fprintf(p_stat," ");
        I++;
    }
    fprintf(p_stat,"\n");

    // lines
    fprintf(p_stat,"# ------- ");

    I.SetToBegin();

    while( (p_item = I.Current()) != NULL ) {
        unsigned int max_len = 0;
        if( max_len < p_item->Name.GetLength() ) max_len = p_item->Name.GetLength();
        if( max_len < p_item->Format.GetRecordLength() ) {
            max_len = p_item->Format.GetRecordLength();
        }
        for(unsigned int i=0; i < max_len; i++) fprintf(p_stat,"-");
        fprintf(p_stat," ");
        I++;
    }
    fprintf(p_stat,"\n");

    // do we have data for statistics?
    if( RecordNumber <= 0 ) {
        fprintf(p_stat,"#  no data for statistics");
        fclose(p_stat);
        ResultMutex.Unlock();
        return(true);
    }

    // average
    fprintf(p_stat,"  <X>     ");
    I.SetToBegin();

    while( (p_item = I.Current()) != NULL ) {
        p_item->PrintItemAve(p_stat,RecordNumber);
        fprintf(p_stat," ");
        I++;
    }
    fprintf(p_stat,"\n");

    // standard deviation of X
    fprintf(p_stat,"  s(X)    ");
    I.SetToBegin();

    while( (p_item = I.Current()) != NULL ) {
        p_item->PrintItemSDev(p_stat,RecordNumber);
        fprintf(p_stat," ");
        I++;
    }
    fprintf(p_stat,"\n");

    // standard deviation of <X>
    fprintf(p_stat,"  s(<X>)  ");
    I.SetToBegin();

    while( (p_item = I.Current()) != NULL ) {
        p_item->PrintItemSDevAve(p_stat,RecordNumber);
        fprintf(p_stat," ");
        I++;
    }
    fprintf(p_stat,"\n");


    fclose(p_stat);
    ResultMutex.Unlock();

    return(true);
}

//------------------------------------------------------------------------------

bool CResultFile::LoadStatistics(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        ES_ERROR("p_ele is NULL");
        return(false);
    }

    if( p_ele->GetName() != "STAT" ) {
        ES_ERROR("p_ele is not STAT");
        return(false);
    }

    if( p_ele->GetAttribute("num_of_records",RecordNumber) == false ) {
        ES_ERROR("unable to get number of records");
        return(false);
    }

    Items.RemoveAll();

    CXMLElement* p_iele = p_ele->GetFirstChildElement("ITEM");

    while( p_iele != NULL ) {
        CResultItem* p_item;

        try {
            p_item = new CResultItem;
        } catch(...) {
            ES_ERROR("unable to allocate result item");
            Items.RemoveAll();
            ResultMutex.Unlock();
            return(false);
        }

        if( p_item->LoadStatistics(p_iele) == false ) {
            ES_ERROR("unable to load item");
            Items.RemoveAll();
            return(false);
        }

        Items.InsertToEnd(p_item,0,true);

        p_iele = p_iele->GetNextSiblingElement("ITEM");
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CResultFile::WriteStatistics(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        ES_ERROR("p_ele is NULL");
        return(false);
    }

    if( p_ele->GetName() != "STAT" ) {
        ES_ERROR("p_ele is not STAT");
        return(false);
    }

    p_ele->SetAttribute("num_of_records",RecordNumber);

    CSimpleIterator<CResultItem> I(Items);
    CResultItem* p_item;

    while( (p_item = I.Current()) != NULL ) {
        CXMLElement* p_iele = p_ele->CreateChildElement("ITEM");
        if( p_iele == NULL ) {
            ES_ERROR("unable to create ITEM");
            return(false);
        }
        if( p_item->SaveStatistics(p_iele) == false ) {
            ES_ERROR("unable to save item statistics");
            return(false);
        }
        I++;
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CResultFile::FlushData(void)
{
    // flush data if they exist
    if(ResultFile != NULL) fflush(ResultFile);

    if(StatFileName != NULL) {
        if( WriteStatistics() == false ) {
            ES_ERROR("unable to write statistics");
            ResultMutex.Unlock();
            return(false);
        }
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CResultItem* CResultFile::FindItem(const CSmallString& name)
{
    CSimpleIterator<CResultItem> I(Items);
    CResultItem* p_item;

    while( (p_item = I.Current()) != NULL ) {
        if( p_item->Name == name ) {
            return(p_item);
        }
        I++;
    }

    return(NULL);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

