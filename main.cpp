#include "rApp.h"
INITIALIZE_EASYLOGGINGPP;

int main(int argc, char *argv[]) {
    START_EASYLOGGINGPP(argc, argv);
    QApplication app(argc, argv);

    rAppMainView window;

    window.resize(600, 300);
    window.setWindowTitle("Rover");
    window.show();

    return app.exec();
}