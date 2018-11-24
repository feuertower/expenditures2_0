#include "guiobject.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    guiObject* window = new guiObject();
    window->show();

    return a.exec();
}
