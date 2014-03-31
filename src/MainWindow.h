#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include <QGraphicsWidget>
#include <QPixmap>
#include <QImage>

class TiffRenderer;

class MainWindow: public QGraphicsWidget
{
public:
    MainWindow( QGraphicsItem* parent = NULL );

    void loadTiff( const QString& tiffFileName );

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
    TiffRenderer* m_renderer;
    QPoint m_mapOffset;
    QPoint m_prevMousePos;
    bool m_isMousePressed;
};

#endif // __MAIN_WINDOW_H__
