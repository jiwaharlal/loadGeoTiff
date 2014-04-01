#include <QApplication>
#include <QtGui>

#include <iostream>

#include "MainWindow.h"
#include "MainView.h"

int main(int argc, char *argv[])
{
    std::cout << "in main" << std::endl;

    if ( argc != 2 )
    {
        std::cout << "Usage: " << argv[0] << " <tiff filename>" << std::endl;
        return 0;
    }

    QApplication a(argc, argv);
    QGraphicsScene scene;
    MainWindow* w = new MainWindow();
    w->loadTiff( argv[1] );
//    scene.setSceneRect( 0, 0, 1429, 605 );
    scene.addItem(w);
    scene.setActiveWindow(w);
    MainView view(&scene);
    view.setMainWindow( w );
//    view.resize(1429, 605);
    // QObject::connect( &view, SIGNAL()
    view.show();
    
    return a.exec();
}

