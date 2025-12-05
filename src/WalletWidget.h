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
    
    // Keystore operations (ordered as in interface)
    void onKeystoreInitButtonClicked();
    void onKeystoreCloseButtonClicked();
    void onKeystoreListAccountsButtonClicked();
    void onKeystoreNewAccountButtonClicked();
    void onKeystoreImportButtonClicked();
    void onKeystoreExportButtonClicked();
    void onKeystoreDeleteButtonClicked();
    void onKeystoreHasAddressButtonClicked();
    void onKeystoreUnlockButtonClicked();
    void onKeystoreLockButtonClicked();
    void onKeystoreTimedUnlockButtonClicked();
    void onKeystoreUpdateButtonClicked();
    void onKeystoreSignHashButtonClicked();
    void onKeystoreSignHashWithPassphraseButtonClicked();
    void onKeystoreImportECDSAButtonClicked();
    void onKeystoreSignTxButtonClicked();
    void onKeystoreSignTxWithPassphraseButtonClicked();
    void onKeystoreFindButtonClicked();
    
    // Extended keystore operations (ordered as in interface)
    void onExtKeystoreInitButtonClicked();
    void onExtKeystoreCloseButtonClicked();
    void onExtKeystoreListAccountsButtonClicked();
    void onExtKeystoreNewAccountButtonClicked();
    void onExtKeystoreImportButtonClicked();
    void onExtKeystoreImportExtendedKeyButtonClicked();
    void onExtKeystoreExportExtButtonClicked();
    void onExtKeystoreExportPrivButtonClicked();
    void onExtKeystoreDeleteButtonClicked();
    void onExtKeystoreHasAddressButtonClicked();
    void onExtKeystoreUnlockButtonClicked();
    void onExtKeystoreLockButtonClicked();
    void onExtKeystoreTimedUnlockButtonClicked();
    void onExtKeystoreUpdateButtonClicked();
    void onExtKeystoreSignHashButtonClicked();
    void onExtKeystoreSignHashWithPassphraseButtonClicked();
    void onExtKeystoreSignTxButtonClicked();
    void onExtKeystoreSignTxWithPassphraseButtonClicked();
    void onExtKeystoreDeriveButtonClicked();
    void onExtKeystoreDeriveWithPassphraseButtonClicked();
    void onExtKeystoreFindButtonClicked();
    
    // Key operations (ordered as in interface)
    void onCreateExtKeyFromMnemonicButtonClicked();
    void onDeriveExtKeyButtonClicked();
    void onExtKeyToECDSAButtonClicked();
    void onEcdsaToPublicKeyButtonClicked();
    void onPublicKeyToAddressButtonClicked();
    
    // Mnemonic operations (ordered as in interface)
    void onCreateMnemonicButtonClicked();
    void onLengthToEntropyStrengthButtonClicked();

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
    QPushButton* keystoreCloseButton;
    QPushButton* keystoreListAccountsButton;
    QListWidget* keystoreAccountsList;
    QLineEdit* keystoreNewAccountPassphraseInput;
    QPushButton* keystoreNewAccountButton;
    QTextEdit* keystoreImportKeyJSONInput;
    QLineEdit* keystoreImportPassphraseInput;
    QLineEdit* keystoreImportNewPassphraseInput;
    QPushButton* keystoreImportButton;
    QLineEdit* keystoreExportAddressInput;
    QLineEdit* keystoreExportPassphraseInput;
    QLineEdit* keystoreExportNewPassphraseInput;
    QPushButton* keystoreExportButton;
    QLabel* keystoreExportResultLabel;
    QTextEdit* keystoreExportResult;
    QLineEdit* keystoreDeleteAddressInput;
    QLineEdit* keystoreDeletePassphraseInput;
    QPushButton* keystoreDeleteButton;
    QLineEdit* keystoreHasAddressInput;
    QPushButton* keystoreHasAddressButton;
    QLabel* keystoreHasAddressResultLabel;
    QLineEdit* keystoreHasAddressResult;
    QLineEdit* keystoreUnlockAddressInput;
    QLineEdit* keystoreUnlockPassphraseInput;
    QPushButton* keystoreUnlockButton;
    QLineEdit* keystoreLockAddressInput;
    QPushButton* keystoreLockButton;
    QLineEdit* keystoreTimedUnlockAddressInput;
    QLineEdit* keystoreTimedUnlockPassphraseInput;
    QLineEdit* keystoreTimedUnlockTimeoutInput;
    QPushButton* keystoreTimedUnlockButton;
    QLineEdit* keystoreUpdateAddressInput;
    QLineEdit* keystoreUpdatePassphraseInput;
    QLineEdit* keystoreUpdateNewPassphraseInput;
    QPushButton* keystoreUpdateButton;
    QLineEdit* keystoreSignHashAddressInput;
    QLineEdit* keystoreSignHashHashInput;
    QPushButton* keystoreSignHashButton;
    QLabel* keystoreSignHashResultLabel;
    QLineEdit* keystoreSignHashResult;
    QLineEdit* keystoreSignHashWithPassphraseAddressInput;
    QLineEdit* keystoreSignHashWithPassphrasePassphraseInput;
    QLineEdit* keystoreSignHashWithPassphraseHashInput;
    QPushButton* keystoreSignHashWithPassphraseButton;
    QLabel* keystoreSignHashWithPassphraseResultLabel;
    QLineEdit* keystoreSignHashWithPassphraseResult;
    QLineEdit* keystoreImportECDSAPrivateKeyInput;
    QLineEdit* keystoreImportECDSAPassphraseInput;
    QPushButton* keystoreImportECDSAButton;
    QLabel* keystoreImportECDSAResultLabel;
    QLineEdit* keystoreImportECDSAResult;
    QLineEdit* keystoreSignTxAddressInput;
    QTextEdit* keystoreSignTxTxJSONInput;
    QLineEdit* keystoreSignTxChainIDInput;
    QPushButton* keystoreSignTxButton;
    QLabel* keystoreSignTxResultLabel;
    QTextEdit* keystoreSignTxResult;
    QLineEdit* keystoreSignTxWithPassphraseAddressInput;
    QLineEdit* keystoreSignTxWithPassphrasePassphraseInput;
    QTextEdit* keystoreSignTxWithPassphraseTxJSONInput;
    QLineEdit* keystoreSignTxWithPassphraseChainIDInput;
    QPushButton* keystoreSignTxWithPassphraseButton;
    QLabel* keystoreSignTxWithPassphraseResultLabel;
    QTextEdit* keystoreSignTxWithPassphraseResult;
    QLineEdit* keystoreFindAddressInput;
    QLineEdit* keystoreFindUrlInput;
    QPushButton* keystoreFindButton;
    QLabel* keystoreFindResultLabel;
    QLineEdit* keystoreFindResult;
    
    // Extended keystore UI elements
    QFrame* extKeystoreFrame;
    QLineEdit* extKeystoreDirInput;
    QLineEdit* extKeystoreScryptNInput;
    QLineEdit* extKeystoreScryptPInput;
    QPushButton* extKeystoreInitButton;
    QPushButton* extKeystoreCloseButton;
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
    QLabel* extKeystoreImportExtendedKeyResultLabel;
    QLineEdit* extKeystoreImportExtendedKeyResult;
    QLineEdit* extKeystoreExportExtAddressInput;
    QLineEdit* extKeystoreExportExtPassphraseInput;
    QLineEdit* extKeystoreExportExtNewPassphraseInput;
    QPushButton* extKeystoreExportExtButton;
    QLabel* extKeystoreExportExtResultLabel;
    QTextEdit* extKeystoreExportExtResult;
    QLineEdit* extKeystoreExportPrivAddressInput;
    QLineEdit* extKeystoreExportPrivPassphraseInput;
    QLineEdit* extKeystoreExportPrivNewPassphraseInput;
    QPushButton* extKeystoreExportPrivButton;
    QLabel* extKeystoreExportPrivResultLabel;
    QTextEdit* extKeystoreExportPrivResult;
    QLineEdit* extKeystoreDeleteAddressInput;
    QLineEdit* extKeystoreDeletePassphraseInput;
    QPushButton* extKeystoreDeleteButton;
    QLineEdit* extKeystoreHasAddressInput;
    QPushButton* extKeystoreHasAddressButton;
    QLabel* extKeystoreHasAddressResultLabel;
    QLineEdit* extKeystoreHasAddressResult;
    QLineEdit* extKeystoreUnlockAddressInput;
    QLineEdit* extKeystoreUnlockPassphraseInput;
    QPushButton* extKeystoreUnlockButton;
    QLineEdit* extKeystoreLockAddressInput;
    QPushButton* extKeystoreLockButton;
    QLineEdit* extKeystoreTimedUnlockAddressInput;
    QLineEdit* extKeystoreTimedUnlockPassphraseInput;
    QLineEdit* extKeystoreTimedUnlockTimeoutInput;
    QPushButton* extKeystoreTimedUnlockButton;
    QLineEdit* extKeystoreUpdateAddressInput;
    QLineEdit* extKeystoreUpdatePassphraseInput;
    QLineEdit* extKeystoreUpdateNewPassphraseInput;
    QPushButton* extKeystoreUpdateButton;
    QLineEdit* extKeystoreSignHashAddressInput;
    QLineEdit* extKeystoreSignHashHashInput;
    QPushButton* extKeystoreSignHashButton;
    QLabel* extKeystoreSignHashResultLabel;
    QLineEdit* extKeystoreSignHashResult;
    QLineEdit* extKeystoreSignHashWithPassphraseAddressInput;
    QLineEdit* extKeystoreSignHashWithPassphrasePassphraseInput;
    QLineEdit* extKeystoreSignHashWithPassphraseHashInput;
    QPushButton* extKeystoreSignHashWithPassphraseButton;
    QLabel* extKeystoreSignHashWithPassphraseResultLabel;
    QLineEdit* extKeystoreSignHashWithPassphraseResult;
    QLineEdit* extKeystoreSignTxAddressInput;
    QTextEdit* extKeystoreSignTxTxJSONInput;
    QLineEdit* extKeystoreSignTxChainIDInput;
    QPushButton* extKeystoreSignTxButton;
    QLabel* extKeystoreSignTxResultLabel;
    QTextEdit* extKeystoreSignTxResult;
    QLineEdit* extKeystoreSignTxWithPassphraseAddressInput;
    QLineEdit* extKeystoreSignTxWithPassphrasePassphraseInput;
    QTextEdit* extKeystoreSignTxWithPassphraseTxJSONInput;
    QLineEdit* extKeystoreSignTxWithPassphraseChainIDInput;
    QPushButton* extKeystoreSignTxWithPassphraseButton;
    QLabel* extKeystoreSignTxWithPassphraseResultLabel;
    QTextEdit* extKeystoreSignTxWithPassphraseResult;
    QLineEdit* extKeystoreDeriveAddressInput;
    QLineEdit* extKeystoreDerivePathInput;
    QLineEdit* extKeystoreDerivePinInput;
    QPushButton* extKeystoreDeriveButton;
    QLabel* extKeystoreDeriveResultLabel;
    QLineEdit* extKeystoreDeriveResult;
    QLineEdit* extKeystoreDeriveWithPassphraseAddressInput;
    QLineEdit* extKeystoreDeriveWithPassphrasePathInput;
    QLineEdit* extKeystoreDeriveWithPassphrasePinInput;
    QLineEdit* extKeystoreDeriveWithPassphrasePassphraseInput;
    QLineEdit* extKeystoreDeriveWithPassphraseNewPassphraseInput;
    QPushButton* extKeystoreDeriveWithPassphraseButton;
    QLabel* extKeystoreDeriveWithPassphraseResultLabel;
    QLineEdit* extKeystoreDeriveWithPassphraseResult;
    QLineEdit* extKeystoreFindAddressInput;
    QLineEdit* extKeystoreFindUrlInput;
    QPushButton* extKeystoreFindButton;
    QLabel* extKeystoreFindResultLabel;
    QLineEdit* extKeystoreFindResult;
    
    // Mnemonic UI elements
    QFrame* mnemonicFrame;
    QLineEdit* mnemonicLengthInput;
    QPushButton* createMnemonicButton;
    QLabel* mnemonicResultLabel;
    QTextEdit* mnemonicResult;
    QLineEdit* lengthToEntropyStrengthInput;
    QPushButton* lengthToEntropyStrengthButton;
    QLabel* lengthToEntropyStrengthResultLabel;
    QLineEdit* lengthToEntropyStrengthResult;
    
    // Key operations UI elements
    QFrame* keyOpsFrame;
    QTextEdit* createExtKeyMnemonicInput;
    QLineEdit* createExtKeyPassphraseInput;
    QPushButton* createExtKeyFromMnemonicButton;
    QLabel* createExtKeyResultLabel;
    QTextEdit* createExtKeyResult;
    QLineEdit* deriveExtKeyInput;
    QLineEdit* deriveExtKeyPathInput;
    QPushButton* deriveExtKeyButton;
    QLabel* deriveExtKeyResultLabel;
    QTextEdit* deriveExtKeyResult;
    QLineEdit* extKeyToECDSAInput;
    QPushButton* extKeyToECDSAButton;
    QLabel* extKeyToECDSAResultLabel;
    QLineEdit* extKeyToECDSAResult;
    QLineEdit* ecdsaToPublicKeyInput;
    QPushButton* ecdsaToPublicKeyButton;
    QLabel* ecdsaToPublicKeyResultLabel;
    QLineEdit* ecdsaToPublicKeyResult;
    QLineEdit* publicKeyToAddressInput;
    QPushButton* publicKeyToAddressButton;
    QLabel* publicKeyToAddressResultLabel;
    QLineEdit* publicKeyToAddressResult;

    // LogosAPI instance for remote method calls
    LogosAPI* m_logosAPI;
    LogosModules* logos;
    
    // Connection status
    bool isWalletInitialized;

    // Helper methods
    void updateStatus(const QString& message);
    QFrame* createSeparator();
};
