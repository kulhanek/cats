#include "IDEWindow.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CIDEWindow w;
    w.show();

    return a.exec();
}
