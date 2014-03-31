#ifndef __MAIN_VIEW_H__
#define __MAIN_VIEW_H__

#include <QGraphicsView>

class MainView: public QGraphicsView
{
public:
    MainView(QGraphicsScene* scene );

    void setMainWindow( QGraphicsWidget* window );
protected:
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void resizeEvent(QResizeEvent *event);

private:
    QGraphicsWidget* m_mainWindow;
};

#endif // __MAIN_VIEW_H__
