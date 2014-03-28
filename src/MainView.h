#ifndef __MAIN_VIEW_H__
#define __MAIN_VIEW_H__

#include <QGraphicsView>

class MainView: public QGraphicsView
{
public:
    MainView(QGraphicsScene* scene );

protected:
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
};

#endif // __MAIN_VIEW_H__
