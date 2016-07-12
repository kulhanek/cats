#ifndef QPMFLibH
#define QPMFLibH
// =============================================================================
// CATS - Conversion and Analysis Tools
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

#include <CATsMainHeader.hpp>
#include <QObject>
#include <QScriptValue>
#include <QScriptContext>
#include <QScriptable>
#include <AmberRestart.hpp>
#include <QCATsScriptable.hpp>

//------------------------------------------------------------------------------

class QTopology;
class QSnapshot;

//------------------------------------------------------------------------------

/// PMFLib bridge - only one instance

class CATS_PACKAGE QPMFLib : public QObject, protected QScriptable, protected QCATsScriptable {
    Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    QPMFLib(void);
    static void Register(QScriptEngine& engine);
    static void SetScriptArgument(const QString& arg);

// methods ---------------------------------------------------------------------
public slots:
    /// load PMFLib control file
    /// init(snapshot,ctrlname)
    QScriptValue init(void);
    
    /// finalize PMFLib
    /// finalize()
    QScriptValue finalize(void);    
    
    /// load PMFLib control from string
    /// bool init(snapshot,ctrlstr)
    QScriptValue initByString(void);    

    /// set coordinates
    /// void setCoordinates(snapshot)
    QScriptValue setCoordinates(void);

    /// get number of CVs
    /// int getNumOfCVs();
    QScriptValue getNumOfCVs(void);

    /// get CV value
    /// double getCVValue(name/index);
    QScriptValue getCVValue(void);

    /// get CV name
    /// double getCVName(index);
    QScriptValue getCVName(void);

    /// get CV type
    /// double getCVType(name/index);
    QScriptValue getCVType(void);

// section of private data -----------------------------------------------------
private:
    bool    Initialized;
};

//------------------------------------------------------------------------------

#endif
