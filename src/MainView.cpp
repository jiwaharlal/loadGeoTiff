#include "MainView.h"

#include <QKeyEvent>
#include <QDebug>

MainView::MainView(QGraphicsScene *scene)
    : QGraphicsView( scene )
{
}

void MainView::keyPressEvent(QKeyEvent *event)
{
    if ( event->key() == Qt::Key_Escape )
    {
        this->close();
    }
    else
    {
        QGraphicsView::keyPressEvent( event );
    }
}

void MainView::resizeEvent(QResizeEvent *event)
{
//    qDebug() << "resize event, new size: " << event->size();
    scene()->setSceneRect( 0, 0, event->size().width(), event->size().height() );

    QGraphicsView::resizeEvent( event );
}
