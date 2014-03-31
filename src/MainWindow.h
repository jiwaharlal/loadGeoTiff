#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include <QGraphicsWidget>
#include <QPixmap>
#include <QImage>

class MainWindow: public QGraphicsWidget
{
public:
    MainWindow( QGraphicsItem* parent = NULL );

    void loadTiff( const QString& tiffFileName );

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
//    QPixmap* m_image;
    QImage* m_image;
};

#endif // __MAIN_WINDOW_H__
