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

class QPMFLib : public QObject, protected QScriptable, protected QCATsScriptable {
    Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    QPMFLib(void);
    static void Register(QScriptEngine& engine);
    static void SetScriptArgument(const QString& arg);

// methods ---------------------------------------------------------------------
public slots:
    /// load PMFLib control file
    /// bool init(snapshot,ctrlname)
    QScriptValue init(void);

    /// set coordinates
    /// void setCoordinates(snapshot)
    QScriptValue setCoordinates(void);

    /// get number of CVs
    /// int getNumOfCVs();
    QScriptValue getNumOfCVs(void);

    /// get CV value
    /// double getCVValue(cvname);
    QScriptValue getCVValue(void);

// section of private data -----------------------------------------------------
private:
    bool    Initialized;
};

//------------------------------------------------------------------------------

#endif
