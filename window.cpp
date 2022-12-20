
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
#include <QtGui>
#include "window.h"


# include <QString>
# include <QStringList>
# include <QThread>
# include <QInputDialog>

#include <qthread.h>

QString x_terminal_emulator = "xterm" ;

class mythread : public QThread
{
public:
        static void sleep(unsigned long secs) {
                QThread::sleep(secs);
        }
};


Window::Window(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi(this);

    scaleFactors << 1. << 1.1 << 1.25 << 1.75 << 2.5 << 4.;

    documentWidget = new DocumentWidget();
    scrollAreaWidget->setWidget(documentWidget);
    fsw = new QFileSystemWatcher();
    sb = statusBar();
    versionWidget = new QLabel(QString("This is vprerex, v.%1        ").arg(version), this);
    sb->addPermanentWidget(versionWidget);

    connect(documentControlsAction, SIGNAL(toggled(bool)),
            controlsDockWidget, SLOT(setVisible(bool)));

    connect(pageSpinBox, SIGNAL(valueChanged(int)),
            documentWidget, SLOT(setPage(int)));
    connect(documentWidget, SIGNAL(pageChanged(int)),
            pageSpinBox, SLOT(setValue(int)));
    connect(scaleComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(scaleDocument(int)));


    connect(fsw, SIGNAL(fileChanged(QString)), 
            documentWidget, SLOT(reloadDocument1()));
    connect(documentWidget, SIGNAL(documentReloaded()),
            this, SLOT(reloadMessage()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(quit()));

}

void Window::setFileNames(QString FileName, QString ChartFileName )
{
  fileName.append(FileName);
  fileInfo.setFile(FileName);
  chartFileName.append(ChartFileName);
  chartFileInfo.setFile(ChartFileName);
  loadDocument();
}

void Window::open()
{
   QString fullFileName;
   QString fullChartFileName;
   fullFileName.append(QDir::cleanPath(QFileDialog::getOpenFileName(this, "Select a prerex file to edit or a pdf file to view (or Cancel).", QString(), "Prerex source files (*.tex);;Portable Document Format files (*.pdf)")));
   if (fullFileName.length() == 0)   
   { // file selection cancelled 
     bool ok;
     QFile file("tmp.tex");
     while (1)
     {
       fullFileName.append(QDir::cleanPath(QFileDialog::getExistingDirectory(this, "Select a working directory.")));
       if ( fullFileName.length() == 0 ) { QMessageBox::critical(this, "vprerex", "Directory selection failed."); close(); exit(1);
       }
       fullFileName.append("/");
       fullFileName.append(QInputDialog::getText(this, "Creating a new prerex file", "Input file name:", QLineEdit::Normal, "mychart.tex" , &ok)); 
       if (!ok) 
       {    
         QMessageBox::critical(this, "vprerex", "File name input failed."); close(); exit(1);
       }
       if (!fullFileName.endsWith(".tex")) 
          fullFileName.append(".tex");
       file.setFileName(fullFileName);
       if (!file.exists()) break;
       if ( QMessageBox::question(this, "vprerex", "File exists; Ok to overwrite?",
              QMessageBox::Ok | QMessageBox::No, QMessageBox::Ok) == QMessageBox::Ok) break; 
       fullFileName.clear();
     }  
     if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
     {    
       QMessageBox::critical(this, "vprerex", "File opening failed."); close(); exit(1);
     }
     QTextStream out(&file);
     out << "\\documentclass{article}\n" << "\\usepackage{geometry}\n" << "\\geometry{noheadfoot, margin=0.5in}\n" << "\\usepackage{prerex}\n" << "\\begin{document}\n" << "\\thispagestyle{empty}\n" << "\\begin{chart}\n" << "\\end{chart}\n" << "\\end{document}\n";
     file.close();
     fileInfo.setFile(fullFileName);
     fileName.append(fileInfo.baseName());
     fileName.append(".");
     fileName.append(fileInfo.suffix());
   }
   else
   {
     fileInfo.setFile(fullFileName);
     fileName.append(fileInfo.baseName());
     fileName.append(".");
     fileName.append(fileInfo.suffix());
     fullChartFileName.append(QDir::cleanPath(QFileDialog::getOpenFileName(this, "Select a chart file (or Cancel).", fileInfo.absolutePath(), "Prerex chart files (*.tex)")));
     if (fullChartFileName.length() > 0)
     {
       chartFileInfo.setFile(fullChartFileName);
       chartFileName.append(chartFileInfo.baseName());
       chartFileName.append(".");
       chartFileName.append(chartFileInfo.suffix());
     }
   }
   loadDocument();
}

void Window::loadDocument(void)
{
   pdfFilePath.append(fileInfo.dir().path());
   pdfFilePath.append("/");
   pdfFilePath.append(fileInfo.baseName());
   pdfFilePath.append(".pdf");
   if ( fileName.endsWith(".tex") )
   {
     QStringList arguments;
     arguments   // for xterm
      << "-geometry" << "100x60" 
      << "-fa" << "Monospace"
      << "-fs" << "10"
      << "-e" << "prerex"; 
     arguments.append(fileName);
     if ( chartFileName.length() > 0 )
       arguments.append(chartFileName);
     process = new QProcess(centralwidget);
     process->setWorkingDirectory(fileInfo.dir().path());
     process->start(x_terminal_emulator, arguments);  
     if (!process->waitForStarted()) 
     { 
       QMessageBox::critical(this, "vprerex", "x-terminal call failed.");
       close();
       exit(1);
     }
   }

   QFileInfo pdfFileInfo;
   pdfFileInfo.setFile(pdfFilePath);
   while ( pdfFileInfo.size() == 0 ) 
   { // wait for the PDF file 
        mythread::sleep (1);
        pdfFileInfo.refresh();
   }
   mythread::sleep (1);
   sb->showMessage(pdfFilePath);
   fsw->addPath(pdfFilePath);
   while ( !(documentWidget->setDocument(pdfFilePath)) ) 
   { // if at first you don't succeed, try, try again 
     mythread::sleep (1);
   }    
   scaleComboBox->setEnabled(true);
   pageSpinBox->setEnabled(true);
   pageSpinBox->setMinimum(1);
   pageSpinBox->setMaximum(documentWidget->document()->numPages());
   pageSpinBox->setValue(1);
}

void Window::reloadMessage(void)
{
  sb->showMessage(pdfFilePath + " reloaded.", 2000);
  QTimer::singleShot(2000, this, SLOT(pathMessage()));
  
  // On Windows, pdflatex removes the existing pdf file initially and
  // this removes the file path from the file-system watcher; the
  // following restores the path to the file-watch list if necessary:
  if (!fsw->files().contains(pdfFilePath)) fsw->addPath(pdfFilePath); }

void Window::pathMessage(void)
{
  sb->showMessage(pdfFilePath);
}

void Window::scaleDocument(int index)
{
  documentWidget->setScale(scaleFactors[index]);
}

void Window::quit(void)
{
  close();
}
