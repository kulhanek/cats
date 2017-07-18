// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <QHistogram.hpp>
#include <moc_QHistogram.cpp>
#include <iomanip>
#include <fstream>
#include <SmallString.hpp>
#include <math.h>
#include <TerminalStr.hpp>

//------------------------------------------------------------------------------

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QHistogram::Register(QScriptEngine& engine)
{
    QScriptValue ctor = engine.newFunction(QHistogram::New);
    QScriptValue metaObject = engine.newQMetaObject(&QHistogram::staticMetaObject, ctor);
    engine.globalObject().setProperty("Histogram", metaObject);
}

//------------------------------------------------------------------------------

QScriptValue QHistogram::New(QScriptContext *context,QScriptEngine *engine)
{
    if( context->isCalledAsConstructor() == false ) {
        context->throwError("new Histogram([histogram]) - it can be called only as a constructor");
        return(engine->undefinedValue());
    }
    if( context->argumentCount() > 1 ) {
        context->throwError("new Histogram([histogram]) - illegal number of arguments, none or one is expected");
        return(engine->undefinedValue());
    }

    QCATsScriptable scriptable("Histogram");
    QScriptValue    value;

// print help ------------------------------------
    if( scriptable.IsHelpRequested() ){
        CTerminalStr sout;
        sout << "Histogram object" << endl;
        sout << endl;
        sout << "Constructors:" << endl;
        sout << "   new Histogram()" << endl;
        sout << "   new Histogram(histogram)" << endl;
        return(scriptable.GetUndefinedValue());
    }

// check arguments -------------------------------
    value = scriptable.IsCalledAsConstructor();
    if( value.isError() ) return(value);

    value = scriptable.CheckNumberOfArguments("[histogram]",0,1);
    if( value.isError() ) return(value);

// create pbject
    QHistogram* p_src = NULL;
    if( scriptable.GetArgumentCount() > 0 ){
        value = scriptable.GetArgAsObject<>("histogram","histogram","Histogram",1,p_src);
        if( value.isError() ) return(value);
    }

    QHistogram* p_hist = new QHistogram();

    if( p_src != NULL ){
        // copy setup from source histogram
        p_hist->Name = p_src->Name;
        p_hist->MinValue = p_src->MinValue;
        p_hist->MaxValue = p_src->MaxValue;
        p_hist->NBins = p_src->NBins;
        p_hist->allocateInternal();
    }

    return(engine->newQObject(p_hist, QScriptEngine::ScriptOwnership));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QHistogram::QHistogram(void)
    : QCATsScriptable("Histogram")
{
    MinValue = 0;
    MaxValue = 10.0;
    NBins = 100;
    NumOfSamples = 0;
    NumOfSamplesWithin = 0;
    allocateInternal();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QHistogram::setMinValue(const QScriptValue& dummy)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Histogram::setMinValue(value)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("value",1);
    if( value.isError() ) return(value);

    double num;
    value = GetArgAsRNumber("value","value",1,num);
    if( value.isError() ) return(value);

    if( num >= MaxValue ){
        CSmallString error;
        error << "min value (" << CSmallString().DoubleToStr(num) << ") must be lower than max value (" << CSmallString().DoubleToStr(MaxValue) <<")";
        return( ThrowError("value",error));
    }

// execute ---------------------------------------
    MinValue = num;
    resetInternal();

    return(true);
}

//------------------------------------------------------------------------------

QScriptValue QHistogram::getMinValue(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double Histogram::getMinValue()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(MinValue);
}

//------------------------------------------------------------------------------

QScriptValue QHistogram::setMaxValue(const QScriptValue& dummy)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Histogram::setMaxValue(value)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("value",1);
    if( value.isError() ) return(value);

    double num;
    value = GetArgAsRNumber("value","value",1,num);
    if( value.isError() ) return(value);

    if( num <= MinValue ){
        CSmallString error;
        error << "max value (" << CSmallString().DoubleToStr(num) << ") must be higher than min value (" << CSmallString().DoubleToStr(MinValue) <<")";
        return( ThrowError("value",error));
    }

// execute ---------------------------------------
    MaxValue = num;
    resetInternal();

    return(true);
}

//------------------------------------------------------------------------------

QScriptValue QHistogram::getMaxValue(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double Histogram::getMaxValue()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(MaxValue);
}

//------------------------------------------------------------------------------

QScriptValue QHistogram::setNumOfBins(const QScriptValue& dummy)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Histogram::setNumOfBins(value)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("value",1);
    if( value.isError() ) return(value);

    int num;
    value = GetArgAsInt("value","value",1,num);
    if( value.isError() ) return(value);

    if( num < 0 ){
        return( ThrowError("value","number of bins must be larger than or equal to zero"));
    }

// execute ---------------------------------------
    NBins = num;
    allocateInternal();

    return(true);
}

//------------------------------------------------------------------------------

QScriptValue QHistogram::getNumOfBins(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int Histogram::getNumOfBins()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(NBins);
}

//------------------------------------------------------------------------------

void QHistogram::allocateInternal(void)
{
    Histogram.resize(NBins);
    DataN.resize(NBins);
    DataSum.resize(NBins);
    Data2Sum.resize(NBins);
    resetInternal();
}

//------------------------------------------------------------------------------

void QHistogram::releaseInternal(void)
{
    Histogram.clear();
    DataN.clear();
    DataSum.clear();
    Data2Sum.clear();
    resetInternal();
}

//------------------------------------------------------------------------------

void QHistogram::resetInternal(void)
{
    NumOfSamples = 0;
    NumOfSamplesWithin = 0;
    for(size_t i=0; i < Histogram.size(); i++){
        Histogram[i] = 0.0;
    }
    for(size_t i=0; i < DataN.size(); i++){
        DataN[i] = 0.0;
    }
    for(size_t i=0; i < DataSum.size(); i++){
        DataSum[i] = 0.0;
    }
    for(size_t i=0; i < Data2Sum.size(); i++){
        Data2Sum[i] = 0.0;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QHistogram::setName(const QScriptValue& dummy)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: LinStat::setName(name)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("name",1);
    if( value.isError() ) return(value);

    QString name;
    value = GetArgAsString("name","name",1,name);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    Name = name;
    return(true);
}

//------------------------------------------------------------------------------

QScriptValue QHistogram::getName(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: string Histogram::getName()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(Name);
}

//------------------------------------------------------------------------------

QScriptValue QHistogram::printInfo(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Histogram::printInfo()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    cout << "=== Histogram" << endl;
    save(cout);   
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QHistogram::addSample(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool Histogram::addSample(value[,extra])" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("value[,extra]",1,2);
    if( value.isError() ) return(value);

    double num;
    value = GetArgAsRNumber("value[,extra]","value",1,num);
    if( value.isError() ) return(value);

    double extra;
    if( GetArgumentCount() > 1 ){
        value = GetArgAsRNumber("value,extra","extra",2,extra);
        if( value.isError() ) return(value);
    }


// execute ---------------------------------------
    if( NBins <= 0 ){
        return( ThrowError("value[,extra]","number of bins is zero"));
    }

    NumOfSamples++;
    if( (num < MinValue) || (num >= MaxValue) ) return(false);
    NumOfSamplesWithin++;

    // calculate index
    double dch = (MaxValue - MinValue)/NBins;
    if( dch <= 0 ){
        return( ThrowError("value[,extra]","bin width is zero or negative value"));
    }
    int index = (num - MinValue)/dch;
    if( (index < 0) || (index >= static_cast<int>(Histogram.size())) ) return(false);

    // increment value
    Histogram[index] += 1;

    if( GetArgumentCount() > 1 ){
        // extra data
        DataN[index] += 1;
        DataSum[index] += extra;
        Data2Sum[index] += extra*extra;
    }

    return(true);
}

//------------------------------------------------------------------------------

QScriptValue QHistogram::multBy(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Histogram::multBy(value)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("value",1);
    if( value.isError() ) return(value);

    double factor;
    value = GetArgAsRNumber("value","value",1,factor);

// execute ---------------------------------------
    for(size_t i=0; i < Histogram.size(); i++){
        Histogram[i] *= factor;
    }

    return(true);
}

//------------------------------------------------------------------------------

QScriptValue QHistogram::normalize(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Histogram::normalize([key1,...])" << endl;
        sout << "       within" << endl;
        return(false);
    }

// check arguments -------------------------------
    bool within = IsArgumentKeySelected("within");
    value = CheckArgumentsUsage("[key1,...]");
    if( value.isError() ) return(value);

// execute ---------------------------------------
    if( NBins <= 0 ){
        return( ThrowError("[key1,...]","no samples with(in)"));
    }

    double factor;
    double dch = (MaxValue-MinValue)/NBins;
    if( within ){
        factor = NumOfSamplesWithin * dch;
    } else {
        factor = NumOfSamples * dch;
    }

    for(size_t i=0; i < Histogram.size(); i++){
        Histogram[i] /= factor;
    }

    return(true);
}

//------------------------------------------------------------------------------

QScriptValue QHistogram::reset(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Histogram::reset()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    resetInternal();
    return(true);
}

//------------------------------------------------------------------------------

QScriptValue QHistogram::getNumOfSamples(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int Histogram::getNumOfSamples()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(NumOfSamples);
}

//------------------------------------------------------------------------------

QScriptValue QHistogram::getNumOfSamplesWithin(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int Histogram::getNumOfSamplesWithin()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(NumOfSamplesWithin);
}

//------------------------------------------------------------------------------

QScriptValue QHistogram::transformToRadial(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Histogram::transformToRadial([key1,...])" << endl;
        sout << "       2D" << endl;
        return(false);
    }

// check arguments -------------------------------
    bool to2d = IsArgumentKeySelected("2D");
    value = CheckArgumentsUsage("[key1,...]");
    if( value.isError() ) return(value);

// execute ---------------------------------------
    for(size_t i=0; i < Histogram.size(); i++){
        double value = (i + 0.5)*(MaxValue-MinValue)/NBins + MinValue;
        double occur = 0.0;
        if( value != 0.0 ){
            occur  = Histogram[i];
            if( ! to2d ){
                occur = occur / (4.0*M_PI*value*value);
            } else {
                occur = occur / (2.0*M_PI*value);
            }
        }
        Histogram[i] = occur;
    }
    return(true);
}

//------------------------------------------------------------------------------

QScriptValue QHistogram::getIntegral(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Histogram::getIntegral(from,to)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("from,to",2);
    if( value.isError() ) return(value);

    double from,to;
    value = GetArgAsRNumber("from,to","from",1,from);
    if( value.isError() ) return(value);
    value = GetArgAsRNumber("from,to","to",2,to);
    if( value.isError() ) return(value);

    if( from >= to ){
        CSmallString error;
        error << "'from' value (" << CSmallString().DoubleToStr(from) << ") must be smaller than 'to' value (" << CSmallString().DoubleToStr(to) <<")";
        return( ThrowError("value",error));
    }

    // calculate index
    double dch = (MaxValue - MinValue)/NBins;
    if( dch <= 0 ){
        return( ThrowError("from,to","bin width is zero or negative value"));
    }
    int index1 = (from - MinValue)/dch;
    if( index1 < 0 ) index1 = 0;
    if( index1 >= static_cast<int>(Histogram.size()) ) index1 = static_cast<int>(Histogram.size()) - 1;

    int index2 = (to - MinValue)/dch;
    if( index2 < 0 ) index2 = 0;
    if( index2 >= static_cast<int>(Histogram.size()) ) index2 = static_cast<int>(Histogram.size()) - 1;

// execute ---------------------------------------
    double intval = 0;
    for(int i=index1;i<=index2; i++){
        intval += Histogram[i]*dch;
    }

    return(intval);
}

//------------------------------------------------------------------------------

QScriptValue QHistogram::getMaxOccupancy(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double Histogram::getMaxOccupancy()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments(0);
    if( value.isError() ) return(value);


// execute ---------------------------------------
    double maxoccu = 0;
    for(size_t i=0; i < Histogram.size(); i++){
        if( maxoccu < Histogram[i] ){
            maxoccu = Histogram[i];
        }
    }

    return(maxoccu);
}

//------------------------------------------------------------------------------

QScriptValue QHistogram::getBinPosition(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double Histogram::getBinPosition(index)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments(1);
    if( value.isError() ) return(value);

    int index;
    value = GetArgAsInt("index","index",1,index);
    if( value.isError() ) return(value);

    if( (index < 0) || (index >= NBins) ){
        return( ThrowError("index","index out-of-legal range"));
    }

// execute ---------------------------------------
    double dch = (MaxValue - MinValue)/NBins;
    double bpos = (index + 0.5)*dch + MinValue;

    return(bpos);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QHistogram::save(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool Histogram::save(name)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("name",1);
    if( value.isError() ) return(value);

    QString name;
    value = GetArgAsString("name","name",1,name);
    if( value.isError() ) return(value);

// execute ---------------------------------------

    ofstream fout;
    fout.open(name.toStdString().c_str());
    if( ! fout ) return(false);
    save(fout);
    return((bool)fout);
}

//------------------------------------------------------------------------------

void QHistogram::save(ostream& str)
{
    str << "# Name      : " << Name.toStdString() << endl;
    str << scientific;
    str << "# Min value : " << MinValue << endl;
    str << "# Max value : " << MaxValue << endl;
    str << "# NBins     : " << NBins << endl;
    if( static_cast<int>(Histogram.size())!= NBins ){
    str << "# WARNING: histogram is not allocated!" << endl;
    }
    str << "#" << endl;
    str << "#      Value         Occurence         Extra         RMS(Extra)   " << endl;
    str << "# --------------- --------------- --------------- --------------- " << endl;

    for(size_t i=0; i < Histogram.size(); i++){
        double value = (i + 0.5)*(MaxValue-MinValue)/NBins + MinValue;
        double occur = Histogram[i];
        str << "  " << setw(15) << scientific << value;
        str << " " << setw(15) << scientific << occur;
        if( DataN[i] > 0 ){
            double extra = DataSum[i] / DataN[i];
            double rmsd = DataN[i]*Data2Sum[i] - DataSum[i]*DataSum[i];
            rmsd = sqrt(rmsd) / DataN[i];
            str << " " << setw(15) << scientific << extra;
            str << " " << setw(15) << scientific << rmsd;
        }
        str << endl;
    }

    str << "# -------------------------------" << endl;
    str << "# Number of samples within : " << NumOfSamplesWithin << endl;
    str << "# Number of samples        : " << NumOfSamples << endl;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


