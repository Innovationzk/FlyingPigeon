#include "uicontroller.h"
#include <QtWidgets/QApplication>
#include "clientapp/clientapp.h"
#include "commondef.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UIController ui;

    CLIENTAPP::Init();

    // 设置service回调函数
    CLIENTAPP::SetCallBackFunc(UIController::serviceCallBackFunc);
    //qRegisterMetaType<u16>("u16");
    //QObject::connect(&ui, SIGNAL(signalServiceCallBack(u16, const void*, u16)), &ui, SLOT(onSignalServiceCallBack(u16, const void*, u16)));

    return a.exec();
}
