
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
#include <poppler-qt5.h>
#include "documentwidget.h"

DocumentWidget::DocumentWidget(QWidget *parent)
    : QLabel(parent)
{
    currentPage = -1;
    doc = NULL;
    scaleFactor = 1.1;
    setCursor(Qt::ArrowCursor);
    setAlignment(Qt::AlignTop | Qt::AlignCenter);
    mousePressed = false;
    setMouseTracking(true);
    cb = QGuiApplication::clipboard();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(reloadDocument2()));
}

DocumentWidget::~DocumentWidget()
{
    delete doc;
}

Poppler::Document *DocumentWidget::document()
{
    return doc;
}

void DocumentWidget::mousePressEvent(QMouseEvent *event)
{
    Qt::MouseButton qtm = event->button();
    if ( !(qtm == Qt::LeftButton || qtm == Qt::RightButton) ) return;
    int x1 = 0, y1 = 0, x2 = 0, y2 = 0;     // pixel coordinates of anchors 
    int xc1 = 0, yc1 = 0, xc2 = 0, yc2 = 0; // chart coordinates of anchors
    int n_anchors = 0;   // number of anchors found 
    imageXoffset = 0; imageYoffset = 0;     // re-compute in case of window re-sizing
    if (imageWidth < width())
       imageXoffset = (width() - imageWidth) / 2;
    if (imageHeight < height())  // possible after re-scaling despite AlignTop
       imageYoffset = (height() - imageHeight) / 2;
    int xmouse = event->pos().x() - imageXoffset;  // mouse coordinates in pixels
    int ymouse = event->pos().y() - imageYoffset;
    QPointF mousePos  // normalized position in [0..1] coordinates
     = QPointF((qreal)xmouse / (qreal)imageWidth, (qreal)ymouse / (qreal)imageHeight);

    
    // iterate through the Links:
    for (int i = 0; i < pageLinks.size(); ++i)
    {
        Poppler::LinkBrowse * link = static_cast<Poppler::LinkBrowse * >(pageLinks[i]);
        if (link == NULL) continue;  // not a Browse Link
        QString URI = link->url();
        QRectF linkRect = pageLinks[i]->linkArea();
        if ( linkRect.contains(mousePos) &&  URI.startsWith(QString("coord:")) )
            {   // clicked in a coord: x,y box or coord: x0,y0,x1,y1 bullet
                QString cbString;
                if ( qtm == Qt::RightButton )
                {
                   if (cb->supportsSelection())
                     cbString.append(cb->text(QClipboard::Selection));
                   else
                     cbString.append(cb->text(QClipboard::Clipboard));
                   cbString.append(" ");
                }
                URI.remove("coord: ");
                cbString.append(URI);
                if (cb->supportsSelection())
                  cb->setText(cbString, QClipboard::Selection);
                else
                  cb->setText(cbString, QClipboard::Clipboard);
                setCursor(Qt::CrossCursor);
                mousePressed = true;
                return;
            }
        if ( URI.startsWith(QString("anchor:")) ) 
        {   
            QPointF anchorCenter  //  in [0..1] coordinates 
            = (pageLinks[i]->linkArea()).center();
            URI.remove(0,7);      // remove "anchor:"
            int comma = URI.indexOf(',');
            QString xs = URI.left(comma);        // x-coordinate as a string
            QString ys = URI.remove(0, comma+1); // y-coordinate as a string
            if (n_anchors == 0)
            {
               xc1 = xs.toInt();
               yc1 = ys.toInt();
               x1 = (int)(imageWidth * anchorCenter.x());
               y1 = (int)(imageHeight * anchorCenter.y());
               n_anchors++;
            }
            else if (n_anchors == 1)
            {
               xc2 = xs.toInt();
               yc2 = ys.toInt();
               x2 = (int)(imageWidth * anchorCenter.x());
               y2 = (int)(imageHeight * anchorCenter.y());
               n_anchors++;
            }
        }
    }
    if (n_anchors == 0) return;  // coordinate grid off?
    if (n_anchors == 1)          
    {
      QMessageBox::warning (this, "vprerex", "Only found one anchor");
      return;
    }
    //  clicked on background grid; must be LeftButton click
    if ( qtm == Qt::RightButton ) return;
    //  determine chart coordinates of the click point:
    int  xc = (int)( 0.25L + (xc1 + xc2)/2.0L - ((x1 + x2)/2.0L - xmouse) * (qreal)(xc2 - xc1) / (x2 - x1) );
    int  yc = (int)( 0.75L + (yc1 + yc2)/2.0L - ((y1 + y2)/2.0L - ymouse) * (qreal)(yc2 - yc1) / (y2 - y1) );
    if (cb->supportsSelection())
      cb->setText(QString("%1,%2").arg(xc).arg(yc), QClipboard::Selection);
    else
      cb->setText(QString("%1,%2").arg(xc).arg(yc), QClipboard::Clipboard);
    mousePressed = true;
    setCursor(Qt::CrossCursor);
}

void DocumentWidget::mouseMoveEvent(QMouseEvent *event)
{

    imageXoffset = 0; imageYoffset = 0;  // re-compute in case of window re-sizing
    if (imageWidth < width())
       imageXoffset = (width() - imageWidth) / 2;
    if (imageHeight < height())  // possible after re-scaling despite AlignTop
       imageYoffset = (height() - imageHeight) / 2;
    int xmouse = event->pos().x() - imageXoffset;  // mouse coordinates in pixels
    int ymouse = event->pos().y() - imageYoffset;
    QPointF mousePos  // normalized click position in [0..1] coordinates
    = QPointF((qreal)xmouse / (qreal)imageWidth, (qreal)ymouse / (qreal)imageHeight);
    
    // iterate through the Links:
    for (int i = 0; i < pageLinks.size(); ++i)
    {
        Poppler::LinkBrowse * link = static_cast<Poppler::LinkBrowse * >(pageLinks[i]);
        if (link == NULL) continue;  // not a Browse Link
        QString URI = link->url();
        QRectF linkRect = pageLinks[i]->linkArea();
        if ( linkRect.contains(mousePos) ) 
        {
          if ( mousePressed ) return;
          setCursor(Qt::PointingHandCursor);
          setToolTip(URI);
          return;
        }
    }
    if (mousePressed) return;
    setCursor(Qt::ArrowCursor);
}

void DocumentWidget::mouseReleaseEvent(QMouseEvent *)
{
    mousePressed = false;
    setCursor(Qt::ArrowCursor);
}

qreal DocumentWidget::scale() const
{
    return scaleFactor;
}

void DocumentWidget::showPage(int page)
{
    if (page != -1 && page != currentPage + 1) {
        currentPage = page - 1;
        emit pageChanged(page);
    }
    QImage image 
    = doc->page(currentPage)->renderToImage(scaleFactor * physicalDpiX(), scaleFactor * physicalDpiY());
    setPixmap(QPixmap::fromImage(image));
    imageWidth = image.width();
    imageHeight = image.height();
    pageLinks = doc -> page(currentPage) -> links();
    setCursor(Qt::ArrowCursor);
}

bool DocumentWidget::setDocument(const QString &filePath)
{
    doc = Poppler::Document::load(filePath);
    if (doc == NULL) {
    // temporarily ill-formed pdf?  wait for another reload signal
      return( false );
    }
    currentPath = filePath;
    fileInfo = new QFileInfo(currentPath);
    doc->setRenderHint(Poppler::Document::Antialiasing);
    doc->setRenderHint(Poppler::Document::TextAntialiasing);
    currentPage = -1;
    setPage(1);
    return( true );
}

void DocumentWidget::reloadDocument1(void)
{ 
  fileInfo->refresh();
  if (fileInfo->size() == 0) return; // pdflatex clears file.pdf 
  timer->setSingleShot(true);
  timer->start(2000);  // wait for the file to stabilize
  // When the timer timesout, it calls reloadDocument2
}

void DocumentWidget::reloadDocument2(void)
{
    Poppler::Document *savedoc;
    fileInfo->refresh();
    if (fileInfo->size() == 0) return; // pdflatex clears file.pdf 
    savedoc = doc;
    doc = Poppler::Document::load(currentPath);
    if (doc == NULL) {
    // temporarily ill-formed pdf?  wait for another reload signal
        doc = savedoc;
        return;
    }
    delete savedoc;
    doc->setRenderHint(Poppler::Document::Antialiasing);
    doc->setRenderHint(Poppler::Document::TextAntialiasing);
    showPage(currentPage+1);
    documentReloaded();
}

void DocumentWidget::setPage(int page)
{
    if (page != currentPage + 1) {
        showPage(page);
    }
}

void DocumentWidget::setScale(qreal scale)
{
    if (scaleFactor != scale) {
        scaleFactor = scale;
        showPage(-1);
    }
}
