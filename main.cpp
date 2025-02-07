#include <QApplication>
#include "lumepadmainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    LumePadMainWindow window;
    window.show();
    return app.exec();
}
