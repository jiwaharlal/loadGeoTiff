#include "MainWindow.h"

#include <QPainter>
#include <QDebug>

#include <tiffio.h>

MainWindow::MainWindow(QGraphicsItem *parent /* = NULL */ )
    : QGraphicsWidget( parent )
    , m_image( NULL )
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
    TIFF* tif = TIFFOpen( tiffFileName.toStdString().c_str(), "r" );
    if ( tif )
    {
        uint32 imageWidth, imageLength;
        uint32 tileWidth, tileLength;
        uint32 x, y;
//        tdata_t buf;
        uint32* buf;

        TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &imageWidth);
        TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &imageLength);
        TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tileWidth);
        TIFFGetField(tif, TIFFTAG_TILELENGTH, &tileLength);

        buf = (uint32*) _TIFFmalloc( TIFFTileSize( tif ) * 4 );
        TIFFReadRGBATile( tif, 0, 0, buf );

//        QPixmap pixmap(tileWidth, tileLength);
        m_image = new QImage( tileWidth, tileLength, QImage::Format_ARGB32 );
        for ( int row = 0; row < tileLength; row++ ) {
            for ( int col = 0; col < tileWidth; col++ ) {
                uint32 pixelColor = *(buf + row * tileWidth + col);
//                m_image->setPixel( col, row, pixelColor );
                m_image->setPixel( col, tileLength - row - 1, qRgba( red(pixelColor), green(pixelColor), blue(pixelColor), alpha(pixelColor) ) );
            }
        }



//        buf = _TIFFmalloc(TIFFTileSize(tif));
//        for (y = 0; y < imageLength; y += tileLength) {
//            for (x = 0; x < imageWidth; x += tileWidth) {
//        TIFFReadTile(tif, buf, 0, 0, 0);
//            }
//        }
        _TIFFfree(buf);

//        int dirCount = 0;
//        do {
//            dirCount++;
//        } while ( TIFFReadDirectory( tif ) );

//        qDebug() << "Found " << dirCount << " directories int " << tiffFileName;

        qDebug() << "Image dimensions: " << imageWidth << ", " << imageLength;
        qDebug() << "Tile dimansions: " << tileWidth << ", " << tileLength;
        qDebug() << "Buffer size: " << TIFFTileSize( tif );
        TIFFClose( tif );
    }

//    if ( m_image )
//    {
//        delete m_image;
//    }

//    m_image = new QPixmap( tiffFileName );
}

void MainWindow::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qDebug() << boundingRect();
    if ( ! m_image )
    {
        return;
    }

    painter->drawImage( 0, 0, *m_image );
}
