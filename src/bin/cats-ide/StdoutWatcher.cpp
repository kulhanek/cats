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
 * CATs developed by: RNDr. Petr Kulhánek, PhD.
 * CATs IDE developed by: Mgr. Jaroslav Oľha
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

    QByteArray ba = RedirectFileName.toLatin1();
    const char *c_str = ba.data();

    freopen(c_str,"wa",stdout);

    setvbuf(stdout,NULL,_IONBF,0);

    start();
}

void CStdoutWatcher::StopOutputRedirection()
{
    setvbuf(stdout,NULL,_IOFBF,0);
    freopen("CON","w",stdout);
}
