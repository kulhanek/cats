/* =====================================================================
 * This file is part of CATs - Conversion and Analysis Tools.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * CATs developed by: Petr Kulhánek, kulhanek@chemi.muni.cz
 * CATs IDE developed by: Jaroslav Oľha, jaroslav.olha@gmail.com
 * =====================================================================
 */

#include "StdoutWatcher.hpp"

CStdoutWatcher::CStdoutWatcher(QObject *parent, QString fileName)
      : QThread(parent)
{
    RedirectFileName = fileName;
}

void CStdoutWatcher::run()
{
    QFile outputFile(RedirectFileName);

    if (!outputFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream stream(&outputFile);

    //Keep checking the temporary file for new lines every 20 miliseconds, until the main window sets
    //the Terminated bool to 'true'. If there are new lines to be read (rather than EOF), send them
    //to the main window via the LineRead signal.
    while (!Terminated)
    {
        sleep(0.02);

        QString line = stream.read(100);

        if (line != NULL)
        {
            emit LineRead(line);
        }
    }
}

void CStdoutWatcher::StartOutputRedirection()
{
    Terminated = false;

    //Convert the temporary file name to char* so that it can be passed to the freopen method.
    QByteArray ba = RedirectFileName.toLatin1();
    const char *c_str = ba.data();

    //Redirect the stdout stream to the given temporary file.
    freopen(c_str,"wa",stdout);
    setvbuf(stdout,NULL,_IONBF,0);

    start();
}

void CStdoutWatcher::StopOutputRedirection()
{
    //Restore the stdout stream.
    setvbuf(stdout,NULL,_IOFBF,0);
    freopen("CON","w",stdout);
}
