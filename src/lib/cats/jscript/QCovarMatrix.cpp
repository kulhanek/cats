// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <iostream>
#include <QScriptEngine>
#include <QCovarMatrix.hpp>
#include <moc_QCovarMatrix.cpp>
#include <QTopology.hpp>
#include <QSnapshot.hpp>
#include <SciLapack.hpp>
#include <iomanip>
#include <fstream>
#include <ErrorSystem.hpp>
#include <QSimpleVector.hpp>

//------------------------------------------------------------------------------

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QCovarMatrix::Register(QScriptEngine& engine)
{
    QScriptValue ctor = engine.newFunction(QCovarMatrix::New);
    QScriptValue metaObject = engine.newQMetaObject(&QCovarMatrix::staticMetaObject, ctor);
    engine.globalObject().setProperty("CovarMatrix", metaObject);
}

//------------------------------------------------------------------------------

QScriptValue QCovarMatrix::New(QScriptContext *context,
                            QScriptEngine *engine)
{
    if( context->isCalledAsConstructor() == false ) {
        context->throwError("CovarMatrix() can be called only as a constructor\n"
                            "usage: new CovarMatrix(topology)");
        return(engine->undefinedValue());
    }

    if( (context->argumentCount() > 2) || (context->argumentCount() < 1) ) {
        context->throwError("CovarMatrix() - illegal number of arguments\n"
                            "usage: new CovarMatrix(topology)");
        return(engine->undefinedValue());
    }

    QTopology* p_qtop =  dynamic_cast<QTopology*>(context->argument(0).toQObject());
    if( p_qtop == NULL ) {
        context->throwError("CovarMatrix() - first parameter is not Topology");
        return(engine->undefinedValue());
    }

    QCovarMatrix* p_obj = new QCovarMatrix(p_qtop);
    return(engine->newQObject(p_obj, QScriptEngine::ScriptOwnership));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QCovarMatrix::QCovarMatrix(QTopology* p_parent)
    : QCATsScriptable("CovarMatrix")
{
    int natoms = p_parent->Topology.AtomList.GetNumberOfAtoms();
    Matrix.CreateMatrix(3*natoms,3*natoms);
    EigenValues.CreateVector(3*natoms);
    SamplingMode = false;
    NumOfSamples = 0;
    NumOfVectors = 0;
    RefHelper.CreateVector(3*natoms);
    CrdHelper.CreateVector(3*natoms);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int QCovarMatrix::GetDimension(void) const
{
    return(Matrix.GetNumberOfColumns());
}

//------------------------------------------------------------------------------

int QCovarMatrix::GetNumOfVectors(void) const
{
    return(NumOfVectors);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QCovarMatrix::begin(QObject* p_qref)
{
    if( argumentCount() != 1 ) {
        context()->throwError("CovarMatrix::begin(ref) - illegal number of arguments, only one is expected");
        return;
    }
    if( SamplingMode == true ){
        context()->throwError("CovarMatrix::begin(ref) - already in sampling mode");
        return;
    }
    QSnapshot* p_ref = dynamic_cast<QSnapshot*>(p_qref);
    if( p_ref == NULL ){
        context()->throwError("CovarMatrix::addSample(ref,crd) - ref must be snapshot");
        return;
    }
    if( GetDimension() != 3*p_ref->Restart.GetNumberOfAtoms() ){
        CSmallString error;
        error << "CovarMatrix::begin(ref) - inconsistent dimensions, ref (";
        error << p_ref->Restart.GetNumberOfAtoms() << "), matrix (" << GetDimension() << ")";
        context()->throwError(QString(error));
        return;
    }
    for(int i=0; i < p_ref->Restart.GetNumberOfAtoms(); i++ ){
        CPoint pos = p_ref->Restart.GetPosition(i);
        for(int j=0; j < 3; j++){
            RefHelper[i*3+j] = pos[j];
        }
    }

    CrdHelper.SetZero();
    Matrix.SetZero();
    SamplingMode = true;
    NumOfSamples = 0;
    NumOfVectors = 0;
}

//------------------------------------------------------------------------------

void QCovarMatrix::addSample(QObject* p_qcrd)
{
    if( argumentCount() != 1 ) {
        context()->throwError("CovarMatrix::addSample(crd) - illegal number of arguments, only one argument is axpected");
        return;
    }
    if( SamplingMode == false ){
        context()->throwError("CovarMatrix::addSample(crd) - not in sampling mode");
        return;
    }

    QSnapshot* p_crd = dynamic_cast<QSnapshot*>(p_qcrd);
    if( p_crd == NULL ){
        context()->throwError("CovarMatrix::addSample(crd) - crd must be snapshot");
        return;
    }

    if( GetDimension() != 3*p_crd->Restart.GetNumberOfAtoms() ){
        CSmallString error;
        error << "CovarMatrix::addSample(crd) - inconsistent dimensions, crd (";
        error << p_crd->Restart.GetNumberOfAtoms() << "), matrix (" << GetDimension() << ")";
        context()->throwError(QString(error));
        return;
    }
    for(int i=0; i < p_crd->Restart.GetNumberOfAtoms(); i++ ){
        CPoint pos = p_crd->Restart.GetPosition(i);
        for(int j=0; j < 3; j++){
            CrdHelper[i*3+j] = pos[j] - RefHelper[i*3+j];
        }
    }

    // highly optimized code :-)
    // matrix is symmetric
    int dim = GetDimension();
    double* p_d1 = CrdHelper.GetRawDataField();
    double* p_m = Matrix.GetRawDataField();
    for(int i=0; i < dim; i++){
        double v1 = *p_d1;
        p_d1++;
        double* p_d2 = CrdHelper.GetRawDataField();
        for(int j=0; j < dim; j++){
            double v2 = *p_d2;
            p_d2++;
            double m = *p_m;
            m += v1*v2;
            *p_m = m;
            p_m++;
        }
    }

    NumOfSamples++;
}

//------------------------------------------------------------------------------

void QCovarMatrix::finish(void)
{
    if( argumentCount() != 0 ) {
        context()->throwError("CovarMatrix::finish() - illegal number of arguments, none is expected");
        return;
    }
    if( SamplingMode == false ){
        context()->throwError("CovarMatrix::begin() - not in sampling mode");
        return;
    }

    if( NumOfSamples > 0 ){
        for(unsigned int i=0; i < Matrix.GetNumberOfRows(); i++){
            for(unsigned int j=0; j < Matrix.GetNumberOfRows(); j++){
                double value = Matrix[i][j];
                value /= NumOfSamples;
                Matrix[i][j] = value;
            }
        }
    }

    SamplingMode = false;
}

//------------------------------------------------------------------------------

bool QCovarMatrix::diagonalize(void)
{
    if( argumentCount() != 0 ) {
        context()->throwError("CovarMatrix::diagonalize() - illegal number of arguments, none is expected");
        return(false);
    }
    if( SamplingMode == true ){
        context()->throwError("CovarMatrix::diagonalize() - still in sampling mode");
        return(false);
    }
    if( NumOfVectors > 0 ){
        context()->throwError("CovarMatrix::diagonalize() - already diagonalized");
        return(false);
    }

    NumOfVectors = GetDimension();

    // syev(char jobz,char uplo,CFortranMatrix& a,CVector& w);
    int result = CSciLapack::syev('V','U',Matrix,EigenValues);

    return(result == 0);
}

//------------------------------------------------------------------------------

double QCovarMatrix::projectSnapshot(QObject* p_qsnap,int vector)
{
    if( argumentCount() != 2 ) {
        context()->throwError("CovarMatrix::projectSnapshot(crd,vector) - illegal number of arguments, two arguments are axpected");
        return(0.0);
    }
    if( SamplingMode == true ){
        context()->throwError("CovarMatrix::projectSnapshot(crd,vector) - still in sampling mode");
        return(0.0);
    }

    QSnapshot* p_crd = dynamic_cast<QSnapshot*>(p_qsnap);
    if( p_crd == NULL ){
        context()->throwError("CovarMatrix::projectSnapshot(crd,vector) - crd must be snapshot");
        return(0.0);
    }

    int natoms = p_crd->Restart.GetNumberOfAtoms();
    int ndim = GetDimension();
    if( ndim != 3*natoms ){
        CSmallString error;
        error << "CovarMatrix::projectSnapshot(crd,vector) - inconsistent dimensions, crd (";
        error << 3*natoms << "), matrix (" << ndim << ")";
        context()->throwError(QString(error));
        return(0.0);
    }

    int nvecs = GetNumOfVectors();
    if( (vector < 1) || (vector > nvecs) ){
        CSmallString error;
        error << "CovarMatrix::projectSnapshot(crd,vector) - vector index is out-of-range, index (";
        error << vector << "), nvecs (" << nvecs << ")";
        context()->throwError(QString(error));
        return(0.0);
    }
    // revers and fix to zero-indexing
    vector = ndim - vector;

    for(int i=0; i < natoms; i++ ){
        CPoint pos = p_crd->Restart.GetPosition(i);
        for(int j=0; j < 3; j++){
            CrdHelper[i*3+j] = pos[j] - RefHelper[i*3+j];
        }
    }
    double proj = 0;
    for(int i=0; i < ndim; i++ ){
        proj += CrdHelper[i]*Matrix[i][vector];
    }
    return(proj);
}

//------------------------------------------------------------------------------

void QCovarMatrix::filterSnapshot(QObject* p_qsnap,int vector1,int vector2)
{
    if( argumentCount() != 3 ) {
        context()->throwError("CovarMatrix::filterSnapshot(crd,vect1,vect2) - illegal number of arguments, three arguments are axpected");
        return;
    }
    if( SamplingMode == true ){
        context()->throwError("CovarMatrix::filterSnapshot(crd,vect1,vect2) - still in sampling mode");
        return;
    }
    QSnapshot* p_crd = dynamic_cast<QSnapshot*>(p_qsnap);
    if( p_crd == NULL ){
        context()->throwError("CovarMatrix::filterSnapshot(crd,vect1,vect2) - crd must be snapshot");
        return;
    }

    int natoms = p_crd->Restart.GetNumberOfAtoms();
    int ndim = GetDimension();

    if( ndim != 3*natoms ){
        CSmallString error;
        error << "CovarMatrix::filterSnapshot(crd,vect1,vect2) - inconsistent dimensions, crd (";
        error << 3*natoms << "), matrix (" << ndim << ")";
        context()->throwError(QString(error));
        return;
    }

    int nvecs = GetNumOfVectors();
    if( (vector1 < 1) || (vector1 > nvecs) ){
        CSmallString error;
        error << "CovarMatrix::filterSnapshot(crd,vect1,vect2) - vect1 is out-of-range, index (";
        error << vector1 << "), nvecs (" << nvecs << ")";
        context()->throwError(QString(error));
        return;
    }

    if( (vector2 < 1) || (vector2 > nvecs) ){
        CSmallString error;
        error << "CovarMatrix::projectSnapshot(crd,vect1,vect2) - vect2 is out-of-range, index (";
        error << vector2 << "), nvecs (" << nvecs << ")";
        context()->throwError(QString(error));
        return;
    }

    if( vector2 < vector1 ){
        CSmallString error;
        error << "CovarMatrix::projectSnapshot(crd,vect1,vect2) - vect2 has to be larger than or equal to vect1";
        context()->throwError(QString(error));
        return;
    }

    // revers and fix to zero-indexing
    vector1 = ndim - vector1;
    vector2 = ndim - vector2;

    for(int i=0; i < natoms; i++ ){
        CPoint pos = p_crd->Restart.GetPosition(i);
        for(int j=0; j < 3; j++){
            CrdHelper[i*3+j] = pos[j] - RefHelper[i*3+j];
        }
    }

    // calculate projections
    vector<double> projections;
    for(int vect = vector2; vect <= vector1; vect++){
        double proj = 0;
        for(int i=0; i < ndim; i++ ){
            proj += CrdHelper[i]*Matrix[i][vect];
        }
        projections.push_back(proj);
    }

    for(int i=0; i < ndim; i++ ){
        int proji = 0;
        CrdHelper[i] = RefHelper[i];
        for(int vect = vector2; vect <= vector1; vect++){
            CrdHelper[i] +=  projections[proji]*Matrix[i][vect];
            proji++;
        }
    }

    for(int i=0; i < natoms; i++ ){
        CPoint pos;
        for(int j=0; j < 3; j++){
            pos[j] = CrdHelper[i*3+j];
        }
        p_crd->Restart.SetPosition(i,pos);
    }

}

//------------------------------------------------------------------------------

void QCovarMatrix::updateSnapshot(QObject* p_qsnap,int vector,double proj)
{
    if( argumentCount() != 3 ) {
        context()->throwError("CovarMatrix::updateSnapshot(crd,vector,proj) - illegal number of arguments, three arguments are axpected");
        return;
    }
    if( SamplingMode == true ){
        context()->throwError("CovarMatrix::updateSnapshot(crd,vector,proj) - still in sampling mode");
        return;
    }

    QSnapshot* p_crd = dynamic_cast<QSnapshot*>(p_qsnap);
    if( p_crd == NULL ){
        context()->throwError("CovarMatrix::updateSnapshot(crd,vector,proj) - crd must be snapshot");
        return;
    }

    int natoms = p_crd->Restart.GetNumberOfAtoms();
    int ndim = GetDimension();

    if( ndim != 3*natoms ){
        CSmallString error;
        error << "CovarMatrix::updateSnapshot(crd,vector,proj) - inconsistent dimensions, crd (";
        error << 3*natoms << "), matrix (" << ndim << ")";
        context()->throwError(QString(error));
        return;
    }

    int nvecs = GetNumOfVectors();

    if( (vector < 1) || (vector > nvecs) ){
        CSmallString error;
        error << "CovarMatrix::updateSnapshot(crd,vector,proj) - vector index is out-of-range, index (";
        error << vector << "), nvecs (" << nvecs << ")";
        context()->throwError(QString(error));
        return;
    }

    // revers and fix to zero-indexing
    vector = ndim - vector;

    for(int i=0; i < ndim; i++ ){
        CrdHelper[i] = RefHelper[i] + proj*Matrix[i][vector];
    }

    for(int i=0; i < natoms; i++ ){
        CPoint pos = p_crd->Restart.GetPosition(i);
        for(int j=0; j < 3; j++){
            pos[j] += CrdHelper[i*3+j];
        }
        p_crd->Restart.SetPosition(i,pos);
    }
}

//------------------------------------------------------------------------------

QScriptValue QCovarMatrix::getEigenVector(int index)
{
    if( argumentCount() != 1 ) {
        context()->throwError("CovarMatrix::getEigenVector(index) - illegal number of arguments, only one is expected");
        return(0.0);
    }
    if( SamplingMode == true ){
        context()->throwError("CovarMatrix::getEigenVector(index) - still in sampling mode");
        return(0.0);
    }

    int nvecs = GetNumOfVectors();
    int ndim = GetDimension();

    if( (index < 1) || (index > nvecs) ){
        CSmallString error;
        error << "CovarMatrix::getEigenVector(index) - vector index is out-of-range, index (";
        error << index << "), nvecs (" << nvecs << ")";
        context()->throwError(QString(error));
        return(0.0);
    }

    // revers and fix to zero-indexing
    index = ndim - index;

    QSimpleVector* p_obj = new QSimpleVector(ndim);
    for(int i=0; i < ndim; i++){
        p_obj->Vector[i] = Matrix[i][index];
    }

    QScriptValue obj = engine()->newQObject(p_obj, QScriptEngine::ScriptOwnership);
    return(obj);
}

//------------------------------------------------------------------------------

double QCovarMatrix::getEigenValue(int index)
{
    if( argumentCount() != 1 ) {
        context()->throwError("CovarMatrix::getEigenValue(index) - illegal number of arguments, only one is expected");
        return(0.0);
    }
    if( SamplingMode == true ){
        context()->throwError("CovarMatrix::getEigenValue(index) - still in sampling mode");
        return(0.0);
    }

    int nvecs = GetNumOfVectors();
    int ndim = GetDimension();

    if( (index < 1) || (index > nvecs) ){
        CSmallString error;
        error << "CovarMatrix::getEigenValue(index) - vector index is out-of-range, index (";
        error << index << "), nvecs (" << nvecs << ")";
        context()->throwError(QString(error));
        return(0.0);
    }

    // revers and fix to zero-indexing
    index = ndim - index;

    return(EigenValues[index]);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QCovarMatrix::printEigenValues(void)
{
    if( argumentCount() > 1 ) {
        context()->throwError("CovarMatrix::printEigenValues() - illegal number of arguments, none or one is expected");
        return;
    }
    if( SamplingMode == true ){
        context()->throwError("CovarMatrix::printEigenValues() - still in sampling mode");
        return;
    }

    int nvec = GetNumOfVectors();
    int num = nvec;
    if( argumentCount() == 1 ){
        if( argument(0).isNumber() == 0 ){
            context()->throwError("CovarMatrix::printEigenValues(number) - provided argument must be integer number");
            return;
        }
        num = argument(0).toInteger();
        if( (num < 1) || (num > nvec) ){
            CSmallString error;
            error << "CovarMatrix::printEigenValues(number) - number of eigenvalues is out-of-range, number (";
            error << num << "), nvec (" << nvec << ")";
            context()->throwError(QString(error));
            return;
        }
    }

    SaveEigenValues(cout,num);
}

//------------------------------------------------------------------------------

bool QCovarMatrix::saveEigenValues(const QString& name)
{
    if( argumentCount() > 2 ) {
        context()->throwError("CovarMatrix::saveEigenValues(name[,nvecs]) - illegal number of arguments, one or two are expected");
        return(false);
    }
    if( SamplingMode == true ){
        context()->throwError("CovarMatrix::saveEigenValues(name[,nvecs]) - still in sampling mode");
        return(false);
    }

    int nvec = GetNumOfVectors();
    int num = nvec;
    if( argumentCount() == 2 ){
        if( argument(1).isNumber() == 0 ){
            context()->throwError("CovarMatrix::saveEigenValues(name,nvecs) - provided argument must be integer number");
            return(false);
        }
        num = argument(1).toInteger();
        if( (num < 1) || (num > nvec) ){
            CSmallString error;
            error << "CovarMatrix::saveEigenValues(name,nvecs) - number of eigenvalues is out-of-range, number (";
            error << num << "), nvec (" << nvec << ")";
            context()->throwError(QString(error));
            return(false);
        }
    }

    ofstream fout;
    fout.open(name.toStdString().c_str());
    if( ! fout ) return(false);
    SaveEigenValues(fout,num);
    return(fout.good());
}

//------------------------------------------------------------------------------

bool QCovarMatrix::load(const QString& name)
{
    if( argumentCount() > 1 ) {
        context()->throwError("CovarMatrix::load(name) - illegal number of arguments, only one is expected");
        return(false);
    }
    if( SamplingMode == true ){
        context()->throwError("CovarMatrix::load(name) - still in sampling mode");
        return(false);
    }

    ifstream fout;
    fout.open(name.toStdString().c_str());
    if( ! fout ) return(false);
    bool result = LoadAll(fout);
    return(result);
}

//------------------------------------------------------------------------------

bool QCovarMatrix::save(const QString& name)
{
    if( argumentCount() > 1 ) {
        context()->throwError("CovarMatrix::save(name) - illegal number of arguments, only one is expected");
        return(false);
    }
    if( SamplingMode == true ){
        context()->throwError("CovarMatrix::save(name) - still in sampling mode");
        return(false);
    }

    ofstream fout;
    fout.open(name.toStdString().c_str());
    if( ! fout ) return(false);
    SaveAll(fout);
    return(fout.good());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QCovarMatrix::SaveEigenValues(ostream& vout,int num)
{
    vout << "#   Index        EigenValue     " << endl;
    vout << "# --------- --------------------" << endl;

    int ndim = GetDimension();

    for(int i=0; i < num; i++){
        vout << right << setw(11)  << i+1 << " " << right << fixed << setw(20) << setprecision(10) << EigenValues[ndim-i-1] << endl;
    }
}

//------------------------------------------------------------------------------

bool QCovarMatrix::LoadAll(std::istream& vin)
{
    string line;
    bool found = false;

    // read header -------------------------------
    while( vin ){
        getline(vin,line);
        if( line == "[EDA]" ){
            found = true;
            break;
        }
    }
    if( found == false ){
        ES_ERROR("[EDA] header not found");
        return(false);
    }
    int ndim, nvec, nsamples;
    vin >> ndim >> nvec >> nsamples;

    NumOfSamples = nsamples;
    NumOfVectors = nvec;

    if( ndim != GetDimension() ){
        CSmallString error;
        error << "dimensions mismatch: ndim (" << GetDimension() << "), ndim file (" << ndim << ")";
        ES_ERROR(error);
        return(false);
    }
    if( nvec > ndim ){
        CSmallString error;
        error << "nvec cannot be larger than ndim: ndim (" << ndim << "), nvec (" << nvec << ")";
        ES_ERROR(error);
        return(false);
    }

    // load reference ----------------------------
    while( vin ){
        getline(vin,line);
        if( line == "[reference]" ){
            found = true;
            break;
        }
    }
    if( found == false ){
        ES_ERROR("[reference] header not found");
        return(false);
    }

    for(int i=0; i < GetDimension(); i ++){
        vin >> RefHelper[i];
    }

    // load vectors -----------------------------
    for(int vector=1; vector <= nvec; vector++){
        while( vin ){
            getline(vin,line);
            if( line == "[vector]" ){
                found = true;
                break;
            }
        }
        if( found == false ){
            CSmallString error;
            error << "[vector] header not found for vector " << vector;
            ES_ERROR(error);
            return(false);
        }
        int rvector = ndim - vector;
        int tmp;
        vin >> tmp >> EigenValues[rvector];
        for(int i=0; i < ndim; i ++){
            vin >> Matrix[i][rvector];
        }
    }

    return( vin.good() );
}

//------------------------------------------------------------------------------

void QCovarMatrix::SaveAll(std::ostream& vout)
{
    int ndim = GetDimension();
    int nvec = GetNumOfVectors();

    vout << "[EDA]" << endl;
    vout << setw(6) << ndim << " " << setw(6) << nvec << " " << setw(8) << NumOfSamples << endl;

    // reference structure
    vout << "[reference]" << endl;
    vout << fixed << setprecision(9);
    int i;
    for(i=0; i < ndim; i ++){
        vout << setw(16) << RefHelper[i];
        if( i % 3 == 2 ){
            vout << endl;
        } else {
            vout << " ";
        }
    }
    if( i % 3 != 0 ) vout << endl;

    // eigenvectors
    for(int vector=1; vector <= nvec; vector++){
        vout << "[vector]" << endl;
        vout << setw(6) << vector << " ";
        int rvector = ndim - vector;
        vout << setw(20) << setprecision(13) << EigenValues[rvector] << endl;
        for(i=0; i < ndim; i ++){
            vout << setw(16) << Matrix[i][rvector];
            if( i % 3 == 2 ){
                vout << endl;
            } else {
                vout << " ";
            }
        }
        if( i % 3 != 0 ) vout << endl;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



