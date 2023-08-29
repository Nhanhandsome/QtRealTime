#include "include/widget.h"
#include "QTimer"
#include <QApplication>




int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.setWindowTitle("Real Time Chart");
    w.show();
    return a.exec();
}
