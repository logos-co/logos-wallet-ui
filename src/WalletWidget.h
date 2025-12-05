#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QLabel>
#include <QFrame>
#include <QListWidget>
#include <QScrollArea>
#include <QTabWidget>
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
    void onRpcCallButtonClicked();
    void onChainIDButtonClicked();
    void onEthBalanceButtonClicked();
    
    // Keystore operations
    void onKeystoreInitButtonClicked();
    void onKeystoreListAccountsButtonClicked();
    void onKeystoreNewAccountButtonClicked();
    void onKeystoreImportButtonClicked();
    void onKeystoreImportECDSAButtonClicked();
    void onKeystoreExportButtonClicked();
    void onKeystoreDeleteButtonClicked();
    void onKeystoreUnlockButtonClicked();
    void onKeystoreLockButtonClicked();
    void onKeystoreSignHashButtonClicked();
    
    // Extended keystore operations
    void onExtKeystoreInitButtonClicked();
    void onExtKeystoreListAccountsButtonClicked();
    void onExtKeystoreNewAccountButtonClicked();
    void onExtKeystoreImportButtonClicked();
    void onExtKeystoreImportExtendedKeyButtonClicked();
    void onExtKeystoreExportButtonClicked();
    void onExtKeystoreExportPrivButtonClicked();
    void onExtKeystoreDeleteButtonClicked();
    void onExtKeystoreUnlockButtonClicked();
    void onExtKeystoreLockButtonClicked();
    void onExtKeystoreSignHashButtonClicked();
    void onExtKeystoreDeriveButtonClicked();
    
    // Mnemonic operations
    void onCreateMnemonicButtonClicked();
    
    // Key operations
    void onCreateExtKeyFromMnemonicButtonClicked();
    void onDeriveExtKeyButtonClicked();

private:
    // UI elements
    QVBoxLayout* mainLayout;
    QTabWidget* tabWidget;
    QWidget* accountsTab;
    QWidget* ethClientTab;
    QScrollArea* accountsScrollArea;
    QWidget* accountsScrollContent;
    QVBoxLayout* accountsTabLayout;
    QVBoxLayout* ethClientTabLayout;

    QPushButton* rpcCallButton;
    QLineEdit* rpcCallMethodInput;
    QLineEdit* rpcCallParamsInput;
    QLabel* rpcCallResultLabel;

    QPushButton* chainIDButton;
    QLabel* chainIDLabel;

    QLineEdit* ethBalanceAddressInput;
    QPushButton* ethBalanceButton;
    QLabel* ethBalanceAddressLabel;
    QLabel* ethBalanceValueLabel;

    QLabel* statusLabel;
    
    // Keystore UI elements
    QFrame* keystoreFrame;
    QLineEdit* keystoreDirInput;
    QLineEdit* keystoreScryptNInput;
    QLineEdit* keystoreScryptPInput;
    QPushButton* keystoreInitButton;
    QPushButton* keystoreListAccountsButton;
    QListWidget* keystoreAccountsList;
    QLineEdit* keystoreNewAccountPassphraseInput;
    QPushButton* keystoreNewAccountButton;
    QTextEdit* keystoreImportKeyJSONInput;
    QLineEdit* keystoreImportPassphraseInput;
    QLineEdit* keystoreImportNewPassphraseInput;
    QPushButton* keystoreImportButton;
    QLineEdit* keystoreImportECDSAKeyInput;
    QLineEdit* keystoreImportECDSAPassphraseInput;
    QPushButton* keystoreImportECDSAButton;
    QLineEdit* keystoreExportAddressInput;
    QLineEdit* keystoreExportPassphraseInput;
    QLineEdit* keystoreExportNewPassphraseInput;
    QPushButton* keystoreExportButton;
    QTextEdit* keystoreExportResult;
    QLineEdit* keystoreDeleteAddressInput;
    QLineEdit* keystoreDeletePassphraseInput;
    QPushButton* keystoreDeleteButton;
    QLineEdit* keystoreUnlockAddressInput;
    QLineEdit* keystoreUnlockPassphraseInput;
    QPushButton* keystoreUnlockButton;
    QLineEdit* keystoreLockAddressInput;
    QPushButton* keystoreLockButton;
    QLineEdit* keystoreSignHashAddressInput;
    QLineEdit* keystoreSignHashHashInput;
    QPushButton* keystoreSignHashButton;
    QLabel* keystoreSignHashResult;
    
    // Extended keystore UI elements
    QFrame* extKeystoreFrame;
    QLineEdit* extKeystoreDirInput;
    QLineEdit* extKeystoreScryptNInput;
    QLineEdit* extKeystoreScryptPInput;
    QPushButton* extKeystoreInitButton;
    QPushButton* extKeystoreListAccountsButton;
    QListWidget* extKeystoreAccountsList;
    QLineEdit* extKeystoreNewAccountPassphraseInput;
    QPushButton* extKeystoreNewAccountButton;
    QTextEdit* extKeystoreImportKeyJSONInput;
    QLineEdit* extKeystoreImportPassphraseInput;
    QLineEdit* extKeystoreImportNewPassphraseInput;
    QPushButton* extKeystoreImportButton;
    QLineEdit* extKeystoreImportExtendedKeyInput;
    QLineEdit* extKeystoreImportExtendedKeyPassphraseInput;
    QPushButton* extKeystoreImportExtendedKeyButton;
    QLineEdit* extKeystoreExportAddressInput;
    QLineEdit* extKeystoreExportPassphraseInput;
    QLineEdit* extKeystoreExportNewPassphraseInput;
    QPushButton* extKeystoreExportButton;
    QTextEdit* extKeystoreExportResult;
    QLineEdit* extKeystoreExportPrivAddressInput;
    QLineEdit* extKeystoreExportPrivPassphraseInput;
    QLineEdit* extKeystoreExportPrivNewPassphraseInput;
    QPushButton* extKeystoreExportPrivButton;
    QTextEdit* extKeystoreExportPrivResult;
    QLineEdit* extKeystoreDeleteAddressInput;
    QLineEdit* extKeystoreDeletePassphraseInput;
    QPushButton* extKeystoreDeleteButton;
    QLineEdit* extKeystoreUnlockAddressInput;
    QLineEdit* extKeystoreUnlockPassphraseInput;
    QPushButton* extKeystoreUnlockButton;
    QLineEdit* extKeystoreLockAddressInput;
    QPushButton* extKeystoreLockButton;
    QLineEdit* extKeystoreSignHashAddressInput;
    QLineEdit* extKeystoreSignHashHashInput;
    QPushButton* extKeystoreSignHashButton;
    QLabel* extKeystoreSignHashResult;
    QLineEdit* extKeystoreDeriveAddressInput;
    QLineEdit* extKeystoreDerivePathInput;
    QLineEdit* extKeystoreDerivePassphraseInput;
    QLineEdit* extKeystoreDeriveNewPassphraseInput;
    QPushButton* extKeystoreDeriveButton;
    QLabel* extKeystoreDeriveResult;
    
    // Mnemonic UI elements
    QFrame* mnemonicFrame;
    QLineEdit* mnemonicLengthInput;
    QPushButton* createMnemonicButton;
    QLabel* mnemonicResult;
    
    // Key operations UI elements
    QFrame* keyOpsFrame;
    QTextEdit* createExtKeyMnemonicInput;
    QLineEdit* createExtKeyPassphraseInput;
    QPushButton* createExtKeyFromMnemonicButton;
    QLabel* createExtKeyResult;
    QLineEdit* deriveExtKeyInput;
    QLineEdit* deriveExtKeyPathInput;
    QPushButton* deriveExtKeyButton;
    QLabel* deriveExtKeyResult;

    // LogosAPI instance for remote method calls
    LogosAPI* m_logosAPI;
    LogosModules* logos;
    
    // Connection status
    bool isWalletInitialized;

    // Helper methods
    void updateStatus(const QString& message);
}; 
