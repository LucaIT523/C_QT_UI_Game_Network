#include "gamelauncher.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //a.setWindowIcon(QIcon(":/res/GameLauncher.ico"));
    GameLauncher w;
    w.show();
    return a.exec();
}
