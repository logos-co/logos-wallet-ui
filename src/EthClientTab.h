#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QLabel>
#include <QFrame>
#include <QComboBox>
#include "logos_sdk.h"

class EthClientTab : public QWidget {
    Q_OBJECT

public:
    explicit EthClientTab(LogosModules* logos, QWidget* parent = nullptr);
    ~EthClientTab() override = default;

    // Public methods
    void initializeDefaultClients();

signals:
    void statusChanged(const QString& message);

private slots:
    void onInitEthClientButtonClicked();
    void onRefreshEthClientsButtonClicked();
    void onCloseEthClientButtonClicked();
    void onEthClientSelectionChanged(int index);
    void onRpcCallButtonClicked();
    void onChainIDButtonClicked();
    void onEthBalanceButtonClicked();

private:
    // Helper methods
    void setupUI();
    void createManagementSection();
    void createRpcCallSection();
    void createChainIDSection();
    void createBalanceSection();
    QFrame* createSeparator();

    QString getSelectedRpcUrl();
    void refreshEthClientList();
    void updateUIState();
    void resetResults();
    bool isClientAlreadyInitialized(const QString& rpcUrl);

    // LogosModules instance
    LogosModules* m_logos;

    // Main layout
    QVBoxLayout* m_mainLayout;

    // Management section widgets
    QLineEdit* m_rpcUrlInput;
    QPushButton* m_initButton;
    QPushButton* m_refreshButton;
    QComboBox* m_clientSelector;
    QPushButton* m_closeButton;

    // RPC Call section widgets
    QPushButton* m_rpcCallButton;
    QLineEdit* m_rpcCallMethodInput;
    QLineEdit* m_rpcCallParamsInput;
    QTextEdit* m_rpcCallResultOutput;

    // Chain ID section widgets
    QPushButton* m_chainIDButton;
    QLineEdit* m_chainIDOutput;

    // Balance section widgets
    QLineEdit* m_balanceAddressInput;
    QPushButton* m_balanceButton;
    QLineEdit* m_balanceAddressOutput;
    QLineEdit* m_balanceValueOutput;

    // Store list of initialized clients
    QStringList m_initializedClients;
};
