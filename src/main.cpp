#include <QApplication>
#include <QDebug>
#include "WalletWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    qDebug() << "Starting Wallet Qt Application";
    
    // Create and show the main window
    WalletWindow mainWindow;
    mainWindow.setWindowTitle("Logos Wallet App");
    mainWindow.resize(800, 600);
    mainWindow.show();
    
    return app.exec();
} 