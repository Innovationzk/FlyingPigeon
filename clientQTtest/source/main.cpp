#include <QtWidgets/QApplication>
#include "uicontroller.h"

QApplication *application;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UIController ui;

    return a.exec();
}
