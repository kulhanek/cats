#ifndef SCRIPTTHREAD_HPP
#define SCRIPTTHREAD_HPP
#include <QScriptEngineDebugger>

#include <QCATs.hpp>


class ScriptThread : public QThread
{
private:
    QScriptEngine JSEngine;
    QScriptValue result;
    QString JSCode;
    QTextEdit textBrowser;
    void run()
    {
        new Q_DebugStream(std::cout, &textBrowser); //Redirect Console output to QTextEdit
        Q_DebugStream::registerQDebugMessageHandler(); //Redirect qDebug() output to QTextEdit

        // redirect stdout to file
        freopen("D:\\programy\\Git\\cats-build\\src\\projects\\cats\\2.0\\src\\bin\\cats-ide\\cats-output.txt","w",stdout);

        // disable buffering
        setvbuf(stdout,NULL,_IONBF,0);

        result = JSEngine.evaluate(JSCode);

        // reenable buffering
        setvbuf(stdout,NULL,_IOFBF,0);

        //return stdout to display
        freopen( "CON","w",stdout);

        #ifdef DEBUG
        printf("unable to do...");
        #endif

        //result = JSEngine.evaluate(JSCode);
    }
public:
    void registerCATs()
    {
        // metoda registerCATs se stava zbytecnou, pouzijte rovnou RegisterAllCATsClasses()
        RegisterAllCATsClasses(JSEngine);
    }
    QString getOutput()
    {
        QString output = textBrowser.document()->toPlainText();
        textBrowser.clear();
        return output;
    }
    QScriptEngine* getEngine()
    {
        return &JSEngine;
    }
    void setCode(QString code)
    {
        JSCode = code;
    }
    QScriptValue getResult()
    {
        return result;
    }
};

#endif // SCRIPTTHREAD_HPP
