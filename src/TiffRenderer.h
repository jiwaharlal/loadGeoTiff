#ifndef __TIFF_RENDERER_H__
#define __TIFF_RENDERER_H__

#include "rl_ptr.h"

#include <QImage>
#include <QRect>

#include <map>

class QPoint;
class QPainter;
class QSize;

class TiffRenderer
{
    struct TilePosition
    {
        TilePosition( int aX, int aY ) : x(aX), y(aY) {
        }

        bool operator ==( const TilePosition& other ) const {
            return ( x == other.x ) && ( y == other.y );
        }

        bool operator <( const TilePosition& other ) const {
            if ( x < other.x ) {
                return true;
            }
            if ( x > other.x ) {
                return false;
            }

            return y < other.y;
        }

        int x;
        int y;
    };
public:
    struct Coords
    {
        Coords( double newLat, double newLon ) : lat(newLat), lon(newLon) {}
        double lat;
        double lon;
    };

public:
    TiffRenderer( const QString& fileName );

    void render( const QPoint& targetLeftTop, const QPoint& srcLeftTop, const QSize& renderSize, QPainter* painter );
    QSize imageSize();
    bool isValid();
    Coords getPointCoords( const QPoint& imagePoint );

private:
    typedef koki::rl_ptr< QImage > ImageSp;
    ImageSp loadTile( const TilePosition& position );
    TilePosition pointToTilePos( const QPoint& imagePoint ) const;
    QRect privTileRect( const TilePosition& position ) const;
    QPoint privTileLeftTop( const TilePosition& position ) const;


    typedef std::map< TilePosition, ImageSp > TileMap;
    TileMap m_tileMap;
    QSize m_tileSize;
    QSize m_imageSize;
    QString m_fileName;
    bool m_isValid;
};

#endif // __TIFF_RENDERER_H__
