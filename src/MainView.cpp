#include "MainView.h"

#include <QKeyEvent>
#include <QDebug>
#include <QGraphicsWidget>

MainView::MainView(QGraphicsScene *scene)
    : QGraphicsView( scene )
    , m_mainWindow( NULL )
{
}

void MainView::setMainWindow(QGraphicsWidget *window)
{
    m_mainWindow = window;
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
    QRect newSceneRect( 0, 0, event->size().width(), event->size().height() );
    scene()->setSceneRect( 0, 0, event->size().width(), event->size().height() );
    if ( m_mainWindow )
    {
        m_mainWindow->resize( event->size() );
    }

    QGraphicsView::resizeEvent( event );
}
