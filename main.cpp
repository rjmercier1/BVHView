#include "mainwindow.h"
#include "myapplication.h"


int main(int argc, char *argv[])
{
    MyApplication a(argc, argv);
    MainWindow w;
    QObject::connect(&a, &MyApplication::fileOpen,
                         &w, &MainWindow::setText);
    if (argc > 1)
        w.setText(QString(argv[1]));
    w.show();

    return a.exec();
}
