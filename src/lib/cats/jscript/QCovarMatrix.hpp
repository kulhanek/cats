#ifndef QCovarMatrixH
#define QCovarMatrixH
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
#include <FortranMatrix.hpp>
#include <Vector.hpp>
#include <QCATsScriptable.hpp>

//------------------------------------------------------------------------------

class QTopology;

//------------------------------------------------------------------------------

/// covariance matrix

class QCovarMatrix : public QObject, protected QScriptable, protected QCATsScriptable {
Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    QCovarMatrix(QTopology* p_parent);
    static QScriptValue New(QScriptContext *context,QScriptEngine *engine);
    static void Register(QScriptEngine& engine);

// properties ------------------------------------------------------------------
    Q_PROPERTY(int dimension READ GetDimension)
    Q_PROPERTY(int nvecs READ GetNumOfVectors)

// methods ---------------------------------------------------------------------
public slots:
// main methods ----------------------------------------------------------------
    /// begin data accumulation
    void begin(QObject* p_qref);

    /// add sample
    void addSample(QObject* p_qcrd);

    /// end data accumulation
    void finish(void);

    /// diagonalize matrix
    bool diagonalize(void);

// projections -----------------------------------------------------------------
    /// project a snapshot to the essential vector
    double projectSnapshot(QObject* p_qsnap,int vector);

    /// filter a snapshot for range of essential vectors
    void filterSnapshot(QObject* p_qsnap,int vector1,int vector2);

    /// add component to snapshot
    void updateSnapshot(QObject* p_qsnap,int vector,double proj);

    /// return given vector as Vector()
    QScriptValue getEigenVector(int index);

    /// return given eigen value
    double getEigenValue(int index);

// info ------------------------------------------------------------------------
    /// print eigenvalues
    void printEigenValues(void);

    /// save eigenvalues
    bool saveEigenValues(const QString& name);

    /// restore data in covar matrix
    bool load(const QString& name);

    /// save result of covar matrix diagonalization
    /** it saves reference structure and obtained eigenvalues and eigen vectors */
    bool save(const QString& name);

// access methods --------------------------------------------------------------
public:
    /// return the size of matrix
    int  GetDimension(void) const;

    /// return number of eigenvectors
    int  GetNumOfVectors(void) const;

    /// save eigen values
    void SaveEigenValues(std::ostream& vout,int num);

    /// load all
    bool LoadAll(std::istream& vin);

    /// save all
    void SaveAll(std::ostream& vout);

// section of private data -----------------------------------------------------
private:
    CFortranMatrix  Matrix;
    CVector         EigenValues;
    bool            SamplingMode;   // object is in sampling mode
    int             NumOfSamples;
    int             NumOfVectors;   // number of calculated vectors
    CVector         RefHelper;
    CVector         CrdHelper;
};

//------------------------------------------------------------------------------

#endif
