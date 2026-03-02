#include "mainwindow.h"
#include "ConnectionCheckDialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ConnectionCheckDialog dlg;
    dlg.exec(); // просто показали, проверили, закрыли

    MainWindow w;
    w.show();

    return a.exec();
}
