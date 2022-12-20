/****************************************************************************
** Adapted from PDFviewer, originally published in the QT Quarterly:
** http://doc.trolltech.com/qq/QtQuarterly27.pdf
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** Modified for use with prerex:
** http://www.ctan.org/tex-archive/graphics/prerex
** Copyright (C) 2009-14 R. D. Tennent 
** School of Computing, Queen's University, rdt@cs.queensu.ca 

** This program is free software; you can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by the
** Free Software Foundation; either version 2 of the License, or (at your
** option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
** Public License for more details.
** 
** You should have received a copy of the GNU General Public License along
** with this program; if not, write to the Free Software Foundation, Inc.,
** 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
**
****************************************************************************/

/*
** vprerex (visual prerex) is a GUI front-end and viewer for the prerex
** interactive editor of prerequisite-chart descriptions in the prerex
** format. The viewer may be used just as a minimalist viewer of
** arbitrary PDF files but supports editing of charts generated using
** prerex.sty by allowing coordinates of course boxes, arrows, and
** background points to be conveyed back to the prerex editor command-line
** using the clipboard.
*/

#include <QApplication>
#include "window.h"
#include <iostream>

using namespace std;


int main(int argc, char *argv[])
{

    QApplication app(argc, argv);
    Window window;
    QStringList Argv = app.arguments();

    printf ("This is vprerex, version %s.\n", version);
    puts ("Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).");
    puts ("Qt Software Information (qt-info@nokia.com)");
    puts ("Copyright (c) 2009-17  R. D. Tennent");
    puts ("School of Computing, Queen's University, rdt@cs.queensu.ca");
    puts
      ("This program comes with NO WARRANTY. You can redistribute vprerex and/or");
    puts
      ("modify it under the terms of the GNU General Public License version 2.");
    puts ("");

    window.show();
    if ( argc > 1 )
    {
      QString fileName;
      QString chartFileName;
      fileName.append(Argv.at(1));
      if (!fileName.endsWith(".tex")  &&  !fileName.endsWith(".pdf")) 
        fileName.append(".tex");
      if ( argc > 2 )
      {
        chartFileName.append(Argv.at(2));
        if (!chartFileName.endsWith(".tex")) 
          chartFileName.append(".tex");
      }
      window.setFileNames(fileName, chartFileName);
    }
    else 
      window.open();

    return app.exec();
}
