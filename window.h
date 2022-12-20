
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

#ifndef WINDOW_H
#define WINDOW_H

# include <QMainWindow>
# include "documentwidget.h"
# include <QFileSystemWatcher>
# include "ui_window.h"
# include <QProcess>
# include <QMessageBox>
# include <QFileDialog>


const char version[] = "6.5.1";

class QTextBrowser;

class Window : public QMainWindow, public Ui_MainWindow
{
    Q_OBJECT

public:
    Window(QWidget *parent = 0);
    QFileSystemWatcher *fsw;
    QStatusBar *sb;
    void setFileNames(QString BaseFileName, QString ChartFileName);
    void loadDocument(void);
    void open();

private slots:
    void scaleDocument(int index);
    void reloadMessage();
    void pathMessage();
    void quit();

private:
    DocumentWidget *documentWidget;
    QString fileName;
    QFileInfo fileInfo;
    QString chartFileName;
    QFileInfo chartFileInfo;
    QString pdfFilePath;
    QVector<qreal> scaleFactors;
    QLabel *versionWidget;
    QProcess *process;
};

#endif
