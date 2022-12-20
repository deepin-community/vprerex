
/****************************************************************************
** Adapted from PDFviewer, originally published in the QT Quarterly:
** http://doc.trolltech.com/qq/QtQuarterly27.pdf
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** Modified for use with prerex:
** http://www.ctan.org/tex-archive/graphics/prerex
** Copyright (C) 2009-12 R. D. Tennent 
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

#ifndef DOCUMENTWIDGET_H
#define DOCUMENTWIDGET_H

#include <QRectF>
#include <QLabel>
#include <poppler-qt5.h>
#include <qclipboard.h>
#include <QFileInfo>
#include <QToolTip>
#include <QMessageBox>

class DocumentWidget : public QLabel
{
    Q_OBJECT

public:
    DocumentWidget(QWidget *parent = 0);
    ~DocumentWidget();
    Poppler::Document *document();
    QMatrix matrix() const;
    qreal scale() const;
    bool setDocument(const QString &filePath);

public slots:
    void reloadDocument1();
    void reloadDocument2();
    void setPage(int page = -1);
    void setScale(qreal scale);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

signals:
    void pageChanged(int currentPage);
    void documentReloaded();

private:
    void showPage(int page = -1);

    Poppler::Document *doc;
    int currentPage;
    qreal scaleFactor;
    QList<Poppler::Link * > pageLinks;
    bool mousePressed;
    QClipboard *cb;
    QString currentPath;
    QTimer *timer ;
    QFileInfo *fileInfo;
    int imageWidth;
    int imageHeight;
    int imageXoffset;
    int imageYoffset;
};

#endif
