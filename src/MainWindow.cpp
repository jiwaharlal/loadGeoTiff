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

void MainWindow::loadTiff(const QString &tiffFileName)
{
    TIFF* tif = TIFFOpen( tiffFileName.toStdString().c_str(), "r" );
    TIFFClose( tif );

    if ( m_image )
    {
        delete m_image;
    }

    m_image = new QPixmap( tiffFileName );
}

void MainWindow::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qDebug() << boundingRect();
    if ( ! m_image )
    {
        return;
    }

    painter->drawPixmap( 0, 0, *m_image );
}
