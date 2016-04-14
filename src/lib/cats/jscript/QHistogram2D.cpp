// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2016 Viktor Illík
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
#include <QHistogram2D.hpp>
#include <iomanip>
#include <fstream>
#include <SmallString.hpp>
#include <math.h>
#include <TerminalStr.hpp>

//------------------------------------------------------------------------------

#include <QHistogram2D.moc>
using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QHistogram2D::Register(QScriptEngine& engine)
{
    QScriptValue ctor = engine.newFunction(QHistogram2D::New);
    QScriptValue metaObject = engine.newQMetaObject(&QHistogram2D::staticMetaObject, ctor);
    engine.globalObject().setProperty("Histogram2D", metaObject);
}

//------------------------------------------------------------------------------

QScriptValue QHistogram2D::New(QScriptContext *context,QScriptEngine *engine)
{
    if( context->isCalledAsConstructor() == false ) {
        context->throwError("new Histogram2D([histogram]) - it can be called only as a constructor");
        return(engine->undefinedValue());
    }
    if( context->argumentCount() > 1 ) {
        context->throwError("new Histogram2D([histogram]) - illegal number of arguments, none or one is expected");
        return(engine->undefinedValue());
    }

    QCATsScriptable scriptable("Histogram2D");
    QScriptValue    value;

// print help ------------------------------------
    if( scriptable.IsHelpRequested() ){
        CTerminalStr sout;
        sout << "Histogram2D object" << endl;
        sout << endl;
        sout << "Constructors:" << endl;
        sout << "   new Histogram2D()" << endl;
        sout << "   new Histogram2D(histogram)" << endl;
        return(scriptable.GetUndefinedValue());
    }

// check arguments -------------------------------
    value = scriptable.IsCalledAsConstructor();
    if( value.isError() ) return(value);

    value = scriptable.CheckNumberOfArguments("[histogram]",0,1);
    if( value.isError() ) return(value);

// create pbject
    QHistogram2D* p_src = NULL;
    if( scriptable.GetArgumentCount() > 0 ){
        value = scriptable.GetArgAsObject<>("histogram","histogram","Histogram2D",1,p_src);
        if( value.isError() ) return(value);
    }

    QHistogram2D* p_hist = new QHistogram2D();

    if( p_src != NULL ){
        // copy setup from source histogram
        p_hist->Name = p_src->Name;
        p_hist->MinValueD1 = p_src->MinValueD1;
        p_hist->MinValueD2 = p_src->MinValueD2;
        p_hist->MaxValueD1 = p_src->MaxValueD1;
        p_hist->MaxValueD2 = p_src->MaxValueD2;
        p_hist->NBinsD1 = p_src->NBinsD1;
        p_hist->NBinsD2 = p_src->NBinsD2;
        p_hist->allocateInternal();
    }

    return(engine->newQObject(p_hist, QScriptEngine::ScriptOwnership));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QHistogram2D::QHistogram2D(void)
    : QCATsScriptable("Histogram2D")
{
    MinValueD1 = 0;
    MaxValueD1 = 10.0;
    NBinsD1 = 100;
    BinSizeD1 = (MaxValueD1 - MinValueD1)/NBinsD1;
    MinValueD2 = 0;
    MaxValueD2 = 10.0;
    NBinsD2 = 100;
    BinSizeD2 = (MaxValueD2 - MinValueD2)/NBinsD2;
    NumOfSamples = 0;
    NumOfSamplesWithin = 0;
    BinSize = BinSizeD1 * BinSizeD2;
    NBins = NBinsD1 * NBinsD2;
    allocateInternal();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QHistogram2D::setName(const QScriptValue& dummy)
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

QScriptValue QHistogram2D::getName(void)
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

QScriptValue QHistogram2D::setMinValueD1(const QScriptValue& dummy)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Histogram::setMinValueD1(value)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("value",1);
    if( value.isError() ) return(value);

    double num;
    value = GetArgAsRNumber("value","value",1,num);
    if( value.isError() ) return(value);

    if( num >= MaxValueD1 ){
        CSmallString error;
        error << "min value (" << CSmallString().DoubleToStr(num) << ") must be lower than max value (" << CSmallString().DoubleToStr(MaxValueD1) <<")";
        return( ThrowError("value",error));
    }

// execute ---------------------------------------
    MinValueD1 = num;
    resetInternal();

    return(true);
}

//------------------------------------------------------------------------------

QScriptValue QHistogram2D::getMinValueD1(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double Histogram::getMinValueD1()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(MinValueD1);
}

//------------------------------------------------------------------------------

QScriptValue QHistogram2D::setMaxValueD1(const QScriptValue& dummy)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Histogram::setMaxValueD1(value)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("value",1);
    if( value.isError() ) return(value);

    double num;
    value = GetArgAsRNumber("value","value",1,num);
    if( value.isError() ) return(value);

    if( num <= MinValueD1 ){
        CSmallString error;
        error << "max value (" << CSmallString().DoubleToStr(num) << ") must be higher than min value (" << CSmallString().DoubleToStr(MinValueD1) <<")";
        return( ThrowError("value",error));
    }

// execute ---------------------------------------
    MaxValueD1 = num;
    resetInternal();

    return(true);
}

//------------------------------------------------------------------------------

QScriptValue QHistogram2D::getMaxValueD1(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double Histogram::getMaxValueD1()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(MaxValueD1);
}

//------------------------------------------------------------------------------

QScriptValue QHistogram2D::setNumOfBinsD1(const QScriptValue& dummy)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Histogram::setNumOfBinsD1(value)" << endl;
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
    NBinsD1 = num;
    allocateInternal();

    return(true);
}

//------------------------------------------------------------------------------

QScriptValue QHistogram2D::getNumOfBinsD1(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int Histogram::getNumOfBinsD1()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(NBinsD1);
}

//------------------------------------------------------------------------------

QScriptValue QHistogram2D::setMinValueD2(const QScriptValue& dummy)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Histogram::setMinValueD2(value)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("value",1);
    if( value.isError() ) return(value);

    double num;
    value = GetArgAsRNumber("value","value",1,num);
    if( value.isError() ) return(value);

    if( num >= MaxValueD2 ){
        CSmallString error;
        error << "min value (" << CSmallString().DoubleToStr(num) << ") must be lower than max value (" << CSmallString().DoubleToStr(MaxValueD2) <<")";
        return( ThrowError("value",error));
    }

// execute ---------------------------------------
    MinValueD2 = num;
    resetInternal();

    return(true);
}

//------------------------------------------------------------------------------

QScriptValue QHistogram2D::getMinValueD2(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double Histogram::getMinValueD2()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(MinValueD2);
}

//------------------------------------------------------------------------------

QScriptValue QHistogram2D::setMaxValueD2(const QScriptValue& dummy)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Histogram::setMaxValueD2(value)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("value",1);
    if( value.isError() ) return(value);

    double num;
    value = GetArgAsRNumber("value","value",1,num);
    if( value.isError() ) return(value);

    if( num <= MinValueD2 ){
        CSmallString error;
        error << "max value (" << CSmallString().DoubleToStr(num) << ") must be higher than min value (" << CSmallString().DoubleToStr(MinValueD2) <<")";
        return( ThrowError("value",error));
    }

// execute ---------------------------------------
    MaxValueD2 = num;
    resetInternal();

    return(true);
}

//------------------------------------------------------------------------------

QScriptValue QHistogram2D::getMaxValueD2(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double Histogram::getMaxValueD2()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(MaxValueD2);
}

//------------------------------------------------------------------------------

QScriptValue QHistogram2D::setNumOfBinsD2(const QScriptValue& dummy)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Histogram::setNumOfBinsD2(value)" << endl;
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
    NBinsD2 = num;
    allocateInternal();

    return(true);
}

//------------------------------------------------------------------------------

QScriptValue QHistogram2D::getNumOfBinsD2(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int Histogram::getNumOfBinsD2()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(NBinsD2);
}

//------------------------------------------------------------------------------

QScriptValue QHistogram2D::printInfo(void)
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

void QHistogram2D::allocateInternal(void)
{
    NBins = NBinsD1 * NBinsD2;
    Histogram.resize(NBins);
    DataN.resize(NBins);
    DataSum.resize(NBins);
    Data2Sum.resize(NBins);
    BinSizeD1 = (MaxValueD1 - MinValueD1)/NBinsD1;
    BinSizeD2 = (MaxValueD2 - MinValueD2)/NBinsD2;
    BinSize = BinSizeD1 * BinSizeD2;
    resetInternal();
}

//------------------------------------------------------------------------------

void QHistogram2D::releaseInternal(void)
{
    Histogram.clear();
    DataN.clear();
    DataSum.clear();
    Data2Sum.clear();
    resetInternal();
}

//------------------------------------------------------------------------------

void QHistogram2D::resetInternal(void)
{
    NumOfSamples = 0;
    NumOfSamplesWithin = 0;
    for(size_t i=0; i < Histogram.size(); i++){
        Histogram[i] = 0;
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

QScriptValue QHistogram2D::addSample(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool Histogram2D::addSample(value,value[,extra])" << endl;
        return(false);
    }

// check arguments ------------------------------
    value = CheckNumberOfArguments("valueD1,valueD2[,extra]",2,3);
    if( value.isError() ) return(value);

    double numD1;
    value = GetArgAsRNumber("valueD1,valueD2[,extra]","valueD1",1,numD1);
    if( value.isError() ) return(value);

    double numD2;
    value = GetArgAsRNumber("valueD1,valueD2[,extra]","valueD1",2,numD2);
    if( value.isError() ) return(value);

    double extra;
    if( GetArgumentCount() > 2 ){
        value = GetArgAsRNumber("valueD1,valueD2,extra","extra",3,extra);
        if( value.isError() ) return(value);
    }


// execute ---------------------------------------
    if( NBins <= 0 ){
        return( ThrowError("value,value[,extra]","number of binsD1 is zero"));
    }

    NumOfSamples++;
    if( (numD1 < MinValueD1) || (numD1 >= MaxValueD1) || (numD2 < MinValueD2) || (numD2 >= MaxValueD2) ) return(false);
    NumOfSamplesWithin++;


    // calculate index
    if( BinSize <= 0 ){ /// STACI BINSIZE alebo D1 D2???
        return( ThrowError("value,value[,extra]","binD1 width is zero or negative value"));
    }

    int indexD1 = (numD1 - MinValueD1)/BinSizeD1;
    if( (indexD1 < 0) || (indexD1 >= static_cast<int>(Histogram.size())) ) return(false);

    int indexD2 = (numD2 - MinValueD2)/BinSizeD2;
    if( (indexD2 < 0) || (indexD2 >= static_cast<int>(Histogram.size())) ) return(false);

    // increment value
    int index = indexD1 * NBinsD2 + indexD2;
    Histogram[index] += 1;

    if( GetArgumentCount() > 2 ){
        // extra data
        DataN[index] += 1;
        DataSum[index] += extra;
        Data2Sum[index] += extra*extra;
    }

    return(true);
}

//------------------------------------------------------------------------------

QScriptValue QHistogram2D::multBy(void)
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

QScriptValue QHistogram2D::normalize(void)
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
    if( within ){
        factor = NumOfSamplesWithin * BinSize;
    } else {
        factor = NumOfSamples * BinSize;
    }

    for(size_t i=0; i < Histogram.size(); i++){
        Histogram[i] /= factor;
    }

    return(true);
}

//------------------------------------------------------------------------------

QScriptValue QHistogram2D::reset(void)
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

QScriptValue QHistogram2D::getNumOfSamples(void)
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

QScriptValue QHistogram2D::getNumOfSamplesWithin(void)
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

QScriptValue QHistogram2D::getIntegral(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Histogram::getIntegral(fromD1,toD1,fromD2,toD2)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("fromD1,toD1,fromD2,toD2",4);
    if( value.isError() ) return(value);

    double fromD1,toD1, fromD2, toD2;
    value = GetArgAsRNumber("fromD1,toD1,fromD2,toD2","fromD1",1,fromD1);
    if( value.isError() ) return(value);
    value = GetArgAsRNumber("fromD1,toD1,fromD2,toD2","toD1",2,toD1);
    if( value.isError() ) return(value);
    value = GetArgAsRNumber("fromD1,toD1,fromD2,toD2","fromD2",3,fromD2);
    if( value.isError() ) return(value);
    value = GetArgAsRNumber("fromD1,toD1,fromD2,toD2","toD2",4,toD2);
    if( value.isError() ) return(value);

    if( fromD1 >= toD1 ){
        CSmallString error;
        error << "'fromD1' value (" << CSmallString().DoubleToStr(fromD1) << ") must be smaller than 'toD1' value (" << CSmallString().DoubleToStr(toD1) <<")";
        return( ThrowError("value",error));
    }
    if( fromD2 >= toD2 ){
        CSmallString error;
        error << "'fromD2' value (" << CSmallString().DoubleToStr(fromD2) << ") must be smaller than 'toD2' value (" << CSmallString().DoubleToStr(toD2) <<")";
        return( ThrowError("value",error));
    }

    // calculate index
    ///double dch = (MaxValue - MinValue)/NBins; ---> BIN SIZE
    ///if( dch <= 0 ){
    if( BinSize <= 0 ){
        return( ThrowError("from,to","bin width is zero or negative value"));
    }
    int indexD1a = (fromD1 - MinValueD1) / BinSizeD1;
    if( indexD1a < 0 ) indexD1a = 0;
    if( indexD1a >= static_cast<int>(Histogram.size()) ) indexD1a = static_cast<int>(Histogram.size()) - 1;

    int indexD1b = (toD1 - MinValueD1) / BinSizeD1;
    if( indexD1b < 0 ) indexD1b = 0;
    if( indexD1b >= static_cast<int>(Histogram.size()) ) indexD1b = static_cast<int>(Histogram.size()) - 1;

    int indexD2a = (fromD2 - MinValueD2) / BinSizeD2;
    if( indexD2a < 0 ) indexD2a = 0;
    if( indexD2a >= static_cast<int>(Histogram.size()) ) indexD2a = static_cast<int>(Histogram.size()) - 1;

    int indexD2b = (toD2 - MinValueD2) / BinSizeD2;
    if( indexD2b < 0 ) indexD2b = 0;
    if( indexD2b >= static_cast<int>(Histogram.size()) ) indexD2b = static_cast<int>(Histogram.size()) - 1;

// execute ---------------------------------------
    double intval = 0;
    for(int i=indexD1a; i<=indexD1b; i++){
        for(int j=indexD2a; j<=indexD2b; j++){
            int index = i*NBinsD2 + j;
            intval += Histogram[index]*BinSize;
        }
    }

    return(intval);
}

//------------------------------------------------------------------------------

QScriptValue QHistogram2D::getMaxOccupancy(void)
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
    for(size_t i=0; i <= Histogram.size(); i++){
        if( maxoccu < Histogram[i] ){
            maxoccu = Histogram[i];
        }
    }

    return(maxoccu);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QHistogram2D::save(void)
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

void QHistogram2D::save(ostream& str)
{
    str << "# Name      : " << Name.toStdString() << endl;
    str << scientific;
    str << "# Dimension1" << endl;
    ///str << "# Name      : " << NameD1.toStdString();
    str << "# Min value : " << MinValueD1 << endl;
    str << "# Max value : " << MaxValueD1 << endl;
    str << "# NBins     : " << NBinsD1 << endl;
    str << "# Dimension2" << endl;
    ///str << "# Name      : " << NameD2.toStdString();
    str << "# Min value : " << MinValueD2 << endl;
    str << "# Max value : " << MaxValueD2 << endl;
    str << "# NBins     : " << NBinsD2 << endl;
    if( static_cast<int>(Histogram.size())!= NBins ){
    str << "# WARNING: histogram is not allocated!" << endl;
    }
    str << "#" << endl;
    str << "#      Value1         Value2         Occurence         Extra         RMS(Extra)   " << endl;
    str << "# --------------- --------------- --------------- --------------- --------------- " << endl;

    for(size_t i=0; i < Histogram.size(); i++){
        double valueD1 = ((i-i%NBinsD2)/NBinsD2 + 0.5)*BinSizeD1 + MinValueD1;
        double valueD2 = ((i%NBinsD1) + 0.5)*BinSizeD2 + MinValueD2;
        double occur = Histogram[i];
        str << " " << setw(15) << scientific << valueD1;
        str << " " << setw(15) << scientific << valueD2;
        str << " " << setw(15) << scientific << occur;
        if( DataN[i] > 0 ){
            double extra = DataSum[i] / DataN[i];
            double rmsd = DataN[i]*Data2Sum[i] - DataSum[i]*DataSum[i];
            rmsd = sqrt(rmsd) / DataN[i];
            str << " " << setw(15) << scientific << extra;
            str << " " << setw(15) << scientific << rmsd;
        }
        str << endl;
        if( i%NBinsD2 == 1){
            str << endl;
        }
    }

    str << "# -------------------------------" << endl;
    str << "# Number of samples within : " << NumOfSamplesWithin << endl;
    str << "# Number of samples        : " << NumOfSamples << endl;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


