#ifndef MYAPPLICATION_H
#define MYAPPLICATION_H
#include <QApplication>
#include <QFileOpenEvent>

class MyApplication : public QApplication{
  Q_OBJECT

  public:
    MyApplication(int &argc, char **argv) : QApplication(argc, argv){}

  signals:
    void fileOpen(QString s);

  protected:
    bool event(QEvent *e){ // the open file event handler
      switch (e->type()) {
        // catch only FileOpen event
        case QEvent::FileOpen: {
          // cast file name from passed event
          const QString file_name = static_cast<QFileOpenEvent *>(e)->file();
          emit fileOpen(file_name);

          // handle file open here
          } //openFile(file_name);
          return true;
        default:
          return QApplication::event(e);
      }
    }
};

#endif // MYAPPLICATION_H
