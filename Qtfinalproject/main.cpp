#include "Qtproject.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Qtproject w;
    w.show(); // 僅顯示主視窗
    return a.exec();
}
