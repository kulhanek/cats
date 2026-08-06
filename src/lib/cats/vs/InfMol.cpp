// =============================================================================
// ChemInfo - Chemoinformatics Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <InfMol.hpp>
#include <ErrorSystem.hpp>
#include <fstream>
#include <unordered_set>

// openbabel
#include <openbabel/mol.h>
#include <openbabel/obiter.h>
#include <openbabel/atom.h>
#include <openbabel/bond.h>
#include <openbabel/residue.h>
#include <openbabel/obconversion.h>

using namespace std;
using namespace OpenBabel;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CInfMol::CInfMol(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CInfMol::ReadMol(const CSmallString& name,const CSmallString& format,int index)
{
    ifstream ifs;

    if(name != "-") {
        ifs.open(name);

        if(! ifs) {
            CSmallString error;
            error << "unable to open molecule: file " << name << "(format: " << format << ")";
            ES_ERROR(error);
            ifs.close();
            return(false);
        }
    }

// read molecule from file to babel internal
    OBConversion   conv(&ifs, NULL);

    if(name == "-") {
        conv.SetInStream(&cin);
    }

    OpenBabel::OBFormat* obFormat;

    if(format == "auto") {
        obFormat = conv.FormatFromExt(name);
    } else {
        obFormat = conv.FindFormat(format);
    }

    if(! conv.SetInFormat(obFormat)) {
        CSmallString error;
        error << "unable to select molecule format: file " << name << "(format: " << format << ")";
        ES_ERROR(error);
        ifs.close();
        return(false);
    }

    for(int i=1; i < index; i++){
        OBMol tmpmol;
        if( conv.Read(&tmpmol) == false ){
            CSmallString error;
            error << "unable to read molecule: file " << name << "(format: " << format << ") / index: " << i ;
            ES_ERROR(error);
            ifs.close();
            return(false);
        }
    }

    if(! conv.Read(this)) {
        CSmallString error;
        error << "unable to read molecule: file " << name << "(format: " << format << ")";
        ES_ERROR(error);
        ifs.close();
        return(false);
    }

    ifs.close();

    ConnectTheDots();

    return(true);
}

//------------------------------------------------------------------------------

bool CInfMol::WriteMol(const CSmallString& name,const CSmallString& format,const CSmallString opts)
{
    ofstream ofs(name);

    if(! ofs) {
        CSmallString error;
        error << "unable to open molecule file: '" << name << "' (format: " << format << ")";
        ES_ERROR(error);
        ofs.close();
        return(false);
    }

// write molecule to file
    OBConversion   conv(NULL, &ofs);

    OpenBabel::OBFormat* obFormat;
    if(format == "auto") {
        obFormat = conv.FormatFromExt(name);
    } else {
        obFormat = conv.FindFormat(format);
    }

    if(! conv.SetOutFormat(obFormat)) {
        CSmallString error;
        error << "unable to select molecule format: file '" << name << "' (format: " << format << ")";
        ES_ERROR(error);
        ofs.close();
        return(false);
    }

    if( opts != NULL ){
        for(size_t i=0; i < opts.GetLength(); i++){
            CSmallString opt(opts.GetBuffer()[i]);
            conv.AddOption(opt,OBConversion::OUTOPTIONS);
        }
    }

    if(! conv.Write(this)) {
        CSmallString error;
        error << "unable to write molecule: file '" << name << "' (format: " << format << ")";
        ES_ERROR(error);
        ofs.close();
        return(false);
    }

    ofs.close();

    return(true);
}

//------------------------------------------------------------------------------

void CInfMol::AlterHydrogens(const CSmallString& mode)
{
    if( mode == "keep" ) return;

    if( mode == "add" ){
        AddHydrogens();
        return;
    }
    if( mode == "addpolar" ){
        AddPolarHydrogens();
        return;
    }
    if( mode == "addnonpolar" ){
        AddNonPolarHydrogens();
        return;
    }

    if( (mode == "delete") || (mode == "remove") ){
        DeleteHydrogens();
        return;
    }
    if( (mode == "deletepolar") || (mode == "removepolar") ){
        DeletePolarHydrogens();
        return;
    }
    if( (mode == "deletenonpolar") || (mode == "removenonpolar") ){
        DeleteNonPolarHydrogens();
        return;
    }

    CSmallString error;
    error << "Unsupported alter hydrogen mode: " << mode;
    RUNTIME_ERROR(error);
}

//------------------------------------------------------------------------------

struct ResiduePairHash
{
    std::size_t operator()(
        const std::pair<const OpenBabel::OBResidue*,
                        const OpenBabel::OBResidue*>& pair) const noexcept
    {
        const auto h1 =
            std::hash<const OpenBabel::OBResidue*>{}(pair.first);
        const auto h2 =
            std::hash<const OpenBabel::OBResidue*>{}(pair.second);

        return h1 ^ (h2 << 1);
    }
};

using ResiduePair =
    std::pair<const OpenBabel::OBResidue*,
              const OpenBabel::OBResidue*>;

ResiduePair MakeOrderedPair(const OpenBabel::OBResidue* first,
                            const OpenBabel::OBResidue* second)
{
    if (std::less<const OpenBabel::OBResidue*>{}(second, first)) {
        std::swap(first, second);
    }

    return {first, second};
}

//------------------------------------------------------------------------------

void CInfMol::RebuildChains(void)
{

    std::vector<OBResidue*> residues;
    residues.reserve(NumResidues());

    FOR_RESIDUES_OF_MOL(residue, *this) {
        residues.push_back(&*residue);
    }

    if (residues.empty()) {
        return;
    }

    /*
     * Record all residue pairs connected by at least one covalent bond.
     * Bonds within a single residue are ignored.
     */
    std::unordered_set<ResiduePair, ResiduePairHash> connectedResidues;

    FOR_BONDS_OF_MOL(bond, *this) {
        OBAtom* beginAtom = bond->GetBeginAtom();
        OBAtom* endAtom   = bond->GetEndAtom();

        if (beginAtom == nullptr || endAtom == nullptr) {
            continue;
        }

        OBResidue* beginResidue = beginAtom->GetResidue();
        OBResidue* endResidue   = endAtom->GetResidue();

        if (beginResidue == nullptr ||
            endResidue == nullptr ||
            beginResidue == endResidue) {
            continue;
        }

        connectedResidues.insert(
            MakeOrderedPair(beginResidue, endResidue));
    }

    char chain = 'A';
    residues.front()->SetChain(chain);

    for (std::size_t i = 1; i < residues.size(); ++i) {
        OBResidue* previous = residues[i - 1];
        OBResidue* current  = residues[i];

        const bool connected =
            connectedResidues.find(
                MakeOrderedPair(previous, current)) !=
            connectedResidues.end();

        if (!connected) {
            if (chain == 'Z') {
                throw std::runtime_error(
                    "ReindexChains: more than 26 chains are required, "
                    "but OBResidue::SetChain() accepts a single character.");
            }

            ++chain;
        }

        current->SetChain(chain);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

