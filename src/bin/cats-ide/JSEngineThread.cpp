#include "JSEngineThread.hpp"

CJSEngineThread::CJSEngineThread(QObject *parent)
    : QThread(parent)
{
    JSEngine = NULL;
}

void CJSEngineThread::RunCode(const QString &code)
{
    JSEngine = new QScriptEngine();

    RegisterAllCATsClasses(*JSEngine);

    QString firstLine = code.split(QRegExp("[\r\n]"),QString::SkipEmptyParts)[0];
    if (firstLine.contains("#!"))
    {
        JSCode = "//" + code;
    }
    else
    {
        JSCode = code;
    }

/*    freopen("D:\\programy\\Git\\cats-build\\src\\projects\\cats\\2.0\\src\\bin\\cats-ide\\cats-output.txt","wa",stdout);
    setvbuf(stdout,NULL,_IONBF,0);
*/
    start();

/*    setvbuf(stdout,NULL,_IOFBF,0);
    freopen( "CON","w",stdout);
*/
}

void CJSEngineThread::run()
{
    Result = JSEngine->evaluate(JSCode);

    delete JSEngine;

    JSEngine = NULL;
}

QScriptValue CJSEngineThread::GetResult()
{
    return Result;
}
