#include "TiffRenderer.h"

#include <QPainter>
#include <QDebug>

#include <tiffio.h>
#include <xtiffio.h>
#include <geotiff.h>
#include "geo_normalize.h"
#include "geo_simpletags.h"
#include "geovalues.h"

#include <cassert>
#include <iostream>
#include <stdio.h>
#include <math.h>

TiffRenderer::TiffRenderer(const QString &fileName)
    : m_fileName( fileName )
    , m_isValid( false )
{
//    TIFF* tif = TIFFOpen( fileName.toStdString().c_str(), "r" );
    TIFF* tif = XTIFFOpen( fileName.toStdString().c_str(), "r" );

    assert( tif && "Error opening TIFF" );

    GTIF* gtif = GTIFNew( tif );

    if ( gtif )
    {
        int     nKeyCount = 0;
        int     anVersion[3];
        GTIFDirectoryInfo( gtif, anVersion, &nKeyCount );

        GTIFDefn defn;

        if ( GTIFGetDefn( gtif, &defn ) )
        {
            GTIFPrintDefn( &defn, stdout );
        }
        else
        {
            qDebug() << "Failed to get GeoTiff definition";
        }
    }
    else
    {
        qDebug() << "GTIFF creation failed";
    }

    m_isValid = TIFFIsTiled( tif );

    if ( m_isValid )
    {
        uint32 imageWidth, imageLength;
        uint32 tileWidth, tileLength;

        TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &imageWidth);
        TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &imageLength);
        TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tileWidth);
        TIFFGetField(tif, TIFFTAG_TILELENGTH, &tileLength);

        m_tileSize = QSize( tileWidth, tileLength );
        m_imageSize = QSize( imageWidth, imageLength );

    }

    XTIFFClose( tif );
}

void TiffRenderer::render(const QPoint &targetLeftTop, const QPoint &srcLeftTop, const QSize &renderSize, QPainter *painter)
{
    TilePosition leftTopTile = pointToTilePos( srcLeftTop );
    QRect tiffRenderRect = QRect( srcLeftTop, renderSize );

    int rightTileIndex = leftTopTile.x + renderSize.width() / m_tileSize.width() + 1;
    int bottomTileIndex = leftTopTile.y + renderSize.height() / m_tileSize.height() + 1;

    int topRenderOffset = 0;
    for ( int tileIndexY = leftTopTile.y; tileIndexY <= bottomTileIndex; tileIndexY++ )
    {
        QRect srcRect;
        int leftRenderOffset = 0;
        for ( int tileIndexX = leftTopTile.x; tileIndexX <= rightTileIndex; tileIndexX++ )
        {
            TilePosition tilePos( tileIndexX, tileIndexY );
            QRect tileRect = privTileRect( tilePos ).intersect( tiffRenderRect );

            if ( tileRect.height() == 0 || tileRect.width() == 0 )
            {
                break;
            }

            TileMap::iterator it = m_tileMap.find( tilePos );
            ImageSp tile;
            if ( it == m_tileMap.end() )
            {
                tile = loadTile( tilePos );
                m_tileMap[ tilePos ] = tile;
            }
            else
            {
                tile = it->second;
            }

            QPoint tileLeftTop = privTileLeftTop( tilePos );
            srcRect = tileRect.adjusted( -tileLeftTop.x(), -tileLeftTop.y(), -tileLeftTop.x(), -tileLeftTop.y() );

            QPoint renderPos = targetLeftTop + QPoint( leftRenderOffset, topRenderOffset );

            painter->drawImage( renderPos, *tile, srcRect );

            leftRenderOffset += srcRect.width();
        }
        topRenderOffset += srcRect.height();
    }
}

QSize TiffRenderer::imageSize()
{
    return m_imageSize;
}

bool TiffRenderer::isValid()
{
    return m_isValid;
}

namespace
{

    static const double RadToDeg = 57.2957795130823208768;     /* 180.0 / M_PI */
    static const double EarthRadius = 6378388l;                /* Earth radius in meters */

    double latitudeMetricToDegree( double latitudeMetric )
    {
        return RadToDeg * ( 2 * atan( exp( latitudeMetric / EarthRadius ) ) - M_PI_2 );
    }

    double longitudeMetricToDegree( double longitudeMetric )
    {
        return longitudeMetric * RadToDeg / EarthRadius;
    }

}

TiffRenderer::Coords TiffRenderer::getPointCoords(const QPoint &imagePoint)
{
    TIFF* tif = XTIFFOpen( m_fileName.toStdString().c_str(), "r" );
    Coords result(0.0, 0.0);
    if ( tif ) {
        GTIF* gtif = GTIFNew( tif );

        if ( gtif ) {
            GTIFDefn defn;

            if ( GTIFGetDefn( gtif, &defn ) ) {
                double x = imagePoint.x();
                double y = imagePoint.y();

                if ( GTIFImageToPCS( gtif, &x, &y ) ) {
                    double lon = longitudeMetricToDegree( x );
                    double lat = latitudeMetricToDegree( y );
                    result = Coords( lat, lon );
                } else {
                    qDebug() << "Failed to confert image coords to geo coords";
                }
            } else {
                qDebug() << "Failed to read GTIFDefn";
            }

            GTIFFree( gtif );
        } else {
            qDebug() << "Failed to create geotiff handler";
        }

        XTIFFClose( tif );
    } else {
        qDebug() << "Failed to open tiff file";
    }

    return result;
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

TiffRenderer::ImageSp TiffRenderer::loadTile(const TiffRenderer::TilePosition &position)
{
    TIFF* tif = TIFFOpen( m_fileName.toStdString().c_str(), "r" );

    assert( tif && "Error opening TIFF" );

    QSize tileSize = privTileRect( position ).size();
    int tileArea = tileSize.height() * tileSize.width();
    uint32* buf = (uint32*) _TIFFmalloc( tileArea * 4 );
    TIFFReadRGBATile( tif, position.x * m_tileSize.width(), position.y * m_tileSize.height(), buf );

    TIFFClose( tif );

    QImage* image = new QImage( tileSize, QImage::Format_ARGB32 );
    for ( int row = 0; row < tileSize.height(); row++ ) {
        for ( int col = 0; col < tileSize.width(); col++ ) {
            uint32 pixelColor = *(buf + row * tileSize.width() + col);
            image->setPixel(
                        col,
                        tileSize.height() - row - 1,
                        qRgba( red(pixelColor),  green(pixelColor), blue(pixelColor), alpha(pixelColor) ) );
        }
    }

    return ImageSp( image );

}

TiffRenderer::TilePosition TiffRenderer::pointToTilePos(const QPoint &imagePoint) const
{
    return TilePosition( imagePoint.x() / m_tileSize.width(), imagePoint.y() / m_tileSize.height() );
}

QRect TiffRenderer::privTileRect(const TiffRenderer::TilePosition &position) const
{
    int x = m_tileSize.width() * position.x;
    int y = m_tileSize.height() * position.y;

    if ( x > m_imageSize.width() || y > m_imageSize.height() )
    {
        return QRect( x, y, 0, 0 );
    }

    QRect tileRect( QPoint(x, y), m_tileSize );

    if ( tileRect.right() >= m_imageSize.width() )
    {
        tileRect.setRight( m_imageSize.width() - 1 );
    }

    if ( tileRect.bottom() >= m_imageSize.height() )
    {
        tileRect.setBottom( m_imageSize.height() - 1 );
    }

    return tileRect;
}

QPoint TiffRenderer::privTileLeftTop(const TiffRenderer::TilePosition &position) const
{
    int x = m_tileSize.width() * position.x;
    int y = m_tileSize.height() * position.y;

    return QPoint( x, y );
}
