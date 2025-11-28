#pragma once

#include <QMainWindow>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QStatusBar>
#include "WalletWidget.h"

class WalletWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit WalletWindow(QWidget* parent = nullptr);
    ~WalletWindow();

private slots:
    void onAboutAction();
    //void onInitWaku();
    //void onStopWaku();
    //void onRetrieveHistory();
    
private:
    void setupMenu();
    void setupStatusBar();
    
    WalletWidget* walletWidget;
    QStatusBar* statusBar;
}; 