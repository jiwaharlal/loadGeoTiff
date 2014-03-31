#include "MainWindow.h"

#include <QPainter>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>

#include <tiffio.h>

#include "TiffRenderer.h"

MainWindow::MainWindow(QGraphicsItem *parent /* = NULL */ )
    : QGraphicsWidget( parent )
    , m_isMousePressed( false )
{
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
//    setMinimumSize( 1024, 768 );
}

namespace {
    inline int red( uint32 tiffRgba ) {
        return 0xff & tiffRgba;
    }

    inline int green( uint32 tiffRgba ) {
        return ( 0xff00 & tiffRgba ) >> 8;
    }

    inline int blue( uint32 tiffRgba ) {
        return ( 0xff0000 & tiffRgba ) >> 16;
    }

    inline int alpha( uint32 tiffRgba ) {
        return ( 0xff000000 & tiffRgba ) >> 24;
    }
}

void MainWindow::loadTiff(const QString &tiffFileName)
{
//    TIFF* tif = TIFFOpen( tiffFileName.toStdString().c_str(), "r" );
//    if ( tif )
//    {
//        uint32 imageWidth, imageLength;
//        uint32 tileWidth, tileLength;
//        uint32 x, y;
//        uint32* buf;

//        TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &imageWidth);
//        TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &imageLength);
//        TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tileWidth);
//        TIFFGetField(tif, TIFFTAG_TILELENGTH, &tileLength);

//        buf = (uint32*) _TIFFmalloc( TIFFTileSize( tif ) * 4 );
//        TIFFReadRGBATile( tif, 0, 0, buf );

//        m_image = new QImage( tileWidth, tileLength, QImage::Format_ARGB32 );
//        for ( int row = 0; row < tileLength; row++ ) {
//            for ( int col = 0; col < tileWidth; col++ ) {
//                uint32 pixelColor = *(buf + row * tileWidth + col);
//                m_image->setPixel( col, tileLength - row - 1, qRgba( red(pixelColor), green(pixelColor), blue(pixelColor), alpha(pixelColor) ) );
//            }
//        }

//        _TIFFfree(buf);

//        qDebug() << "Image dimensions: " << imageWidth << ", " << imageLength;
//        qDebug() << "Tile dimensions: " << tileWidth << ", " << tileLength;
//        qDebug() << "Buffer size: " << TIFFTileSize( tif );
//        TIFFClose( tif );
//    }

    m_renderer = new TiffRenderer( tiffFileName );
    m_mapOffset = QPoint( 0, 0 );
}

void MainWindow::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRect rect = boundingRect().toRect();
    rect.adjust( -rect.left(), -rect.top(), -rect.left(), -rect.top() );
    QRect imageRect = QRect( QPoint(0, 0), m_renderer->imageSize() ).intersect( rect );

    m_renderer->render( QPoint(0, 0), m_mapOffset, imageRect.size(), painter );
}

void MainWindow::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_isMousePressed = true;
    m_prevMousePos = event->pos().toPoint();
}

void MainWindow::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    m_isMousePressed = false;
}

void MainWindow::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if ( ! m_isMousePressed )
    {
        return;
    }

    QPoint mousePos = event->pos().toPoint();
    QPoint mouseDiff = event->pos().toPoint() - m_prevMousePos;
    m_prevMousePos = event->pos().toPoint();

    m_mapOffset -= mouseDiff;
    if ( m_mapOffset.x() < 0 )
    {
        m_mapOffset.setX( 0 );
    }
    if ( m_mapOffset.y() < 0 )
    {
        m_mapOffset.setY( 0 );
    }

    update();
}

