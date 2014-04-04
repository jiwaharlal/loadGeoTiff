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

namespace
{
QString degreesAsDegMinSec( double degrees )
{
    int deg = static_cast<int>( degrees );
    degrees -= static_cast<double>( deg );
    double minutes = degrees * 60;
    int min = static_cast<int>( minutes );
    minutes -= static_cast<double>( min );
    int secs = static_cast<int>( minutes * 60 );

    return QString( "%1 %2' %3\"" ).arg(deg).arg(min).arg(secs);
}
}


void MainWindow::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if ( m_renderer->isValid() )
    {
        QRect rect = boundingRect().toRect();
        rect.adjust( -rect.left(), -rect.top(), -rect.left(), -rect.top() );
        QRect imageRect = QRect( QPoint(0, 0), m_renderer->imageSize() ).intersect( rect );

        m_renderer->render( QPoint(0, 0), m_mapOffset, imageRect.size(), painter );

        TiffRenderer::Coords coords = m_renderer->getPointCoords( m_mapOffset );
        qDebug() << "Map offset " << m_mapOffset;
        QString text = QString( "Lat: %1, Lon: %2").arg( coords.lat ).arg( coords.lon );
        QFontMetrics metrics( painter->font() );
        int rectWidth = metrics.width( text ) + 20;
        int rectHeight = metrics.height() + 14;
        QRect coordRect( 10, 10, rectWidth, rectHeight );
        painter->fillRect( coordRect, Qt::white );
        painter->setPen( Qt::red );
        painter->drawText( coordRect.adjusted( 10, 7, 0, 0 ), text );
        text = "Lat: " + degreesAsDegMinSec( coords.lat ) + " Lon: " + degreesAsDegMinSec( coords.lon );
        coordRect.setWidth( metrics.width(text) + 20 );
        coordRect.adjust( 0, coordRect.height(), 0, coordRect.height() );
        painter->fillRect( coordRect, Qt::white );
        painter->drawText( coordRect.adjusted( 10, 7, 0, 0 ), text );
    }
    else
    {
        painter->setPen( Qt::red );
        painter->drawText( 20, 20, "Tiff was not loaded" );
    }
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

