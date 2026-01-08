#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QTabWidget>
#include "logos_api.h"
#include "logos_api_client.h"
#include "logos_sdk.h"
#include "EthClientTab.h"
#include "TransactionsTab.h"

class WalletWidget : public QWidget {
    Q_OBJECT

public:
    explicit WalletWidget(QWidget* parent = nullptr);
    ~WalletWidget();

    // Wallet operations
    void initWallet();
    void stopWallet();

private slots:
    void updateStatus(const QString& message);

private:
    // UI elements
    QVBoxLayout* m_mainLayout;
    QTabWidget* m_tabWidget;
    QLabel* m_statusLabel;

    // Tab instances
    EthClientTab* m_ethClientTab;
    TransactionsTab* m_transactionsTab;

    // LogosAPI instance for remote method calls
    LogosAPI* m_logosAPI;
    LogosModules* m_logos;
}; 
