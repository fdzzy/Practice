#include <QApplication>
#include "utcDialog.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    utcDialog *dialog = new utcDialog;
    dialog->show();

    return app.exec();
}
