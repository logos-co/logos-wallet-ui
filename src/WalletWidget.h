#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QLabel>
#include <string>
#include "logos_api.h"
#include "logos_api_client.h"
#include "logos_sdk.h"

class WalletWidget : public QWidget {
    Q_OBJECT

public:
    explicit WalletWidget(QWidget* parent = nullptr);
    ~WalletWidget();

    // Wallet operations
    void initWallet();
    void stopWallet();

private slots:
    void onChainIDButtonClicked();
    void onEthBalanceButtonClicked();

private:
    // UI elements
    QVBoxLayout* mainLayout;

    QPushButton* chainIDButton;
    QLabel* chainIDLabel;

    QLineEdit* ethBalanceAddressInput;
    QPushButton* ethBalanceButton;
    QLabel* ethBalanceAddressLabel;
    QLabel* ethBalanceValueLabel;

    QLabel* statusLabel;

    // LogosAPI instance for remote method calls
    LogosAPI* m_logosAPI;
    LogosModules* logos;
    
    // Connection status
    bool isWalletInitialized;

    // Helper methods
    void updateStatus(const QString& message);
}; 
