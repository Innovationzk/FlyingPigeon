#include "uicontroller.h"
#include <QtWidgets/QApplication>
#include "clientapp/clientapp.h"
#include "commondef.h"
#include "../common/json/json.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UIController ui;

    Json::Value root;
    root["123"]="123";

    CLIENTAPP::Init();

    // ����service�ص�����
    CLIENTAPP::SetCallBackFunc(UIController::serviceCallBackFunc);

    return a.exec();
}
