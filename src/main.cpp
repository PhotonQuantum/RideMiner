#include <QApplication>
#include "mainwindow.h"
#include "loaderdialog.h"
#include "types.h"

int main(int argc, char *argv[]) {
    int result = 0;

    do {
        QApplication app(argc, argv);
        MainWindow window;
        LoaderDialog dialog;
        dialog.open();

        QApplication::connect(&dialog, &LoaderDialog::accepted, [&window](){
            window.show();
            QMetaObject::invokeMethod(&window, "Show");
        });

        result = QApplication::exec();
    } while (result == 114514);

    return result;
}
