#include "WalletWidget.h"
#include <QDebug>
#include <QDateTime>
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonValue>
#include <QDir>
#include <iostream>
#include <csignal>
#include <QTimer>
#include "logos_api_client.h"

// Static pointer to the active WalletWidget for callbacks
static WalletWidget* activeWidget = nullptr;
/*
// Static callback that can be passed to the C API
void WalletWidget::handleWakuMessage(const std::string& timestamp, const std::string& nick, const std::string& message) {
    qDebug() << "RECEIVED: [" << QString::fromStdString(timestamp) << "] " 
             << QString::fromStdString(nick) << ": " 
             << QString::fromStdString(message);
    
    // Forward to the active widget if available
    if (activeWidget) {
        QMetaObject::invokeMethod(activeWidget, [=]() {
            activeWidget->displayMessage(QString::fromStdString(nick), QString::fromStdString(message));
        }, Qt::QueuedConnection);
    }
}
*/

WalletWidget::WalletWidget(QWidget* parent) 
    : QWidget(parent), 
      isWalletInitialized(false),
      m_logosAPI(nullptr) {
    
    // Set as the active widget
    activeWidget = this;
    
    m_logosAPI = new LogosAPI("core", this);
    logos = new LogosModules(m_logosAPI);
    
    // Main vertical layout
    mainLayout = new QVBoxLayout(this);
    
    // Create status frame
    QFrame* statusFrame = new QFrame(this);
    QVBoxLayout* statusLayout = new QVBoxLayout(statusFrame);
    statusFrame->setLayout(statusLayout);
    statusFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    statusFrame->setLineWidth(1);

    // Create status label
    statusLabel = new QLabel("Status: Not initialized", this);
    statusLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // Add status label to layout
    statusLayout->addWidget(statusLabel);
    
    // Create tab widget
    tabWidget = new QTabWidget(this);
    
    // Create Accounts tab with scroll area
    accountsTab = new QWidget(this);
    accountsScrollArea = new QScrollArea(accountsTab);
    accountsScrollArea->setWidgetResizable(true);
    accountsScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    accountsScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    accountsScrollContent = new QWidget();
    accountsTabLayout = new QVBoxLayout(accountsScrollContent);
    accountsTabLayout->setSpacing(10);
    accountsTabLayout->setContentsMargins(10, 10, 10, 10);
    
    accountsScrollArea->setWidget(accountsScrollContent);
    
    QVBoxLayout* accountsTabMainLayout = new QVBoxLayout(accountsTab);
    accountsTabMainLayout->setContentsMargins(0, 0, 0, 0);
    accountsTabMainLayout->addWidget(accountsScrollArea);
    
    // Create EthClient tab
    ethClientTab = new QWidget(this);
    ethClientTabLayout = new QVBoxLayout(ethClientTab);
    ethClientTabLayout->setSpacing(10);
    ethClientTabLayout->setContentsMargins(10, 10, 10, 10);

    // Create RPC Call frame
    QFrame* rpcCallFrame = new QFrame(ethClientTab);
    QVBoxLayout* rpcCallLayout = new QVBoxLayout(rpcCallFrame);
    rpcCallFrame->setLayout(rpcCallLayout);
    rpcCallFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    rpcCallFrame->setLineWidth(1);

    // Create RPC Call button
    rpcCallButton = new QPushButton("RPC Call", ethClientTab);

    // Create RPC Call method input
    rpcCallMethodInput = new QLineEdit(ethClientTab);
    rpcCallMethodInput->setPlaceholderText("Enter RPC Call Method...");

    // Create RPC Call params input
    rpcCallParamsInput = new QLineEdit(ethClientTab);
    rpcCallParamsInput->setPlaceholderText("Enter RPC Call Params...");

    // Create RPC Call result label
    rpcCallResultLabel = new QLabel("RPC Call Result: Not initialized", ethClientTab);
    rpcCallResultLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // Add RPC Call components to layout
    rpcCallLayout->addWidget(rpcCallButton);
    rpcCallLayout->addWidget(rpcCallMethodInput);
    rpcCallLayout->addWidget(rpcCallParamsInput);
    rpcCallLayout->addWidget(rpcCallResultLabel);

    // Create Chain ID frame
    QFrame* chainIDFrame = new QFrame(ethClientTab);
    QVBoxLayout* chainIDLayout = new QVBoxLayout(chainIDFrame);
    chainIDFrame->setLayout(chainIDLayout);
    chainIDFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    chainIDFrame->setLineWidth(1);

    // Create Chain ID button
    chainIDButton = new QPushButton("Get Chain ID", ethClientTab);
    
    // Create Chain ID label
    chainIDLabel = new QLabel("Chain ID: Not initialized", ethClientTab);

    // Add Chain ID components to layout
    chainIDLayout->addWidget(chainIDButton);
    chainIDLayout->addWidget(chainIDLabel);

    // Create ETH Balance frame
    QFrame* ethBalanceFrame = new QFrame(ethClientTab);
    QVBoxLayout* ethBalanceLayout = new QVBoxLayout(ethBalanceFrame);
    ethBalanceFrame->setLayout(ethBalanceLayout);
    ethBalanceFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    ethBalanceFrame->setLineWidth(1);

    // Create ETH Balance button
    ethBalanceButton = new QPushButton("Get ETH Balance", ethClientTab);
    
    // Create ETH Balance address input
    ethBalanceAddressInput = new QLineEdit(ethClientTab);
    ethBalanceAddressInput->setPlaceholderText("Enter ETH Balance Address...");

    // Create ETH Balance label
    ethBalanceAddressLabel = new QLabel("ETH Balance Address: Not initialized", ethClientTab);
    ethBalanceAddressLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // Create ETH Balance value label
    ethBalanceValueLabel = new QLabel("ETH Balance Value: Not initialized", ethClientTab);
    ethBalanceValueLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // Add ETH Balance components to layout
    ethBalanceLayout->addWidget(ethBalanceAddressInput);
    ethBalanceLayout->addWidget(ethBalanceButton);
    ethBalanceLayout->addWidget(ethBalanceAddressLabel);
    ethBalanceLayout->addWidget(ethBalanceValueLabel);

    // Create Keystore frame
    keystoreFrame = new QFrame(accountsScrollContent);
    QVBoxLayout* keystoreLayout = new QVBoxLayout(keystoreFrame);
    keystoreFrame->setLayout(keystoreLayout);
    keystoreFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    keystoreFrame->setLineWidth(1);
    
    QLabel* keystoreTitle = new QLabel("<b>Keystore Operations</b>", accountsScrollContent);
    keystoreLayout->addWidget(keystoreTitle);
    
    // Keystore Init
    QHBoxLayout* keystoreInitLayout = new QHBoxLayout();
    keystoreDirInput = new QLineEdit(accountsScrollContent);
    keystoreDirInput->setPlaceholderText("Keystore Directory...");
    QString keystoreTempDir = QDir::tempPath() + QDir::separator() + "logos_keystore";
    keystoreDirInput->setText(keystoreTempDir);
    keystoreScryptNInput = new QLineEdit(accountsScrollContent);
    keystoreScryptNInput->setPlaceholderText("Scrypt N (default: 262144)");
    keystoreScryptNInput->setText("262144");
    keystoreScryptPInput = new QLineEdit(accountsScrollContent);
    keystoreScryptPInput->setPlaceholderText("Scrypt P (default: 1)");
    keystoreScryptPInput->setText("1");
    keystoreInitButton = new QPushButton("Init Keystore", accountsScrollContent);
    keystoreInitLayout->addWidget(keystoreDirInput);
    keystoreInitLayout->addWidget(keystoreScryptNInput);
    keystoreInitLayout->addWidget(keystoreScryptPInput);
    keystoreInitLayout->addWidget(keystoreInitButton);
    keystoreLayout->addLayout(keystoreInitLayout);
    keystoreLayout->addWidget(createSeparator());
    
    // Keystore Close
    keystoreCloseButton = new QPushButton("Close Keystore", accountsScrollContent);
    keystoreLayout->addWidget(keystoreCloseButton);
    keystoreLayout->addWidget(createSeparator());
    
    // Keystore List Accounts
    QHBoxLayout* keystoreListLayout = new QHBoxLayout();
    keystoreListAccountsButton = new QPushButton("List Accounts", accountsScrollContent);
    keystoreAccountsList = new QListWidget(accountsScrollContent);
    keystoreAccountsList->setMaximumHeight(100);
    keystoreListLayout->addWidget(keystoreListAccountsButton);
    keystoreListLayout->addWidget(keystoreAccountsList, 1);
    keystoreLayout->addLayout(keystoreListLayout);
    keystoreLayout->addWidget(createSeparator());
    
    // Keystore New Account
    QHBoxLayout* keystoreNewAccountLayout = new QHBoxLayout();
    keystoreNewAccountPassphraseInput = new QLineEdit(accountsScrollContent);
    keystoreNewAccountPassphraseInput->setPlaceholderText("Passphrase...");
    keystoreNewAccountPassphraseInput->setEchoMode(QLineEdit::Password);
    keystoreNewAccountButton = new QPushButton("New Account", accountsScrollContent);
    keystoreNewAccountLayout->addWidget(keystoreNewAccountPassphraseInput);
    keystoreNewAccountLayout->addWidget(keystoreNewAccountButton);
    keystoreLayout->addLayout(keystoreNewAccountLayout);
    keystoreLayout->addWidget(createSeparator());
    
    // Keystore Import
    QLabel* keystoreImportLabel = new QLabel("Import Account:", accountsScrollContent);
    keystoreLayout->addWidget(keystoreImportLabel);
    keystoreImportKeyJSONInput = new QTextEdit(accountsScrollContent);
    keystoreImportKeyJSONInput->setPlaceholderText("Key JSON...");
    keystoreImportKeyJSONInput->setMaximumHeight(80);
    keystoreLayout->addWidget(keystoreImportKeyJSONInput);
    QHBoxLayout* keystoreImportLayout = new QHBoxLayout();
    keystoreImportPassphraseInput = new QLineEdit(accountsScrollContent);
    keystoreImportPassphraseInput->setPlaceholderText("Current Passphrase...");
    keystoreImportPassphraseInput->setEchoMode(QLineEdit::Password);
    keystoreImportNewPassphraseInput = new QLineEdit(accountsScrollContent);
    keystoreImportNewPassphraseInput->setPlaceholderText("New Passphrase...");
    keystoreImportNewPassphraseInput->setEchoMode(QLineEdit::Password);
    keystoreImportButton = new QPushButton("Import", accountsScrollContent);
    keystoreImportLayout->addWidget(keystoreImportPassphraseInput);
    keystoreImportLayout->addWidget(keystoreImportNewPassphraseInput);
    keystoreImportLayout->addWidget(keystoreImportButton);
    keystoreLayout->addLayout(keystoreImportLayout);
    keystoreLayout->addWidget(createSeparator());
    
    // Keystore Export
    QHBoxLayout* keystoreExportLayout = new QHBoxLayout();
    keystoreExportAddressInput = new QLineEdit(accountsScrollContent);
    keystoreExportAddressInput->setPlaceholderText("Address...");
    keystoreExportPassphraseInput = new QLineEdit(accountsScrollContent);
    keystoreExportPassphraseInput->setPlaceholderText("Passphrase...");
    keystoreExportPassphraseInput->setEchoMode(QLineEdit::Password);
    keystoreExportNewPassphraseInput = new QLineEdit(accountsScrollContent);
    keystoreExportNewPassphraseInput->setPlaceholderText("New Passphrase...");
    keystoreExportNewPassphraseInput->setEchoMode(QLineEdit::Password);
    keystoreExportButton = new QPushButton("Export", accountsScrollContent);
    keystoreExportLayout->addWidget(keystoreExportAddressInput);
    keystoreExportLayout->addWidget(keystoreExportPassphraseInput);
    keystoreExportLayout->addWidget(keystoreExportNewPassphraseInput);
    keystoreExportLayout->addWidget(keystoreExportButton);
    keystoreLayout->addLayout(keystoreExportLayout);
    keystoreExportResultLabel = new QLabel("Export Result:", accountsScrollContent);
    keystoreLayout->addWidget(keystoreExportResultLabel);
    keystoreExportResult = new QTextEdit(accountsScrollContent);
    keystoreExportResult->setPlaceholderText("Export result will appear here...");
    keystoreExportResult->setMaximumHeight(80);
    keystoreExportResult->setReadOnly(true);
    keystoreLayout->addWidget(keystoreExportResult);
    keystoreLayout->addWidget(createSeparator());
    
    // Keystore Delete
    QHBoxLayout* keystoreDeleteLayout = new QHBoxLayout();
    keystoreDeleteAddressInput = new QLineEdit(accountsScrollContent);
    keystoreDeleteAddressInput->setPlaceholderText("Address...");
    keystoreDeletePassphraseInput = new QLineEdit(accountsScrollContent);
    keystoreDeletePassphraseInput->setPlaceholderText("Passphrase...");
    keystoreDeletePassphraseInput->setEchoMode(QLineEdit::Password);
    keystoreDeleteButton = new QPushButton("Delete Account", accountsScrollContent);
    keystoreDeleteLayout->addWidget(keystoreDeleteAddressInput);
    keystoreDeleteLayout->addWidget(keystoreDeletePassphraseInput);
    keystoreDeleteLayout->addWidget(keystoreDeleteButton);
    keystoreLayout->addLayout(keystoreDeleteLayout);
    keystoreLayout->addWidget(createSeparator());
    
    // Keystore Has Address
    QHBoxLayout* keystoreHasAddressLayout = new QHBoxLayout();
    keystoreHasAddressInput = new QLineEdit(accountsScrollContent);
    keystoreHasAddressInput->setPlaceholderText("Address...");
    keystoreHasAddressButton = new QPushButton("Has Address", accountsScrollContent);
    keystoreHasAddressLayout->addWidget(keystoreHasAddressInput);
    keystoreHasAddressLayout->addWidget(keystoreHasAddressButton);
    keystoreLayout->addLayout(keystoreHasAddressLayout);
    keystoreHasAddressResultLabel = new QLabel("Has Address Result:", accountsScrollContent);
    keystoreLayout->addWidget(keystoreHasAddressResultLabel);
    keystoreHasAddressResult = new QLineEdit(accountsScrollContent);
    keystoreHasAddressResult->setReadOnly(true);
    keystoreHasAddressResult->setPlaceholderText("Result will appear here...");
    keystoreLayout->addWidget(keystoreHasAddressResult);
    keystoreLayout->addWidget(createSeparator());
    
    // Keystore Unlock/Lock
    QHBoxLayout* keystoreUnlockLayout = new QHBoxLayout();
    keystoreUnlockAddressInput = new QLineEdit(accountsScrollContent);
    keystoreUnlockAddressInput->setPlaceholderText("Address...");
    keystoreUnlockPassphraseInput = new QLineEdit(accountsScrollContent);
    keystoreUnlockPassphraseInput->setPlaceholderText("Passphrase...");
    keystoreUnlockPassphraseInput->setEchoMode(QLineEdit::Password);
    keystoreUnlockButton = new QPushButton("Unlock", accountsScrollContent);
    keystoreUnlockLayout->addWidget(keystoreUnlockAddressInput);
    keystoreUnlockLayout->addWidget(keystoreUnlockPassphraseInput);
    keystoreUnlockLayout->addWidget(keystoreUnlockButton);
    keystoreLayout->addLayout(keystoreUnlockLayout);
    
    QHBoxLayout* keystoreLockLayout = new QHBoxLayout();
    keystoreLockAddressInput = new QLineEdit(accountsScrollContent);
    keystoreLockAddressInput->setPlaceholderText("Address...");
    keystoreLockButton = new QPushButton("Lock", accountsScrollContent);
    keystoreLockLayout->addWidget(keystoreLockAddressInput);
    keystoreLockLayout->addWidget(keystoreLockButton);
    keystoreLayout->addLayout(keystoreLockLayout);
    keystoreLayout->addWidget(createSeparator());
    
    // Keystore Timed Unlock
    QHBoxLayout* keystoreTimedUnlockLayout = new QHBoxLayout();
    keystoreTimedUnlockAddressInput = new QLineEdit(accountsScrollContent);
    keystoreTimedUnlockAddressInput->setPlaceholderText("Address...");
    keystoreTimedUnlockPassphraseInput = new QLineEdit(accountsScrollContent);
    keystoreTimedUnlockPassphraseInput->setPlaceholderText("Passphrase...");
    keystoreTimedUnlockPassphraseInput->setEchoMode(QLineEdit::Password);
    keystoreTimedUnlockTimeoutInput = new QLineEdit(accountsScrollContent);
    keystoreTimedUnlockTimeoutInput->setPlaceholderText("Timeout (seconds)...");
    keystoreTimedUnlockButton = new QPushButton("Timed Unlock", accountsScrollContent);
    keystoreTimedUnlockLayout->addWidget(keystoreTimedUnlockAddressInput);
    keystoreTimedUnlockLayout->addWidget(keystoreTimedUnlockPassphraseInput);
    keystoreTimedUnlockLayout->addWidget(keystoreTimedUnlockTimeoutInput);
    keystoreTimedUnlockLayout->addWidget(keystoreTimedUnlockButton);
    keystoreLayout->addLayout(keystoreTimedUnlockLayout);
    keystoreLayout->addWidget(createSeparator());
    
    // Keystore Update
    QHBoxLayout* keystoreUpdateLayout = new QHBoxLayout();
    keystoreUpdateAddressInput = new QLineEdit(accountsScrollContent);
    keystoreUpdateAddressInput->setPlaceholderText("Address...");
    keystoreUpdatePassphraseInput = new QLineEdit(accountsScrollContent);
    keystoreUpdatePassphraseInput->setPlaceholderText("Current Passphrase...");
    keystoreUpdatePassphraseInput->setEchoMode(QLineEdit::Password);
    keystoreUpdateNewPassphraseInput = new QLineEdit(accountsScrollContent);
    keystoreUpdateNewPassphraseInput->setPlaceholderText("New Passphrase...");
    keystoreUpdateNewPassphraseInput->setEchoMode(QLineEdit::Password);
    keystoreUpdateButton = new QPushButton("Update Passphrase", accountsScrollContent);
    keystoreUpdateLayout->addWidget(keystoreUpdateAddressInput);
    keystoreUpdateLayout->addWidget(keystoreUpdatePassphraseInput);
    keystoreUpdateLayout->addWidget(keystoreUpdateNewPassphraseInput);
    keystoreUpdateLayout->addWidget(keystoreUpdateButton);
    keystoreLayout->addLayout(keystoreUpdateLayout);
    keystoreLayout->addWidget(createSeparator());
    
    // Keystore Sign Hash
    QHBoxLayout* keystoreSignHashLayout = new QHBoxLayout();
    keystoreSignHashAddressInput = new QLineEdit(accountsScrollContent);
    keystoreSignHashAddressInput->setPlaceholderText("Address...");
    keystoreSignHashHashInput = new QLineEdit(accountsScrollContent);
    keystoreSignHashHashInput->setPlaceholderText("Hash (hex)...");
    keystoreSignHashButton = new QPushButton("Sign Hash", accountsScrollContent);
    keystoreSignHashLayout->addWidget(keystoreSignHashAddressInput);
    keystoreSignHashLayout->addWidget(keystoreSignHashHashInput);
    keystoreSignHashLayout->addWidget(keystoreSignHashButton);
    keystoreLayout->addLayout(keystoreSignHashLayout);
    keystoreSignHashResultLabel = new QLabel("Signature Result:", accountsScrollContent);
    keystoreLayout->addWidget(keystoreSignHashResultLabel);
    keystoreSignHashResult = new QLineEdit(accountsScrollContent);
    keystoreSignHashResult->setReadOnly(true);
    keystoreSignHashResult->setPlaceholderText("Signature result will appear here...");
    keystoreLayout->addWidget(keystoreSignHashResult);
    keystoreLayout->addWidget(createSeparator());
    
    // Keystore Sign Hash With Passphrase
    QHBoxLayout* keystoreSignHashWithPassphraseLayout = new QHBoxLayout();
    keystoreSignHashWithPassphraseAddressInput = new QLineEdit(accountsScrollContent);
    keystoreSignHashWithPassphraseAddressInput->setPlaceholderText("Address...");
    keystoreSignHashWithPassphrasePassphraseInput = new QLineEdit(accountsScrollContent);
    keystoreSignHashWithPassphrasePassphraseInput->setPlaceholderText("Passphrase...");
    keystoreSignHashWithPassphrasePassphraseInput->setEchoMode(QLineEdit::Password);
    keystoreSignHashWithPassphraseHashInput = new QLineEdit(accountsScrollContent);
    keystoreSignHashWithPassphraseHashInput->setPlaceholderText("Hash (hex)...");
    keystoreSignHashWithPassphraseButton = new QPushButton("Sign Hash With Passphrase", accountsScrollContent);
    keystoreSignHashWithPassphraseLayout->addWidget(keystoreSignHashWithPassphraseAddressInput);
    keystoreSignHashWithPassphraseLayout->addWidget(keystoreSignHashWithPassphrasePassphraseInput);
    keystoreSignHashWithPassphraseLayout->addWidget(keystoreSignHashWithPassphraseHashInput);
    keystoreSignHashWithPassphraseLayout->addWidget(keystoreSignHashWithPassphraseButton);
    keystoreLayout->addLayout(keystoreSignHashWithPassphraseLayout);
    keystoreSignHashWithPassphraseResultLabel = new QLabel("Signature Result:", accountsScrollContent);
    keystoreLayout->addWidget(keystoreSignHashWithPassphraseResultLabel);
    keystoreSignHashWithPassphraseResult = new QLineEdit(accountsScrollContent);
    keystoreSignHashWithPassphraseResult->setReadOnly(true);
    keystoreSignHashWithPassphraseResult->setPlaceholderText("Signature result will appear here...");
    keystoreLayout->addWidget(keystoreSignHashWithPassphraseResult);
    keystoreLayout->addWidget(createSeparator());
    
    // Keystore Import ECDSA
    QHBoxLayout* keystoreImportECDSALayout = new QHBoxLayout();
    keystoreImportECDSAPrivateKeyInput = new QLineEdit(accountsScrollContent);
    keystoreImportECDSAPrivateKeyInput->setPlaceholderText("Private Key (hex)...");
    keystoreImportECDSAPassphraseInput = new QLineEdit(accountsScrollContent);
    keystoreImportECDSAPassphraseInput->setPlaceholderText("Passphrase...");
    keystoreImportECDSAPassphraseInput->setEchoMode(QLineEdit::Password);
    keystoreImportECDSAButton = new QPushButton("Import ECDSA", accountsScrollContent);
    keystoreImportECDSALayout->addWidget(keystoreImportECDSAPrivateKeyInput);
    keystoreImportECDSALayout->addWidget(keystoreImportECDSAPassphraseInput);
    keystoreImportECDSALayout->addWidget(keystoreImportECDSAButton);
    keystoreLayout->addLayout(keystoreImportECDSALayout);
    keystoreImportECDSAResultLabel = new QLabel("Import Result:", accountsScrollContent);
    keystoreLayout->addWidget(keystoreImportECDSAResultLabel);
    keystoreImportECDSAResult = new QLineEdit(accountsScrollContent);
    keystoreImportECDSAResult->setReadOnly(true);
    keystoreImportECDSAResult->setPlaceholderText("Address will appear here...");
    keystoreLayout->addWidget(keystoreImportECDSAResult);
    keystoreLayout->addWidget(createSeparator());
    
    // Keystore Sign Tx
    QLabel* keystoreSignTxLabel = new QLabel("Sign Transaction:", accountsScrollContent);
    keystoreLayout->addWidget(keystoreSignTxLabel);
    keystoreSignTxAddressInput = new QLineEdit(accountsScrollContent);
    keystoreSignTxAddressInput->setPlaceholderText("Address...");
    keystoreSignTxTxJSONInput = new QTextEdit(accountsScrollContent);
    keystoreSignTxTxJSONInput->setPlaceholderText("Transaction JSON...");
    keystoreSignTxTxJSONInput->setMaximumHeight(80);
    keystoreLayout->addWidget(keystoreSignTxAddressInput);
    keystoreLayout->addWidget(keystoreSignTxTxJSONInput);
    QHBoxLayout* keystoreSignTxLayout = new QHBoxLayout();
    keystoreSignTxChainIDInput = new QLineEdit(accountsScrollContent);
    keystoreSignTxChainIDInput->setPlaceholderText("Chain ID (hex)...");
    keystoreSignTxButton = new QPushButton("Sign Transaction", accountsScrollContent);
    keystoreSignTxLayout->addWidget(keystoreSignTxChainIDInput);
    keystoreSignTxLayout->addWidget(keystoreSignTxButton);
    keystoreLayout->addLayout(keystoreSignTxLayout);
    keystoreSignTxResultLabel = new QLabel("Signed Transaction Result:", accountsScrollContent);
    keystoreLayout->addWidget(keystoreSignTxResultLabel);
    keystoreSignTxResult = new QTextEdit(accountsScrollContent);
    keystoreSignTxResult->setReadOnly(true);
    keystoreSignTxResult->setPlaceholderText("Signed transaction will appear here...");
    keystoreSignTxResult->setMaximumHeight(80);
    keystoreLayout->addWidget(keystoreSignTxResult);
    keystoreLayout->addWidget(createSeparator());
    
    // Keystore Sign Tx With Passphrase
    QLabel* keystoreSignTxWithPassphraseLabel = new QLabel("Sign Transaction With Passphrase:", accountsScrollContent);
    keystoreLayout->addWidget(keystoreSignTxWithPassphraseLabel);
    keystoreSignTxWithPassphraseAddressInput = new QLineEdit(accountsScrollContent);
    keystoreSignTxWithPassphraseAddressInput->setPlaceholderText("Address...");
    keystoreSignTxWithPassphrasePassphraseInput = new QLineEdit(accountsScrollContent);
    keystoreSignTxWithPassphrasePassphraseInput->setPlaceholderText("Passphrase...");
    keystoreSignTxWithPassphrasePassphraseInput->setEchoMode(QLineEdit::Password);
    keystoreSignTxWithPassphraseTxJSONInput = new QTextEdit(accountsScrollContent);
    keystoreSignTxWithPassphraseTxJSONInput->setPlaceholderText("Transaction JSON...");
    keystoreSignTxWithPassphraseTxJSONInput->setMaximumHeight(80);
    keystoreLayout->addWidget(keystoreSignTxWithPassphraseAddressInput);
    keystoreLayout->addWidget(keystoreSignTxWithPassphrasePassphraseInput);
    keystoreLayout->addWidget(keystoreSignTxWithPassphraseTxJSONInput);
    QHBoxLayout* keystoreSignTxWithPassphraseLayout = new QHBoxLayout();
    keystoreSignTxWithPassphraseChainIDInput = new QLineEdit(accountsScrollContent);
    keystoreSignTxWithPassphraseChainIDInput->setPlaceholderText("Chain ID (hex)...");
    keystoreSignTxWithPassphraseButton = new QPushButton("Sign Transaction With Passphrase", accountsScrollContent);
    keystoreSignTxWithPassphraseLayout->addWidget(keystoreSignTxWithPassphraseChainIDInput);
    keystoreSignTxWithPassphraseLayout->addWidget(keystoreSignTxWithPassphraseButton);
    keystoreLayout->addLayout(keystoreSignTxWithPassphraseLayout);
    keystoreSignTxWithPassphraseResultLabel = new QLabel("Signed Transaction Result:", accountsScrollContent);
    keystoreLayout->addWidget(keystoreSignTxWithPassphraseResultLabel);
    keystoreSignTxWithPassphraseResult = new QTextEdit(accountsScrollContent);
    keystoreSignTxWithPassphraseResult->setReadOnly(true);
    keystoreSignTxWithPassphraseResult->setPlaceholderText("Signed transaction will appear here...");
    keystoreSignTxWithPassphraseResult->setMaximumHeight(80);
    keystoreLayout->addWidget(keystoreSignTxWithPassphraseResult);
    keystoreLayout->addWidget(createSeparator());
    
    // Keystore Find
    QHBoxLayout* keystoreFindLayout = new QHBoxLayout();
    keystoreFindAddressInput = new QLineEdit(accountsScrollContent);
    keystoreFindAddressInput->setPlaceholderText("Address...");
    keystoreFindUrlInput = new QLineEdit(accountsScrollContent);
    keystoreFindUrlInput->setPlaceholderText("URL...");
    keystoreFindButton = new QPushButton("Find", accountsScrollContent);
    keystoreFindLayout->addWidget(keystoreFindAddressInput);
    keystoreFindLayout->addWidget(keystoreFindUrlInput);
    keystoreFindLayout->addWidget(keystoreFindButton);
    keystoreLayout->addLayout(keystoreFindLayout);
    keystoreFindResultLabel = new QLabel("Find Result:", accountsScrollContent);
    keystoreLayout->addWidget(keystoreFindResultLabel);
    keystoreFindResult = new QLineEdit(accountsScrollContent);
    keystoreFindResult->setReadOnly(true);
    keystoreFindResult->setPlaceholderText("Result will appear here...");
    keystoreLayout->addWidget(keystoreFindResult);
    
    // Create Extended Keystore frame
    extKeystoreFrame = new QFrame(accountsScrollContent);
    QVBoxLayout* extKeystoreLayout = new QVBoxLayout(extKeystoreFrame);
    extKeystoreFrame->setLayout(extKeystoreLayout);
    extKeystoreFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    extKeystoreFrame->setLineWidth(1);
    
    QLabel* extKeystoreTitle = new QLabel("<b>Extended Keystore Operations</b>", accountsScrollContent);
    extKeystoreLayout->addWidget(extKeystoreTitle);
    
    // Extended Keystore Init
    QHBoxLayout* extKeystoreInitLayout = new QHBoxLayout();
    extKeystoreDirInput = new QLineEdit(accountsScrollContent);
    extKeystoreDirInput->setPlaceholderText("Keystore Directory...");
    QString extKeystoreTempDir = QDir::tempPath() + QDir::separator() + "logos_ext_keystore";
    extKeystoreDirInput->setText(extKeystoreTempDir);
    extKeystoreScryptNInput = new QLineEdit(accountsScrollContent);
    extKeystoreScryptNInput->setPlaceholderText("Scrypt N (default: 262144)");
    extKeystoreScryptNInput->setText("262144");
    extKeystoreScryptPInput = new QLineEdit(accountsScrollContent);
    extKeystoreScryptPInput->setPlaceholderText("Scrypt P (default: 1)");
    extKeystoreScryptPInput->setText("1");
    extKeystoreInitButton = new QPushButton("Init Extended Keystore", accountsScrollContent);
    extKeystoreInitLayout->addWidget(extKeystoreDirInput);
    extKeystoreInitLayout->addWidget(extKeystoreScryptNInput);
    extKeystoreInitLayout->addWidget(extKeystoreScryptPInput);
    extKeystoreInitLayout->addWidget(extKeystoreInitButton);
    extKeystoreLayout->addLayout(extKeystoreInitLayout);
    extKeystoreLayout->addWidget(createSeparator());
    
    // Extended Keystore Close
    extKeystoreCloseButton = new QPushButton("Close Extended Keystore", accountsScrollContent);
    extKeystoreLayout->addWidget(extKeystoreCloseButton);
    extKeystoreLayout->addWidget(createSeparator());
    
    // Extended Keystore List Accounts
    QHBoxLayout* extKeystoreListLayout = new QHBoxLayout();
    extKeystoreListAccountsButton = new QPushButton("List Accounts", accountsScrollContent);
    extKeystoreAccountsList = new QListWidget(accountsScrollContent);
    extKeystoreAccountsList->setMaximumHeight(100);
    extKeystoreListLayout->addWidget(extKeystoreListAccountsButton);
    extKeystoreListLayout->addWidget(extKeystoreAccountsList, 1);
    extKeystoreLayout->addLayout(extKeystoreListLayout);
    extKeystoreLayout->addWidget(createSeparator());
    
    // Extended Keystore New Account
    QHBoxLayout* extKeystoreNewAccountLayout = new QHBoxLayout();
    extKeystoreNewAccountPassphraseInput = new QLineEdit(accountsScrollContent);
    extKeystoreNewAccountPassphraseInput->setPlaceholderText("Passphrase...");
    extKeystoreNewAccountPassphraseInput->setEchoMode(QLineEdit::Password);
    extKeystoreNewAccountButton = new QPushButton("New Account", accountsScrollContent);
    extKeystoreNewAccountLayout->addWidget(extKeystoreNewAccountPassphraseInput);
    extKeystoreNewAccountLayout->addWidget(extKeystoreNewAccountButton);
    extKeystoreLayout->addLayout(extKeystoreNewAccountLayout);
    extKeystoreLayout->addWidget(createSeparator());
    
    // Extended Keystore Import
    QLabel* extKeystoreImportLabel = new QLabel("Import Account:", accountsScrollContent);
    extKeystoreLayout->addWidget(extKeystoreImportLabel);
    extKeystoreImportKeyJSONInput = new QTextEdit(accountsScrollContent);
    extKeystoreImportKeyJSONInput->setPlaceholderText("Key JSON...");
    extKeystoreImportKeyJSONInput->setMaximumHeight(80);
    extKeystoreLayout->addWidget(extKeystoreImportKeyJSONInput);
    QHBoxLayout* extKeystoreImportLayout = new QHBoxLayout();
    extKeystoreImportPassphraseInput = new QLineEdit(accountsScrollContent);
    extKeystoreImportPassphraseInput->setPlaceholderText("Current Passphrase...");
    extKeystoreImportPassphraseInput->setEchoMode(QLineEdit::Password);
    extKeystoreImportNewPassphraseInput = new QLineEdit(accountsScrollContent);
    extKeystoreImportNewPassphraseInput->setPlaceholderText("New Passphrase...");
    extKeystoreImportNewPassphraseInput->setEchoMode(QLineEdit::Password);
    extKeystoreImportButton = new QPushButton("Import", accountsScrollContent);
    extKeystoreImportLayout->addWidget(extKeystoreImportPassphraseInput);
    extKeystoreImportLayout->addWidget(extKeystoreImportNewPassphraseInput);
    extKeystoreImportLayout->addWidget(extKeystoreImportButton);
    extKeystoreLayout->addLayout(extKeystoreImportLayout);
    extKeystoreLayout->addWidget(createSeparator());
    
    // Extended Keystore Import Extended Key
    QHBoxLayout* extKeystoreImportExtendedKeyLayout = new QHBoxLayout();
    extKeystoreImportExtendedKeyInput = new QLineEdit(accountsScrollContent);
    extKeystoreImportExtendedKeyInput->setPlaceholderText("Extended Key...");
    extKeystoreImportExtendedKeyPassphraseInput = new QLineEdit(accountsScrollContent);
    extKeystoreImportExtendedKeyPassphraseInput->setPlaceholderText("Passphrase...");
    extKeystoreImportExtendedKeyPassphraseInput->setEchoMode(QLineEdit::Password);
    extKeystoreImportExtendedKeyButton = new QPushButton("Import Extended Key", accountsScrollContent);
    extKeystoreImportExtendedKeyLayout->addWidget(extKeystoreImportExtendedKeyInput);
    extKeystoreImportExtendedKeyLayout->addWidget(extKeystoreImportExtendedKeyPassphraseInput);
    extKeystoreImportExtendedKeyLayout->addWidget(extKeystoreImportExtendedKeyButton);
    extKeystoreLayout->addLayout(extKeystoreImportExtendedKeyLayout);
    extKeystoreImportExtendedKeyResultLabel = new QLabel("Import Result:", accountsScrollContent);
    extKeystoreLayout->addWidget(extKeystoreImportExtendedKeyResultLabel);
    extKeystoreImportExtendedKeyResult = new QLineEdit(accountsScrollContent);
    extKeystoreImportExtendedKeyResult->setReadOnly(true);
    extKeystoreImportExtendedKeyResult->setPlaceholderText("Address will appear here...");
    extKeystoreLayout->addWidget(extKeystoreImportExtendedKeyResult);
    extKeystoreLayout->addWidget(createSeparator());
    
    // Extended Keystore Export Ext
    QHBoxLayout* extKeystoreExportExtLayout = new QHBoxLayout();
    extKeystoreExportExtAddressInput = new QLineEdit(accountsScrollContent);
    extKeystoreExportExtAddressInput->setPlaceholderText("Address...");
    extKeystoreExportExtPassphraseInput = new QLineEdit(accountsScrollContent);
    extKeystoreExportExtPassphraseInput->setPlaceholderText("Passphrase...");
    extKeystoreExportExtPassphraseInput->setEchoMode(QLineEdit::Password);
    extKeystoreExportExtNewPassphraseInput = new QLineEdit(accountsScrollContent);
    extKeystoreExportExtNewPassphraseInput->setPlaceholderText("New Passphrase...");
    extKeystoreExportExtNewPassphraseInput->setEchoMode(QLineEdit::Password);
    extKeystoreExportExtButton = new QPushButton("Export Extended Key", accountsScrollContent);
    extKeystoreExportExtLayout->addWidget(extKeystoreExportExtAddressInput);
    extKeystoreExportExtLayout->addWidget(extKeystoreExportExtPassphraseInput);
    extKeystoreExportExtLayout->addWidget(extKeystoreExportExtNewPassphraseInput);
    extKeystoreExportExtLayout->addWidget(extKeystoreExportExtButton);
    extKeystoreLayout->addLayout(extKeystoreExportExtLayout);
    extKeystoreExportExtResultLabel = new QLabel("Export Result:", accountsScrollContent);
    extKeystoreLayout->addWidget(extKeystoreExportExtResultLabel);
    extKeystoreExportExtResult = new QTextEdit(accountsScrollContent);
    extKeystoreExportExtResult->setReadOnly(true);
    extKeystoreExportExtResult->setPlaceholderText("Export result will appear here...");
    extKeystoreExportExtResult->setMaximumHeight(80);
    extKeystoreLayout->addWidget(extKeystoreExportExtResult);
    extKeystoreLayout->addWidget(createSeparator());
    
    // Extended Keystore Export Priv
    QHBoxLayout* extKeystoreExportPrivLayout = new QHBoxLayout();
    extKeystoreExportPrivAddressInput = new QLineEdit(accountsScrollContent);
    extKeystoreExportPrivAddressInput->setPlaceholderText("Address...");
    extKeystoreExportPrivPassphraseInput = new QLineEdit(accountsScrollContent);
    extKeystoreExportPrivPassphraseInput->setPlaceholderText("Passphrase...");
    extKeystoreExportPrivPassphraseInput->setEchoMode(QLineEdit::Password);
    extKeystoreExportPrivNewPassphraseInput = new QLineEdit(accountsScrollContent);
    extKeystoreExportPrivNewPassphraseInput->setPlaceholderText("New Passphrase...");
    extKeystoreExportPrivNewPassphraseInput->setEchoMode(QLineEdit::Password);
    extKeystoreExportPrivButton = new QPushButton("Export Private Key", accountsScrollContent);
    extKeystoreExportPrivLayout->addWidget(extKeystoreExportPrivAddressInput);
    extKeystoreExportPrivLayout->addWidget(extKeystoreExportPrivPassphraseInput);
    extKeystoreExportPrivLayout->addWidget(extKeystoreExportPrivNewPassphraseInput);
    extKeystoreExportPrivLayout->addWidget(extKeystoreExportPrivButton);
    extKeystoreLayout->addLayout(extKeystoreExportPrivLayout);
    extKeystoreExportPrivResultLabel = new QLabel("Export Result:", accountsScrollContent);
    extKeystoreLayout->addWidget(extKeystoreExportPrivResultLabel);
    extKeystoreExportPrivResult = new QTextEdit(accountsScrollContent);
    extKeystoreExportPrivResult->setReadOnly(true);
    extKeystoreExportPrivResult->setPlaceholderText("Export result will appear here...");
    extKeystoreExportPrivResult->setMaximumHeight(80);
    extKeystoreLayout->addWidget(extKeystoreExportPrivResult);
    extKeystoreLayout->addWidget(createSeparator());
    
    // Extended Keystore Delete
    QHBoxLayout* extKeystoreDeleteLayout = new QHBoxLayout();
    extKeystoreDeleteAddressInput = new QLineEdit(accountsScrollContent);
    extKeystoreDeleteAddressInput->setPlaceholderText("Address...");
    extKeystoreDeletePassphraseInput = new QLineEdit(accountsScrollContent);
    extKeystoreDeletePassphraseInput->setPlaceholderText("Passphrase...");
    extKeystoreDeletePassphraseInput->setEchoMode(QLineEdit::Password);
    extKeystoreDeleteButton = new QPushButton("Delete Account", accountsScrollContent);
    extKeystoreDeleteLayout->addWidget(extKeystoreDeleteAddressInput);
    extKeystoreDeleteLayout->addWidget(extKeystoreDeletePassphraseInput);
    extKeystoreDeleteLayout->addWidget(extKeystoreDeleteButton);
    extKeystoreLayout->addLayout(extKeystoreDeleteLayout);
    extKeystoreLayout->addWidget(createSeparator());
    
    // Extended Keystore Has Address
    QHBoxLayout* extKeystoreHasAddressLayout = new QHBoxLayout();
    extKeystoreHasAddressInput = new QLineEdit(accountsScrollContent);
    extKeystoreHasAddressInput->setPlaceholderText("Address...");
    extKeystoreHasAddressButton = new QPushButton("Has Address", accountsScrollContent);
    extKeystoreHasAddressLayout->addWidget(extKeystoreHasAddressInput);
    extKeystoreHasAddressLayout->addWidget(extKeystoreHasAddressButton);
    extKeystoreLayout->addLayout(extKeystoreHasAddressLayout);
    extKeystoreHasAddressResultLabel = new QLabel("Has Address Result:", accountsScrollContent);
    extKeystoreLayout->addWidget(extKeystoreHasAddressResultLabel);
    extKeystoreHasAddressResult = new QLineEdit(accountsScrollContent);
    extKeystoreHasAddressResult->setReadOnly(true);
    extKeystoreHasAddressResult->setPlaceholderText("Result will appear here...");
    extKeystoreLayout->addWidget(extKeystoreHasAddressResult);
    extKeystoreLayout->addWidget(createSeparator());
    
    // Extended Keystore Unlock
    QHBoxLayout* extKeystoreUnlockLayout = new QHBoxLayout();
    extKeystoreUnlockAddressInput = new QLineEdit(accountsScrollContent);
    extKeystoreUnlockAddressInput->setPlaceholderText("Address...");
    extKeystoreUnlockPassphraseInput = new QLineEdit(accountsScrollContent);
    extKeystoreUnlockPassphraseInput->setPlaceholderText("Passphrase...");
    extKeystoreUnlockPassphraseInput->setEchoMode(QLineEdit::Password);
    extKeystoreUnlockButton = new QPushButton("Unlock", accountsScrollContent);
    extKeystoreUnlockLayout->addWidget(extKeystoreUnlockAddressInput);
    extKeystoreUnlockLayout->addWidget(extKeystoreUnlockPassphraseInput);
    extKeystoreUnlockLayout->addWidget(extKeystoreUnlockButton);
    extKeystoreLayout->addLayout(extKeystoreUnlockLayout);
    extKeystoreLayout->addWidget(createSeparator());
    
    // Extended Keystore Lock
    QHBoxLayout* extKeystoreLockLayout = new QHBoxLayout();
    extKeystoreLockAddressInput = new QLineEdit(accountsScrollContent);
    extKeystoreLockAddressInput->setPlaceholderText("Address...");
    extKeystoreLockButton = new QPushButton("Lock", accountsScrollContent);
    extKeystoreLockLayout->addWidget(extKeystoreLockAddressInput);
    extKeystoreLockLayout->addWidget(extKeystoreLockButton);
    extKeystoreLayout->addLayout(extKeystoreLockLayout);
    extKeystoreLayout->addWidget(createSeparator());
    
    // Extended Keystore Timed Unlock
    QHBoxLayout* extKeystoreTimedUnlockLayout = new QHBoxLayout();
    extKeystoreTimedUnlockAddressInput = new QLineEdit(accountsScrollContent);
    extKeystoreTimedUnlockAddressInput->setPlaceholderText("Address...");
    extKeystoreTimedUnlockPassphraseInput = new QLineEdit(accountsScrollContent);
    extKeystoreTimedUnlockPassphraseInput->setPlaceholderText("Passphrase...");
    extKeystoreTimedUnlockPassphraseInput->setEchoMode(QLineEdit::Password);
    extKeystoreTimedUnlockTimeoutInput = new QLineEdit(accountsScrollContent);
    extKeystoreTimedUnlockTimeoutInput->setPlaceholderText("Timeout (seconds)...");
    extKeystoreTimedUnlockButton = new QPushButton("Timed Unlock", accountsScrollContent);
    extKeystoreTimedUnlockLayout->addWidget(extKeystoreTimedUnlockAddressInput);
    extKeystoreTimedUnlockLayout->addWidget(extKeystoreTimedUnlockPassphraseInput);
    extKeystoreTimedUnlockLayout->addWidget(extKeystoreTimedUnlockTimeoutInput);
    extKeystoreTimedUnlockLayout->addWidget(extKeystoreTimedUnlockButton);
    extKeystoreLayout->addLayout(extKeystoreTimedUnlockLayout);
    extKeystoreLayout->addWidget(createSeparator());
    
    // Extended Keystore Update
    QHBoxLayout* extKeystoreUpdateLayout = new QHBoxLayout();
    extKeystoreUpdateAddressInput = new QLineEdit(accountsScrollContent);
    extKeystoreUpdateAddressInput->setPlaceholderText("Address...");
    extKeystoreUpdatePassphraseInput = new QLineEdit(accountsScrollContent);
    extKeystoreUpdatePassphraseInput->setPlaceholderText("Current Passphrase...");
    extKeystoreUpdatePassphraseInput->setEchoMode(QLineEdit::Password);
    extKeystoreUpdateNewPassphraseInput = new QLineEdit(accountsScrollContent);
    extKeystoreUpdateNewPassphraseInput->setPlaceholderText("New Passphrase...");
    extKeystoreUpdateNewPassphraseInput->setEchoMode(QLineEdit::Password);
    extKeystoreUpdateButton = new QPushButton("Update Passphrase", accountsScrollContent);
    extKeystoreUpdateLayout->addWidget(extKeystoreUpdateAddressInput);
    extKeystoreUpdateLayout->addWidget(extKeystoreUpdatePassphraseInput);
    extKeystoreUpdateLayout->addWidget(extKeystoreUpdateNewPassphraseInput);
    extKeystoreUpdateLayout->addWidget(extKeystoreUpdateButton);
    extKeystoreLayout->addLayout(extKeystoreUpdateLayout);
    extKeystoreLayout->addWidget(createSeparator());
    
    // Extended Keystore Sign Hash
    QHBoxLayout* extKeystoreSignHashLayout = new QHBoxLayout();
    extKeystoreSignHashAddressInput = new QLineEdit(accountsScrollContent);
    extKeystoreSignHashAddressInput->setPlaceholderText("Address...");
    extKeystoreSignHashHashInput = new QLineEdit(accountsScrollContent);
    extKeystoreSignHashHashInput->setPlaceholderText("Hash (hex)...");
    extKeystoreSignHashButton = new QPushButton("Sign Hash", accountsScrollContent);
    extKeystoreSignHashLayout->addWidget(extKeystoreSignHashAddressInput);
    extKeystoreSignHashLayout->addWidget(extKeystoreSignHashHashInput);
    extKeystoreSignHashLayout->addWidget(extKeystoreSignHashButton);
    extKeystoreLayout->addLayout(extKeystoreSignHashLayout);
    extKeystoreSignHashResultLabel = new QLabel("Signature Result:", accountsScrollContent);
    extKeystoreLayout->addWidget(extKeystoreSignHashResultLabel);
    extKeystoreSignHashResult = new QLineEdit(accountsScrollContent);
    extKeystoreSignHashResult->setReadOnly(true);
    extKeystoreSignHashResult->setPlaceholderText("Signature result will appear here...");
    extKeystoreLayout->addWidget(extKeystoreSignHashResult);
    extKeystoreLayout->addWidget(createSeparator());
    
    // Extended Keystore Sign Hash With Passphrase
    QHBoxLayout* extKeystoreSignHashWithPassphraseLayout = new QHBoxLayout();
    extKeystoreSignHashWithPassphraseAddressInput = new QLineEdit(accountsScrollContent);
    extKeystoreSignHashWithPassphraseAddressInput->setPlaceholderText("Address...");
    extKeystoreSignHashWithPassphrasePassphraseInput = new QLineEdit(accountsScrollContent);
    extKeystoreSignHashWithPassphrasePassphraseInput->setPlaceholderText("Passphrase...");
    extKeystoreSignHashWithPassphrasePassphraseInput->setEchoMode(QLineEdit::Password);
    extKeystoreSignHashWithPassphraseHashInput = new QLineEdit(accountsScrollContent);
    extKeystoreSignHashWithPassphraseHashInput->setPlaceholderText("Hash (hex)...");
    extKeystoreSignHashWithPassphraseButton = new QPushButton("Sign Hash With Passphrase", accountsScrollContent);
    extKeystoreSignHashWithPassphraseLayout->addWidget(extKeystoreSignHashWithPassphraseAddressInput);
    extKeystoreSignHashWithPassphraseLayout->addWidget(extKeystoreSignHashWithPassphrasePassphraseInput);
    extKeystoreSignHashWithPassphraseLayout->addWidget(extKeystoreSignHashWithPassphraseHashInput);
    extKeystoreSignHashWithPassphraseLayout->addWidget(extKeystoreSignHashWithPassphraseButton);
    extKeystoreLayout->addLayout(extKeystoreSignHashWithPassphraseLayout);
    extKeystoreSignHashWithPassphraseResultLabel = new QLabel("Signature Result:", accountsScrollContent);
    extKeystoreLayout->addWidget(extKeystoreSignHashWithPassphraseResultLabel);
    extKeystoreSignHashWithPassphraseResult = new QLineEdit(accountsScrollContent);
    extKeystoreSignHashWithPassphraseResult->setReadOnly(true);
    extKeystoreSignHashWithPassphraseResult->setPlaceholderText("Signature result will appear here...");
    extKeystoreLayout->addWidget(extKeystoreSignHashWithPassphraseResult);
    extKeystoreLayout->addWidget(createSeparator());
    
    // Extended Keystore Sign Tx
    QLabel* extKeystoreSignTxLabel = new QLabel("Sign Transaction:", accountsScrollContent);
    extKeystoreLayout->addWidget(extKeystoreSignTxLabel);
    extKeystoreSignTxAddressInput = new QLineEdit(accountsScrollContent);
    extKeystoreSignTxAddressInput->setPlaceholderText("Address...");
    extKeystoreSignTxTxJSONInput = new QTextEdit(accountsScrollContent);
    extKeystoreSignTxTxJSONInput->setPlaceholderText("Transaction JSON...");
    extKeystoreSignTxTxJSONInput->setMaximumHeight(80);
    extKeystoreLayout->addWidget(extKeystoreSignTxAddressInput);
    extKeystoreLayout->addWidget(extKeystoreSignTxTxJSONInput);
    QHBoxLayout* extKeystoreSignTxLayout = new QHBoxLayout();
    extKeystoreSignTxChainIDInput = new QLineEdit(accountsScrollContent);
    extKeystoreSignTxChainIDInput->setPlaceholderText("Chain ID (hex)...");
    extKeystoreSignTxButton = new QPushButton("Sign Transaction", accountsScrollContent);
    extKeystoreSignTxLayout->addWidget(extKeystoreSignTxChainIDInput);
    extKeystoreSignTxLayout->addWidget(extKeystoreSignTxButton);
    extKeystoreLayout->addLayout(extKeystoreSignTxLayout);
    extKeystoreSignTxResultLabel = new QLabel("Signed Transaction Result:", accountsScrollContent);
    extKeystoreLayout->addWidget(extKeystoreSignTxResultLabel);
    extKeystoreSignTxResult = new QTextEdit(accountsScrollContent);
    extKeystoreSignTxResult->setReadOnly(true);
    extKeystoreSignTxResult->setPlaceholderText("Signed transaction will appear here...");
    extKeystoreSignTxResult->setMaximumHeight(80);
    extKeystoreLayout->addWidget(extKeystoreSignTxResult);
    extKeystoreLayout->addWidget(createSeparator());
    
    // Extended Keystore Sign Tx With Passphrase
    QLabel* extKeystoreSignTxWithPassphraseLabel = new QLabel("Sign Transaction With Passphrase:", accountsScrollContent);
    extKeystoreLayout->addWidget(extKeystoreSignTxWithPassphraseLabel);
    extKeystoreSignTxWithPassphraseAddressInput = new QLineEdit(accountsScrollContent);
    extKeystoreSignTxWithPassphraseAddressInput->setPlaceholderText("Address...");
    extKeystoreSignTxWithPassphrasePassphraseInput = new QLineEdit(accountsScrollContent);
    extKeystoreSignTxWithPassphrasePassphraseInput->setPlaceholderText("Passphrase...");
    extKeystoreSignTxWithPassphrasePassphraseInput->setEchoMode(QLineEdit::Password);
    extKeystoreSignTxWithPassphraseTxJSONInput = new QTextEdit(accountsScrollContent);
    extKeystoreSignTxWithPassphraseTxJSONInput->setPlaceholderText("Transaction JSON...");
    extKeystoreSignTxWithPassphraseTxJSONInput->setMaximumHeight(80);
    extKeystoreLayout->addWidget(extKeystoreSignTxWithPassphraseAddressInput);
    extKeystoreLayout->addWidget(extKeystoreSignTxWithPassphrasePassphraseInput);
    extKeystoreLayout->addWidget(extKeystoreSignTxWithPassphraseTxJSONInput);
    QHBoxLayout* extKeystoreSignTxWithPassphraseLayout = new QHBoxLayout();
    extKeystoreSignTxWithPassphraseChainIDInput = new QLineEdit(accountsScrollContent);
    extKeystoreSignTxWithPassphraseChainIDInput->setPlaceholderText("Chain ID (hex)...");
    extKeystoreSignTxWithPassphraseButton = new QPushButton("Sign Transaction With Passphrase", accountsScrollContent);
    extKeystoreSignTxWithPassphraseLayout->addWidget(extKeystoreSignTxWithPassphraseChainIDInput);
    extKeystoreSignTxWithPassphraseLayout->addWidget(extKeystoreSignTxWithPassphraseButton);
    extKeystoreLayout->addLayout(extKeystoreSignTxWithPassphraseLayout);
    extKeystoreSignTxWithPassphraseResultLabel = new QLabel("Signed Transaction Result:", accountsScrollContent);
    extKeystoreLayout->addWidget(extKeystoreSignTxWithPassphraseResultLabel);
    extKeystoreSignTxWithPassphraseResult = new QTextEdit(accountsScrollContent);
    extKeystoreSignTxWithPassphraseResult->setReadOnly(true);
    extKeystoreSignTxWithPassphraseResult->setPlaceholderText("Signed transaction will appear here...");
    extKeystoreSignTxWithPassphraseResult->setMaximumHeight(80);
    extKeystoreLayout->addWidget(extKeystoreSignTxWithPassphraseResult);
    extKeystoreLayout->addWidget(createSeparator());
    
    // Extended Keystore Derive
    QHBoxLayout* extKeystoreDeriveLayout = new QHBoxLayout();
    extKeystoreDeriveAddressInput = new QLineEdit(accountsScrollContent);
    extKeystoreDeriveAddressInput->setPlaceholderText("Address...");
    extKeystoreDerivePathInput = new QLineEdit(accountsScrollContent);
    extKeystoreDerivePathInput->setPlaceholderText("Derivation Path (e.g., m/44'/60'/0'/0/0)...");
    extKeystoreDerivePinInput = new QLineEdit(accountsScrollContent);
    extKeystoreDerivePinInput->setPlaceholderText("PIN...");
    extKeystoreDeriveButton = new QPushButton("Derive", accountsScrollContent);
    extKeystoreDeriveLayout->addWidget(extKeystoreDeriveAddressInput);
    extKeystoreDeriveLayout->addWidget(extKeystoreDerivePathInput);
    extKeystoreDeriveLayout->addWidget(extKeystoreDerivePinInput);
    extKeystoreDeriveLayout->addWidget(extKeystoreDeriveButton);
    extKeystoreLayout->addLayout(extKeystoreDeriveLayout);
    extKeystoreDeriveResultLabel = new QLabel("Derived Address Result:", accountsScrollContent);
    extKeystoreLayout->addWidget(extKeystoreDeriveResultLabel);
    extKeystoreDeriveResult = new QLineEdit(accountsScrollContent);
    extKeystoreDeriveResult->setReadOnly(true);
    extKeystoreDeriveResult->setPlaceholderText("Derived address will appear here...");
    extKeystoreLayout->addWidget(extKeystoreDeriveResult);
    extKeystoreLayout->addWidget(createSeparator());
    
    // Extended Keystore Derive With Passphrase
    QHBoxLayout* extKeystoreDeriveWithPassphraseLayout = new QHBoxLayout();
    extKeystoreDeriveWithPassphraseAddressInput = new QLineEdit(accountsScrollContent);
    extKeystoreDeriveWithPassphraseAddressInput->setPlaceholderText("Address...");
    extKeystoreDeriveWithPassphrasePathInput = new QLineEdit(accountsScrollContent);
    extKeystoreDeriveWithPassphrasePathInput->setPlaceholderText("Derivation Path...");
    extKeystoreDeriveWithPassphrasePinInput = new QLineEdit(accountsScrollContent);
    extKeystoreDeriveWithPassphrasePinInput->setPlaceholderText("PIN...");
    extKeystoreDeriveWithPassphrasePassphraseInput = new QLineEdit(accountsScrollContent);
    extKeystoreDeriveWithPassphrasePassphraseInput->setPlaceholderText("Passphrase...");
    extKeystoreDeriveWithPassphrasePassphraseInput->setEchoMode(QLineEdit::Password);
    extKeystoreDeriveWithPassphraseNewPassphraseInput = new QLineEdit(accountsScrollContent);
    extKeystoreDeriveWithPassphraseNewPassphraseInput->setPlaceholderText("New Passphrase...");
    extKeystoreDeriveWithPassphraseNewPassphraseInput->setEchoMode(QLineEdit::Password);
    extKeystoreDeriveWithPassphraseButton = new QPushButton("Derive With Passphrase", accountsScrollContent);
    extKeystoreDeriveWithPassphraseLayout->addWidget(extKeystoreDeriveWithPassphraseAddressInput);
    extKeystoreDeriveWithPassphraseLayout->addWidget(extKeystoreDeriveWithPassphrasePathInput);
    extKeystoreDeriveWithPassphraseLayout->addWidget(extKeystoreDeriveWithPassphrasePinInput);
    extKeystoreDeriveWithPassphraseLayout->addWidget(extKeystoreDeriveWithPassphrasePassphraseInput);
    extKeystoreDeriveWithPassphraseLayout->addWidget(extKeystoreDeriveWithPassphraseNewPassphraseInput);
    extKeystoreDeriveWithPassphraseLayout->addWidget(extKeystoreDeriveWithPassphraseButton);
    extKeystoreLayout->addLayout(extKeystoreDeriveWithPassphraseLayout);
    extKeystoreDeriveWithPassphraseResultLabel = new QLabel("Derived Address Result:", accountsScrollContent);
    extKeystoreLayout->addWidget(extKeystoreDeriveWithPassphraseResultLabel);
    extKeystoreDeriveWithPassphraseResult = new QLineEdit(accountsScrollContent);
    extKeystoreDeriveWithPassphraseResult->setReadOnly(true);
    extKeystoreDeriveWithPassphraseResult->setPlaceholderText("Derived address will appear here...");
    extKeystoreLayout->addWidget(extKeystoreDeriveWithPassphraseResult);
    extKeystoreLayout->addWidget(createSeparator());
    
    // Extended Keystore Find
    QHBoxLayout* extKeystoreFindLayout = new QHBoxLayout();
    extKeystoreFindAddressInput = new QLineEdit(accountsScrollContent);
    extKeystoreFindAddressInput->setPlaceholderText("Address...");
    extKeystoreFindUrlInput = new QLineEdit(accountsScrollContent);
    extKeystoreFindUrlInput->setPlaceholderText("URL...");
    extKeystoreFindButton = new QPushButton("Find", accountsScrollContent);
    extKeystoreFindLayout->addWidget(extKeystoreFindAddressInput);
    extKeystoreFindLayout->addWidget(extKeystoreFindUrlInput);
    extKeystoreFindLayout->addWidget(extKeystoreFindButton);
    extKeystoreLayout->addLayout(extKeystoreFindLayout);
    extKeystoreFindResultLabel = new QLabel("Find Result:", accountsScrollContent);
    extKeystoreLayout->addWidget(extKeystoreFindResultLabel);
    extKeystoreFindResult = new QLineEdit(accountsScrollContent);
    extKeystoreFindResult->setReadOnly(true);
    extKeystoreFindResult->setPlaceholderText("Result will appear here...");
    extKeystoreLayout->addWidget(extKeystoreFindResult);
    
    // Create Mnemonic frame
    mnemonicFrame = new QFrame(accountsScrollContent);
    QVBoxLayout* mnemonicLayout = new QVBoxLayout(mnemonicFrame);
    mnemonicFrame->setLayout(mnemonicLayout);
    mnemonicFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    mnemonicFrame->setLineWidth(1);
    
    QLabel* mnemonicTitle = new QLabel("<b>Mnemonic Operations</b>", accountsScrollContent);
    mnemonicLayout->addWidget(mnemonicTitle);
    
    QHBoxLayout* mnemonicLayout2 = new QHBoxLayout();
    mnemonicLengthInput = new QLineEdit(accountsScrollContent);
    mnemonicLengthInput->setPlaceholderText("Length (12, 15, 18, 21, 24) - leave empty for default");
    createMnemonicButton = new QPushButton("Create Random Mnemonic", accountsScrollContent);
    mnemonicLayout2->addWidget(mnemonicLengthInput);
    mnemonicLayout2->addWidget(createMnemonicButton);
    mnemonicLayout->addLayout(mnemonicLayout2);
    mnemonicResultLabel = new QLabel("Mnemonic Result:", accountsScrollContent);
    mnemonicLayout->addWidget(mnemonicResultLabel);
    mnemonicResult = new QTextEdit(accountsScrollContent);
    mnemonicResult->setReadOnly(true);
    mnemonicResult->setPlaceholderText("Mnemonic will appear here...");
    mnemonicResult->setMaximumHeight(60);
    mnemonicLayout->addWidget(mnemonicResult);
    mnemonicLayout->addWidget(createSeparator());
    
    // Length To Entropy Strength
    QHBoxLayout* lengthToEntropyStrengthLayout = new QHBoxLayout();
    lengthToEntropyStrengthInput = new QLineEdit(accountsScrollContent);
    lengthToEntropyStrengthInput->setPlaceholderText("Length (12, 15, 18, 21, 24)...");
    lengthToEntropyStrengthButton = new QPushButton("Length To Entropy Strength", accountsScrollContent);
    lengthToEntropyStrengthLayout->addWidget(lengthToEntropyStrengthInput);
    lengthToEntropyStrengthLayout->addWidget(lengthToEntropyStrengthButton);
    mnemonicLayout->addLayout(lengthToEntropyStrengthLayout);
    lengthToEntropyStrengthResultLabel = new QLabel("Entropy Strength Result:", accountsScrollContent);
    mnemonicLayout->addWidget(lengthToEntropyStrengthResultLabel);
    lengthToEntropyStrengthResult = new QLineEdit(accountsScrollContent);
    lengthToEntropyStrengthResult->setReadOnly(true);
    lengthToEntropyStrengthResult->setPlaceholderText("Entropy strength will appear here...");
    mnemonicLayout->addWidget(lengthToEntropyStrengthResult);
    
    // Create Key Operations frame
    keyOpsFrame = new QFrame(accountsScrollContent);
    QVBoxLayout* keyOpsLayout = new QVBoxLayout(keyOpsFrame);
    keyOpsFrame->setLayout(keyOpsLayout);
    keyOpsFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    keyOpsFrame->setLineWidth(1);
    
    QLabel* keyOpsTitle = new QLabel("<b>Key Operations</b>", accountsScrollContent);
    keyOpsLayout->addWidget(keyOpsTitle);
    
    // Create Extended Key from Mnemonic
    QLabel* createExtKeyLabel = new QLabel("Create Extended Key from Mnemonic:", accountsScrollContent);
    keyOpsLayout->addWidget(createExtKeyLabel);
    createExtKeyMnemonicInput = new QTextEdit(accountsScrollContent);
    createExtKeyMnemonicInput->setPlaceholderText("Mnemonic phrase...");
    createExtKeyMnemonicInput->setMaximumHeight(60);
    keyOpsLayout->addWidget(createExtKeyMnemonicInput);
    QHBoxLayout* createExtKeyLayout = new QHBoxLayout();
    createExtKeyPassphraseInput = new QLineEdit(accountsScrollContent);
    createExtKeyPassphraseInput->setPlaceholderText("Passphrase...");
    createExtKeyPassphraseInput->setEchoMode(QLineEdit::Password);
    createExtKeyFromMnemonicButton = new QPushButton("Create Extended Key", accountsScrollContent);
    createExtKeyLayout->addWidget(createExtKeyPassphraseInput);
    createExtKeyLayout->addWidget(createExtKeyFromMnemonicButton);
    keyOpsLayout->addLayout(createExtKeyLayout);
    createExtKeyResultLabel = new QLabel("Extended Key Result:", accountsScrollContent);
    keyOpsLayout->addWidget(createExtKeyResultLabel);
    createExtKeyResult = new QTextEdit(accountsScrollContent);
    createExtKeyResult->setReadOnly(true);
    createExtKeyResult->setPlaceholderText("Extended key will appear here...");
    createExtKeyResult->setMaximumHeight(60);
    keyOpsLayout->addWidget(createExtKeyResult);
    
    // Derive Extended Key
    QLabel* deriveExtKeyLabel = new QLabel("Derive Extended Key:", accountsScrollContent);
    keyOpsLayout->addWidget(deriveExtKeyLabel);
    QHBoxLayout* deriveExtKeyLayout = new QHBoxLayout();
    deriveExtKeyInput = new QLineEdit(accountsScrollContent);
    deriveExtKeyInput->setPlaceholderText("Extended Key...");
    deriveExtKeyPathInput = new QLineEdit(accountsScrollContent);
    deriveExtKeyPathInput->setPlaceholderText("Derivation Path (e.g., m/44'/60'/0'/0/0)...");
    deriveExtKeyButton = new QPushButton("Derive", accountsScrollContent);
    deriveExtKeyLayout->addWidget(deriveExtKeyInput);
    deriveExtKeyLayout->addWidget(deriveExtKeyPathInput);
    deriveExtKeyLayout->addWidget(deriveExtKeyButton);
    keyOpsLayout->addLayout(deriveExtKeyLayout);
    deriveExtKeyResultLabel = new QLabel("Derived Key Result:", accountsScrollContent);
    keyOpsLayout->addWidget(deriveExtKeyResultLabel);
    deriveExtKeyResult = new QTextEdit(accountsScrollContent);
    deriveExtKeyResult->setReadOnly(true);
    deriveExtKeyResult->setPlaceholderText("Derived key will appear here...");
    deriveExtKeyResult->setMaximumHeight(60);
    keyOpsLayout->addWidget(deriveExtKeyResult);
    keyOpsLayout->addWidget(createSeparator());
    
    // Ext Key To ECDSA
    QHBoxLayout* extKeyToECDSALayout = new QHBoxLayout();
    extKeyToECDSAInput = new QLineEdit(accountsScrollContent);
    extKeyToECDSAInput->setPlaceholderText("Extended Key...");
    extKeyToECDSAButton = new QPushButton("Ext Key To ECDSA", accountsScrollContent);
    extKeyToECDSALayout->addWidget(extKeyToECDSAInput);
    extKeyToECDSALayout->addWidget(extKeyToECDSAButton);
    keyOpsLayout->addLayout(extKeyToECDSALayout);
    extKeyToECDSAResultLabel = new QLabel("ECDSA Private Key Result:", accountsScrollContent);
    keyOpsLayout->addWidget(extKeyToECDSAResultLabel);
    extKeyToECDSAResult = new QLineEdit(accountsScrollContent);
    extKeyToECDSAResult->setReadOnly(true);
    extKeyToECDSAResult->setPlaceholderText("ECDSA private key will appear here...");
    keyOpsLayout->addWidget(extKeyToECDSAResult);
    keyOpsLayout->addWidget(createSeparator());
    
    // ECDSA To Public Key
    QHBoxLayout* ecdsaToPublicKeyLayout = new QHBoxLayout();
    ecdsaToPublicKeyInput = new QLineEdit(accountsScrollContent);
    ecdsaToPublicKeyInput->setPlaceholderText("ECDSA Private Key (hex)...");
    ecdsaToPublicKeyButton = new QPushButton("ECDSA To Public Key", accountsScrollContent);
    ecdsaToPublicKeyLayout->addWidget(ecdsaToPublicKeyInput);
    ecdsaToPublicKeyLayout->addWidget(ecdsaToPublicKeyButton);
    keyOpsLayout->addLayout(ecdsaToPublicKeyLayout);
    ecdsaToPublicKeyResultLabel = new QLabel("Public Key Result:", accountsScrollContent);
    keyOpsLayout->addWidget(ecdsaToPublicKeyResultLabel);
    ecdsaToPublicKeyResult = new QLineEdit(accountsScrollContent);
    ecdsaToPublicKeyResult->setReadOnly(true);
    ecdsaToPublicKeyResult->setPlaceholderText("Public key will appear here...");
    keyOpsLayout->addWidget(ecdsaToPublicKeyResult);
    keyOpsLayout->addWidget(createSeparator());
    
    // Public Key To Address
    QHBoxLayout* publicKeyToAddressLayout = new QHBoxLayout();
    publicKeyToAddressInput = new QLineEdit(accountsScrollContent);
    publicKeyToAddressInput->setPlaceholderText("Public Key (hex)...");
    publicKeyToAddressButton = new QPushButton("Public Key To Address", accountsScrollContent);
    publicKeyToAddressLayout->addWidget(publicKeyToAddressInput);
    publicKeyToAddressLayout->addWidget(publicKeyToAddressButton);
    keyOpsLayout->addLayout(publicKeyToAddressLayout);
    publicKeyToAddressResultLabel = new QLabel("Address Result:", accountsScrollContent);
    keyOpsLayout->addWidget(publicKeyToAddressResultLabel);
    publicKeyToAddressResult = new QLineEdit(accountsScrollContent);
    publicKeyToAddressResult->setReadOnly(true);
    publicKeyToAddressResult->setPlaceholderText("Address will appear here...");
    keyOpsLayout->addWidget(publicKeyToAddressResult);
    
    // Add accounts-related frames to Accounts tab (Mnemonic and Key Operations at top, then Keystore, then ExtKeystore)
    accountsTabLayout->addWidget(mnemonicFrame);
    accountsTabLayout->addWidget(keyOpsFrame);
    accountsTabLayout->addWidget(keystoreFrame);
    accountsTabLayout->addWidget(extKeystoreFrame);
    accountsTabLayout->addStretch();
    
    // Add eth client-related frames to EthClient tab
    ethClientTabLayout->addWidget(rpcCallFrame);
    ethClientTabLayout->addWidget(chainIDFrame);
    ethClientTabLayout->addWidget(ethBalanceFrame);
    ethClientTabLayout->addStretch();
    
    // Add tabs to tab widget
    tabWidget->addTab(accountsTab, "Accounts");
    tabWidget->addTab(ethClientTab, "EthClient");
    
    // Add status frame and tab widget to main layout
    mainLayout->addWidget(statusFrame);
    mainLayout->addWidget(tabWidget);

    // Set spacing and margins
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // Connect signals to slots
    connect(rpcCallButton, &QPushButton::clicked, this, &WalletWidget::onRpcCallButtonClicked);
    connect(chainIDButton, &QPushButton::clicked, this, &WalletWidget::onChainIDButtonClicked);
    connect(ethBalanceButton, &QPushButton::clicked, this, &WalletWidget::onEthBalanceButtonClicked);
    
    // Connect keystore signals (in interface order)
    connect(keystoreInitButton, &QPushButton::clicked, this, &WalletWidget::onKeystoreInitButtonClicked);
    connect(keystoreCloseButton, &QPushButton::clicked, this, &WalletWidget::onKeystoreCloseButtonClicked);
    connect(keystoreListAccountsButton, &QPushButton::clicked, this, &WalletWidget::onKeystoreListAccountsButtonClicked);
    connect(keystoreNewAccountButton, &QPushButton::clicked, this, &WalletWidget::onKeystoreNewAccountButtonClicked);
    connect(keystoreImportButton, &QPushButton::clicked, this, &WalletWidget::onKeystoreImportButtonClicked);
    connect(keystoreExportButton, &QPushButton::clicked, this, &WalletWidget::onKeystoreExportButtonClicked);
    connect(keystoreDeleteButton, &QPushButton::clicked, this, &WalletWidget::onKeystoreDeleteButtonClicked);
    connect(keystoreHasAddressButton, &QPushButton::clicked, this, &WalletWidget::onKeystoreHasAddressButtonClicked);
    connect(keystoreUnlockButton, &QPushButton::clicked, this, &WalletWidget::onKeystoreUnlockButtonClicked);
    connect(keystoreLockButton, &QPushButton::clicked, this, &WalletWidget::onKeystoreLockButtonClicked);
    connect(keystoreTimedUnlockButton, &QPushButton::clicked, this, &WalletWidget::onKeystoreTimedUnlockButtonClicked);
    connect(keystoreUpdateButton, &QPushButton::clicked, this, &WalletWidget::onKeystoreUpdateButtonClicked);
    connect(keystoreSignHashButton, &QPushButton::clicked, this, &WalletWidget::onKeystoreSignHashButtonClicked);
    connect(keystoreSignHashWithPassphraseButton, &QPushButton::clicked, this, &WalletWidget::onKeystoreSignHashWithPassphraseButtonClicked);
    connect(keystoreImportECDSAButton, &QPushButton::clicked, this, &WalletWidget::onKeystoreImportECDSAButtonClicked);
    connect(keystoreSignTxButton, &QPushButton::clicked, this, &WalletWidget::onKeystoreSignTxButtonClicked);
    connect(keystoreSignTxWithPassphraseButton, &QPushButton::clicked, this, &WalletWidget::onKeystoreSignTxWithPassphraseButtonClicked);
    connect(keystoreFindButton, &QPushButton::clicked, this, &WalletWidget::onKeystoreFindButtonClicked);
    
    // Connect extended keystore signals (in interface order)
    connect(extKeystoreInitButton, &QPushButton::clicked, this, &WalletWidget::onExtKeystoreInitButtonClicked);
    connect(extKeystoreCloseButton, &QPushButton::clicked, this, &WalletWidget::onExtKeystoreCloseButtonClicked);
    connect(extKeystoreListAccountsButton, &QPushButton::clicked, this, &WalletWidget::onExtKeystoreListAccountsButtonClicked);
    connect(extKeystoreNewAccountButton, &QPushButton::clicked, this, &WalletWidget::onExtKeystoreNewAccountButtonClicked);
    connect(extKeystoreImportButton, &QPushButton::clicked, this, &WalletWidget::onExtKeystoreImportButtonClicked);
    connect(extKeystoreImportExtendedKeyButton, &QPushButton::clicked, this, &WalletWidget::onExtKeystoreImportExtendedKeyButtonClicked);
    connect(extKeystoreExportExtButton, &QPushButton::clicked, this, &WalletWidget::onExtKeystoreExportExtButtonClicked);
    connect(extKeystoreExportPrivButton, &QPushButton::clicked, this, &WalletWidget::onExtKeystoreExportPrivButtonClicked);
    connect(extKeystoreDeleteButton, &QPushButton::clicked, this, &WalletWidget::onExtKeystoreDeleteButtonClicked);
    connect(extKeystoreHasAddressButton, &QPushButton::clicked, this, &WalletWidget::onExtKeystoreHasAddressButtonClicked);
    connect(extKeystoreUnlockButton, &QPushButton::clicked, this, &WalletWidget::onExtKeystoreUnlockButtonClicked);
    connect(extKeystoreLockButton, &QPushButton::clicked, this, &WalletWidget::onExtKeystoreLockButtonClicked);
    connect(extKeystoreTimedUnlockButton, &QPushButton::clicked, this, &WalletWidget::onExtKeystoreTimedUnlockButtonClicked);
    connect(extKeystoreUpdateButton, &QPushButton::clicked, this, &WalletWidget::onExtKeystoreUpdateButtonClicked);
    connect(extKeystoreSignHashButton, &QPushButton::clicked, this, &WalletWidget::onExtKeystoreSignHashButtonClicked);
    connect(extKeystoreSignHashWithPassphraseButton, &QPushButton::clicked, this, &WalletWidget::onExtKeystoreSignHashWithPassphraseButtonClicked);
    connect(extKeystoreSignTxButton, &QPushButton::clicked, this, &WalletWidget::onExtKeystoreSignTxButtonClicked);
    connect(extKeystoreSignTxWithPassphraseButton, &QPushButton::clicked, this, &WalletWidget::onExtKeystoreSignTxWithPassphraseButtonClicked);
    connect(extKeystoreDeriveButton, &QPushButton::clicked, this, &WalletWidget::onExtKeystoreDeriveButtonClicked);
    connect(extKeystoreDeriveWithPassphraseButton, &QPushButton::clicked, this, &WalletWidget::onExtKeystoreDeriveWithPassphraseButtonClicked);
    connect(extKeystoreFindButton, &QPushButton::clicked, this, &WalletWidget::onExtKeystoreFindButtonClicked);
    
    // Connect mnemonic signals (in interface order)
    connect(createMnemonicButton, &QPushButton::clicked, this, &WalletWidget::onCreateMnemonicButtonClicked);
    connect(lengthToEntropyStrengthButton, &QPushButton::clicked, this, &WalletWidget::onLengthToEntropyStrengthButtonClicked);
    
    // Connect key operations signals (in interface order)
    connect(createExtKeyFromMnemonicButton, &QPushButton::clicked, this, &WalletWidget::onCreateExtKeyFromMnemonicButtonClicked);
    connect(deriveExtKeyButton, &QPushButton::clicked, this, &WalletWidget::onDeriveExtKeyButtonClicked);
    connect(extKeyToECDSAButton, &QPushButton::clicked, this, &WalletWidget::onExtKeyToECDSAButtonClicked);
    connect(ecdsaToPublicKeyButton, &QPushButton::clicked, this, &WalletWidget::onEcdsaToPublicKeyButtonClicked);
    connect(publicKeyToAddressButton, &QPushButton::clicked, this, &WalletWidget::onPublicKeyToAddressButtonClicked);

    // Disable UI components until Wallet is initialized
    chainIDButton->setEnabled(false);
    ethBalanceButton->setEnabled(false);

    // Auto-initialize Wallet
    initWallet();
}

WalletWidget::~WalletWidget() {
    // Reset the active widget if it's this instance
    if (activeWidget == this) {
        activeWidget = nullptr;
    }

    stopWallet();
}

void WalletWidget::initWallet()
{
    updateStatus("Status: Initializing Wallet...");

    bool success = logos->wallet_module.initWallet("");

    if (!success) {
        updateStatus("Error: Failed to initialize Wallet");
        return;
    }

    isWalletInitialized = true;
    updateStatus("Status: Wallet initialized");

    // Enable UI components
    chainIDButton->setEnabled(true);
    ethBalanceButton->setEnabled(true);
}

void WalletWidget::stopWallet() {
    // Nothing to do here as the plugin handles the cleanup internally
    updateStatus("Status: Stopping Wallet...");
    isWalletInitialized = false;
    updateStatus("Status: Wallet stopped");
    
    // Disable UI components
    chainIDButton->setEnabled(false);
    ethBalanceButton->setEnabled(false);
}

void WalletWidget::onRpcCallButtonClicked() {
    rpcCallResultLabel->setText("RPC Call Result: Calling...");
    QString method = rpcCallMethodInput->text();
    QString paramsString = rpcCallParamsInput->text();
    QString result = logos->wallet_module.rpcCall("", method, paramsString);
    rpcCallResultLabel->setText("RPC Call Result: " + result);
}

void WalletWidget::onChainIDButtonClicked() {
    chainIDLabel->setText("Getting Chain ID...");
    QString chainID = logos->wallet_module.chainId("");
    chainIDLabel->setText("Chain ID: " + chainID);
}

void WalletWidget::onEthBalanceButtonClicked() {
    ethBalanceAddressLabel->setText("ETH Balance Address: " + ethBalanceAddressInput->text());
    ethBalanceValueLabel->setText("Getting ETH Balance...");
    QString ethBalance = logos->wallet_module.getEthBalance("", ethBalanceAddressInput->text());
    ethBalanceValueLabel->setText("ETH Balance: " + ethBalance);
}

void WalletWidget::updateStatus(const QString& message) {
    statusLabel->setText(message);
    qDebug() << message;
}

QFrame* WalletWidget::createSeparator() {
    QFrame* separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    return separator;
}

// Keystore operations
void WalletWidget::onKeystoreInitButtonClicked() {
    QString dir = keystoreDirInput->text();
    int scryptN = keystoreScryptNInput->text().toInt();
    int scryptP = keystoreScryptPInput->text().toInt();
    
    if (dir.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter a keystore directory");
        return;
    }
    
    updateStatus("Initializing keystore...");
    // Note: Assuming accounts_module is accessed via logos->accounts_module similar to wallet_module
    // This may need adjustment based on actual SDK structure
    bool success = logos->accounts_module.initKeystore(dir, scryptN, scryptP);
    
    if (success) {
        updateStatus("Keystore initialized successfully");
        QMessageBox::information(this, "Success", "Keystore initialized successfully");
    } else {
        updateStatus("Failed to initialize keystore");
        QMessageBox::warning(this, "Error", "Failed to initialize keystore");
    }
}

void WalletWidget::onKeystoreListAccountsButtonClicked() {
    updateStatus("Listing accounts...");
    QStringList accounts = logos->accounts_module.keystoreAccounts();
    
    keystoreAccountsList->clear();
    for (const QString& account : accounts) {
        keystoreAccountsList->addItem(account);
    }
    
    updateStatus(QString("Found %1 accounts").arg(accounts.size()));
}

void WalletWidget::onKeystoreNewAccountButtonClicked() {
    QString passphrase = keystoreNewAccountPassphraseInput->text();
    
    if (passphrase.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter a passphrase");
        return;
    }
    
    updateStatus("Creating new account...");
    QString address = logos->accounts_module.keystoreNewAccount(passphrase);
    
    if (!address.isEmpty()) {
        updateStatus(QString("New account created: %1").arg(address));
        QMessageBox::information(this, "Success", QString("New account created:\n%1").arg(address));
        keystoreNewAccountPassphraseInput->clear();
        onKeystoreListAccountsButtonClicked(); // Refresh list
    } else {
        updateStatus("Failed to create new account");
        QMessageBox::warning(this, "Error", "Failed to create new account");
    }
}

void WalletWidget::onKeystoreImportButtonClicked() {
    QString keyJSON = keystoreImportKeyJSONInput->toPlainText();
    QString passphrase = keystoreImportPassphraseInput->text();
    QString newPassphrase = keystoreImportNewPassphraseInput->text();
    
    if (keyJSON.isEmpty() || passphrase.isEmpty() || newPassphrase.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields");
        return;
    }
    
    updateStatus("Importing account...");
    QString address = logos->accounts_module.keystoreImport(keyJSON, passphrase, newPassphrase);
    
    if (!address.isEmpty()) {
        updateStatus(QString("Account imported: %1").arg(address));
        QMessageBox::information(this, "Success", QString("Account imported:\n%1").arg(address));
        keystoreImportKeyJSONInput->clear();
        keystoreImportPassphraseInput->clear();
        keystoreImportNewPassphraseInput->clear();
        onKeystoreListAccountsButtonClicked(); // Refresh list
    } else {
        updateStatus("Failed to import account");
        QMessageBox::warning(this, "Error", "Failed to import account");
    }
}

void WalletWidget::onKeystoreExportButtonClicked() {
    QString address = keystoreExportAddressInput->text();
    QString passphrase = keystoreExportPassphraseInput->text();
    QString newPassphrase = keystoreExportNewPassphraseInput->text();
    
    if (address.isEmpty() || passphrase.isEmpty() || newPassphrase.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields");
        return;
    }
    
    updateStatus("Exporting account...");
    QString keyJSON = logos->accounts_module.keystoreExport(address, passphrase, newPassphrase);
    
    if (!keyJSON.isEmpty()) {
        keystoreExportResult->setPlainText(keyJSON);
        updateStatus(QString("Account exported: %1").arg(address));
    } else {
        keystoreExportResult->setPlainText("Export failed");
        updateStatus("Failed to export account");
        QMessageBox::warning(this, "Error", "Failed to export account");
    }
}

void WalletWidget::onKeystoreDeleteButtonClicked() {
    QString address = keystoreDeleteAddressInput->text();
    QString passphrase = keystoreDeletePassphraseInput->text();
    
    if (address.isEmpty() || passphrase.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields");
        return;
    }
    
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm Delete", 
        QString("Are you sure you want to delete account %1?").arg(address),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        updateStatus("Deleting account...");
        bool success = logos->accounts_module.keystoreDelete(address, passphrase);
        
        if (success) {
            updateStatus(QString("Account deleted: %1").arg(address));
            QMessageBox::information(this, "Success", "Account deleted successfully");
            keystoreDeleteAddressInput->clear();
            keystoreDeletePassphraseInput->clear();
            onKeystoreListAccountsButtonClicked(); // Refresh list
        } else {
            updateStatus("Failed to delete account");
            QMessageBox::warning(this, "Error", "Failed to delete account");
        }
    }
}

void WalletWidget::onKeystoreUnlockButtonClicked() {
    QString address = keystoreUnlockAddressInput->text();
    QString passphrase = keystoreUnlockPassphraseInput->text();
    
    if (address.isEmpty() || passphrase.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields");
        return;
    }
    
    updateStatus("Unlocking account...");
    bool success = logos->accounts_module.keystoreUnlock(address, passphrase);
    
    if (success) {
        updateStatus(QString("Account unlocked: %1").arg(address));
        QMessageBox::information(this, "Success", "Account unlocked successfully");
        keystoreUnlockPassphraseInput->clear();
    } else {
        updateStatus("Failed to unlock account");
        QMessageBox::warning(this, "Error", "Failed to unlock account");
    }
}

void WalletWidget::onKeystoreLockButtonClicked() {
    QString address = keystoreLockAddressInput->text();
    
    if (address.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter an address");
        return;
    }
    
    updateStatus("Locking account...");
    bool success = logos->accounts_module.keystoreLock(address);
    
    if (success) {
        updateStatus(QString("Account locked: %1").arg(address));
        QMessageBox::information(this, "Success", "Account locked successfully");
        keystoreLockAddressInput->clear();
    } else {
        updateStatus("Failed to lock account");
        QMessageBox::warning(this, "Error", "Failed to lock account");
    }
}

void WalletWidget::onKeystoreSignHashButtonClicked() {
    QString address = keystoreSignHashAddressInput->text();
    QString hashHex = keystoreSignHashHashInput->text();
    
    if (address.isEmpty() || hashHex.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields");
        return;
    }
    
    updateStatus("Signing hash...");
    QString signature = logos->accounts_module.keystoreSignHash(address, hashHex);
    
    if (!signature.isEmpty()) {
        keystoreSignHashResult->setText(signature);
        updateStatus("Hash signed successfully");
    } else {
        keystoreSignHashResult->setText("Signing failed");
        updateStatus("Failed to sign hash");
        QMessageBox::warning(this, "Error", "Failed to sign hash");
    }
}

// Extended keystore operations
void WalletWidget::onExtKeystoreInitButtonClicked() {
    QString dir = extKeystoreDirInput->text();
    int scryptN = extKeystoreScryptNInput->text().toInt();
    int scryptP = extKeystoreScryptPInput->text().toInt();
    
    if (dir.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter a keystore directory");
        return;
    }
    
    updateStatus("Initializing extended keystore...");
    bool success = logos->accounts_module.initExtKeystore(dir, scryptN, scryptP);
    
    if (success) {
        updateStatus("Extended keystore initialized successfully");
        QMessageBox::information(this, "Success", "Extended keystore initialized successfully");
    } else {
        updateStatus("Failed to initialize extended keystore");
        QMessageBox::warning(this, "Error", "Failed to initialize extended keystore");
    }
}

void WalletWidget::onExtKeystoreListAccountsButtonClicked() {
    updateStatus("Listing extended keystore accounts...");
    QStringList accounts = logos->accounts_module.extKeystoreAccounts();
    
    extKeystoreAccountsList->clear();
    for (const QString& account : accounts) {
        extKeystoreAccountsList->addItem(account);
    }
    
    updateStatus(QString("Found %1 extended keystore accounts").arg(accounts.size()));
}

void WalletWidget::onExtKeystoreNewAccountButtonClicked() {
    QString passphrase = extKeystoreNewAccountPassphraseInput->text();
    
    if (passphrase.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter a passphrase");
        return;
    }
    
    updateStatus("Creating new extended keystore account...");
    QString address = logos->accounts_module.extKeystoreNewAccount(passphrase);
    
    if (!address.isEmpty()) {
        updateStatus(QString("New extended keystore account created: %1").arg(address));
        QMessageBox::information(this, "Success", QString("New extended keystore account created:\n%1").arg(address));
        extKeystoreNewAccountPassphraseInput->clear();
        onExtKeystoreListAccountsButtonClicked(); // Refresh list
    } else {
        updateStatus("Failed to create new extended keystore account");
        QMessageBox::warning(this, "Error", "Failed to create new extended keystore account");
    }
}

void WalletWidget::onExtKeystoreImportButtonClicked() {
    QString keyJSON = extKeystoreImportKeyJSONInput->toPlainText();
    QString passphrase = extKeystoreImportPassphraseInput->text();
    QString newPassphrase = extKeystoreImportNewPassphraseInput->text();
    
    if (keyJSON.isEmpty() || passphrase.isEmpty() || newPassphrase.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields");
        return;
    }
    
    updateStatus("Importing extended keystore account...");
    QString address = logos->accounts_module.extKeystoreImport(keyJSON, passphrase, newPassphrase);
    
    if (!address.isEmpty()) {
        updateStatus(QString("Extended keystore account imported: %1").arg(address));
        QMessageBox::information(this, "Success", QString("Extended keystore account imported:\n%1").arg(address));
        extKeystoreImportKeyJSONInput->clear();
        extKeystoreImportPassphraseInput->clear();
        extKeystoreImportNewPassphraseInput->clear();
        onExtKeystoreListAccountsButtonClicked(); // Refresh list
    } else {
        updateStatus("Failed to import extended keystore account");
        QMessageBox::warning(this, "Error", "Failed to import extended keystore account");
    }
}

void WalletWidget::onExtKeystoreDeriveButtonClicked() {
    QString address = extKeystoreDeriveAddressInput->text();
    QString derivationPath = extKeystoreDerivePathInput->text();
    int pin = extKeystoreDerivePinInput->text().toInt();
    
    if (address.isEmpty() || derivationPath.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in address and derivation path");
        return;
    }
    
    updateStatus("Deriving address...");
    QString derivedAddress = logos->accounts_module.extKeystoreDerive(address, derivationPath, pin);
    
    if (!derivedAddress.isEmpty()) {
        extKeystoreDeriveResult->setText(derivedAddress);
        updateStatus("Address derived successfully");
    } else {
        extKeystoreDeriveResult->setText("Derivation failed");
        updateStatus("Failed to derive address");
        QMessageBox::warning(this, "Error", "Failed to derive address");
    }
}

// Mnemonic operations
void WalletWidget::onCreateMnemonicButtonClicked() {
    QString lengthStr = mnemonicLengthInput->text();
    QJsonValue result;
    
    updateStatus("Creating random mnemonic...");
    QString mnemonic;
    
    if (lengthStr.isEmpty()) {
        mnemonic = logos->accounts_module.createRandomMnemonicWithDefaultLength();
    } else {
        int length = lengthStr.toInt();
        mnemonic = logos->accounts_module.createRandomMnemonic(length);
    }

    qDebug() << "Mnemonic: " << mnemonic;
    
    if (!mnemonic.isEmpty()) {
        mnemonicResult->setPlainText(mnemonic);
        updateStatus("Mnemonic created successfully");
    } else {
        mnemonicResult->setPlainText("Failed to create mnemonic");
        updateStatus("Failed to create mnemonic");
        QMessageBox::warning(this, "Error", "Failed to create mnemonic");
    }
}

// Key operations
void WalletWidget::onCreateExtKeyFromMnemonicButtonClicked() {
    QString phrase = createExtKeyMnemonicInput->toPlainText();
    QString passphrase = createExtKeyPassphraseInput->text();
    
    if (phrase.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter a mnemonic phrase");
        return;
    }
    
    updateStatus("Creating extended key from mnemonic...");
    QString extKey = logos->accounts_module.createExtKeyFromMnemonic(phrase, passphrase);
    
    if (!extKey.isEmpty()) {
        createExtKeyResult->setPlainText(extKey);
        updateStatus("Extended key created successfully");
    } else {
        createExtKeyResult->setPlainText("Failed to create extended key");
        updateStatus("Failed to create extended key");
        QMessageBox::warning(this, "Error", "Failed to create extended key");
    }
}

void WalletWidget::onDeriveExtKeyButtonClicked() {
    QString extKeyStr = deriveExtKeyInput->text();
    QString pathStr = deriveExtKeyPathInput->text();
    
    if (extKeyStr.isEmpty() || pathStr.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields");
        return;
    }
    
    updateStatus("Deriving extended key...");
    QString derivedKey = logos->accounts_module.deriveExtKey(extKeyStr, pathStr);
    
    if (!derivedKey.isEmpty()) {
        deriveExtKeyResult->setPlainText(derivedKey);
        updateStatus("Extended key derived successfully");
    } else {
        deriveExtKeyResult->setPlainText("Derivation failed");
        updateStatus("Failed to derive extended key");
        QMessageBox::warning(this, "Error", "Failed to derive extended key");
    }
}

// Missing keystore handlers
void WalletWidget::onKeystoreCloseButtonClicked() {
    updateStatus("Closing keystore...");
    bool success = logos->accounts_module.closeKeystore("");
    
    if (success) {
        updateStatus("Keystore closed successfully");
        QMessageBox::information(this, "Success", "Keystore closed successfully");
    } else {
        updateStatus("Failed to close keystore");
        QMessageBox::warning(this, "Error", "Failed to close keystore");
    }
}

void WalletWidget::onKeystoreHasAddressButtonClicked() {
    QString address = keystoreHasAddressInput->text();
    
    if (address.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter an address");
        return;
    }
    
    updateStatus("Checking if address exists...");
    bool hasAddress = logos->accounts_module.keystoreHasAddress(address);
    
    keystoreHasAddressResult->setText(hasAddress ? "true" : "false");
    updateStatus(hasAddress ? "Address exists" : "Address does not exist");
}

void WalletWidget::onKeystoreTimedUnlockButtonClicked() {
    QString address = keystoreTimedUnlockAddressInput->text();
    QString passphrase = keystoreTimedUnlockPassphraseInput->text();
    QString timeoutStr = keystoreTimedUnlockTimeoutInput->text();
    
    if (address.isEmpty() || passphrase.isEmpty() || timeoutStr.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields");
        return;
    }
    
    uint timeout = timeoutStr.toUInt();
    updateStatus("Unlocking account with timeout...");
    bool success = logos->accounts_module.keystoreTimedUnlock(address, passphrase, timeout);
    
    if (success) {
        updateStatus(QString("Account unlocked with timeout: %1").arg(address));
        QMessageBox::information(this, "Success", "Account unlocked successfully");
        keystoreTimedUnlockPassphraseInput->clear();
    } else {
        updateStatus("Failed to unlock account");
        QMessageBox::warning(this, "Error", "Failed to unlock account");
    }
}

void WalletWidget::onKeystoreUpdateButtonClicked() {
    QString address = keystoreUpdateAddressInput->text();
    QString passphrase = keystoreUpdatePassphraseInput->text();
    QString newPassphrase = keystoreUpdateNewPassphraseInput->text();
    
    if (address.isEmpty() || passphrase.isEmpty() || newPassphrase.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields");
        return;
    }
    
    updateStatus("Updating passphrase...");
    bool success = logos->accounts_module.keystoreUpdate(address, passphrase, newPassphrase);
    
    if (success) {
        updateStatus(QString("Passphrase updated: %1").arg(address));
        QMessageBox::information(this, "Success", "Passphrase updated successfully");
        keystoreUpdatePassphraseInput->clear();
        keystoreUpdateNewPassphraseInput->clear();
    } else {
        updateStatus("Failed to update passphrase");
        QMessageBox::warning(this, "Error", "Failed to update passphrase");
    }
}

void WalletWidget::onKeystoreSignHashWithPassphraseButtonClicked() {
    QString address = keystoreSignHashWithPassphraseAddressInput->text();
    QString passphrase = keystoreSignHashWithPassphrasePassphraseInput->text();
    QString hashHex = keystoreSignHashWithPassphraseHashInput->text();
    
    if (address.isEmpty() || passphrase.isEmpty() || hashHex.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields");
        return;
    }
    
    updateStatus("Signing hash with passphrase...");
    QString signature = logos->accounts_module.keystoreSignHashWithPassphrase(address, passphrase, hashHex);
    
    if (!signature.isEmpty()) {
        keystoreSignHashWithPassphraseResult->setText(signature);
        updateStatus("Hash signed successfully");
    } else {
        keystoreSignHashWithPassphraseResult->setText("Signing failed");
        updateStatus("Failed to sign hash");
        QMessageBox::warning(this, "Error", "Failed to sign hash");
    }
}

void WalletWidget::onKeystoreImportECDSAButtonClicked() {
    QString privateKeyHex = keystoreImportECDSAPrivateKeyInput->text();
    QString passphrase = keystoreImportECDSAPassphraseInput->text();
    
    if (privateKeyHex.isEmpty() || passphrase.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields");
        return;
    }
    
    updateStatus("Importing ECDSA private key...");
    QString address = logos->accounts_module.keystoreImportECDSA(privateKeyHex, passphrase);
    
    if (!address.isEmpty()) {
        keystoreImportECDSAResult->setText(address);
        updateStatus(QString("ECDSA key imported: %1").arg(address));
        QMessageBox::information(this, "Success", QString("ECDSA key imported:\n%1").arg(address));
        keystoreImportECDSAPrivateKeyInput->clear();
        keystoreImportECDSAPassphraseInput->clear();
        onKeystoreListAccountsButtonClicked();
    } else {
        keystoreImportECDSAResult->setText("Import failed");
        updateStatus("Failed to import ECDSA key");
        QMessageBox::warning(this, "Error", "Failed to import ECDSA key");
    }
}

void WalletWidget::onKeystoreSignTxButtonClicked() {
    QString address = keystoreSignTxAddressInput->text();
    QString txJSON = keystoreSignTxTxJSONInput->toPlainText();
    QString chainIDHex = keystoreSignTxChainIDInput->text();
    
    if (address.isEmpty() || txJSON.isEmpty() || chainIDHex.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields");
        return;
    }
    
    updateStatus("Signing transaction...");
    QString signedTx = logos->accounts_module.keystoreSignTx(address, txJSON, chainIDHex);
    
    if (!signedTx.isEmpty()) {
        keystoreSignTxResult->setPlainText(signedTx);
        updateStatus("Transaction signed successfully");
    } else {
        keystoreSignTxResult->setPlainText("Signing failed");
        updateStatus("Failed to sign transaction");
        QMessageBox::warning(this, "Error", "Failed to sign transaction");
    }
}

void WalletWidget::onKeystoreSignTxWithPassphraseButtonClicked() {
    QString address = keystoreSignTxWithPassphraseAddressInput->text();
    QString passphrase = keystoreSignTxWithPassphrasePassphraseInput->text();
    QString txJSON = keystoreSignTxWithPassphraseTxJSONInput->toPlainText();
    QString chainIDHex = keystoreSignTxWithPassphraseChainIDInput->text();
    
    if (address.isEmpty() || passphrase.isEmpty() || txJSON.isEmpty() || chainIDHex.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields");
        return;
    }
    
    updateStatus("Signing transaction with passphrase...");
    QString signedTx = logos->accounts_module.keystoreSignTxWithPassphrase(address, passphrase, txJSON, chainIDHex);
    
    if (!signedTx.isEmpty()) {
        keystoreSignTxWithPassphraseResult->setPlainText(signedTx);
        updateStatus("Transaction signed successfully");
    } else {
        keystoreSignTxWithPassphraseResult->setPlainText("Signing failed");
        updateStatus("Failed to sign transaction");
        QMessageBox::warning(this, "Error", "Failed to sign transaction");
    }
}

void WalletWidget::onKeystoreFindButtonClicked() {
    QString address = keystoreFindAddressInput->text();
    QString url = keystoreFindUrlInput->text();
    
    if (address.isEmpty() || url.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields");
        return;
    }
    
    updateStatus("Finding account...");
    QString result = logos->accounts_module.keystoreFind(address, url);
    
    if (!result.isEmpty()) {
        keystoreFindResult->setText(result);
        updateStatus("Account found");
    } else {
        keystoreFindResult->setText("Not found");
        updateStatus("Account not found");
    }
}

// Missing extended keystore handlers
void WalletWidget::onExtKeystoreCloseButtonClicked() {
    updateStatus("Closing extended keystore...");
    bool success = logos->accounts_module.closeExtKeystore();
    
    if (success) {
        updateStatus("Extended keystore closed successfully");
        QMessageBox::information(this, "Success", "Extended keystore closed successfully");
    } else {
        updateStatus("Failed to close extended keystore");
        QMessageBox::warning(this, "Error", "Failed to close extended keystore");
    }
}

void WalletWidget::onExtKeystoreImportExtendedKeyButtonClicked() {
    QString extKeyStr = extKeystoreImportExtendedKeyInput->text();
    QString passphrase = extKeystoreImportExtendedKeyPassphraseInput->text();
    
    if (extKeyStr.isEmpty() || passphrase.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields");
        return;
    }
    
    updateStatus("Importing extended key...");
    QString address = logos->accounts_module.extKeystoreImportExtendedKey(extKeyStr, passphrase);
    
    if (!address.isEmpty()) {
        extKeystoreImportExtendedKeyResult->setText(address);
        updateStatus(QString("Extended key imported: %1").arg(address));
        QMessageBox::information(this, "Success", QString("Extended key imported:\n%1").arg(address));
        extKeystoreImportExtendedKeyInput->clear();
        extKeystoreImportExtendedKeyPassphraseInput->clear();
        onExtKeystoreListAccountsButtonClicked();
    } else {
        extKeystoreImportExtendedKeyResult->setText("Import failed");
        updateStatus("Failed to import extended key");
        QMessageBox::warning(this, "Error", "Failed to import extended key");
    }
}

void WalletWidget::onExtKeystoreExportExtButtonClicked() {
    QString address = extKeystoreExportExtAddressInput->text();
    QString passphrase = extKeystoreExportExtPassphraseInput->text();
    QString newPassphrase = extKeystoreExportExtNewPassphraseInput->text();
    
    if (address.isEmpty() || passphrase.isEmpty() || newPassphrase.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields");
        return;
    }
    
    updateStatus("Exporting extended key...");
    QString keyJSON = logos->accounts_module.extKeystoreExportExt(address, passphrase, newPassphrase);
    
    if (!keyJSON.isEmpty()) {
        extKeystoreExportExtResult->setPlainText(keyJSON);
        updateStatus(QString("Extended key exported: %1").arg(address));
    } else {
        extKeystoreExportExtResult->setPlainText("Export failed");
        updateStatus("Failed to export extended key");
        QMessageBox::warning(this, "Error", "Failed to export extended key");
    }
}

void WalletWidget::onExtKeystoreExportPrivButtonClicked() {
    QString address = extKeystoreExportPrivAddressInput->text();
    QString passphrase = extKeystoreExportPrivPassphraseInput->text();
    QString newPassphrase = extKeystoreExportPrivNewPassphraseInput->text();
    
    if (address.isEmpty() || passphrase.isEmpty() || newPassphrase.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields");
        return;
    }
    
    updateStatus("Exporting private key...");
    QString keyJSON = logos->accounts_module.extKeystoreExportPriv(address, passphrase, newPassphrase);
    
    if (!keyJSON.isEmpty()) {
        extKeystoreExportPrivResult->setPlainText(keyJSON);
        updateStatus(QString("Private key exported: %1").arg(address));
    } else {
        extKeystoreExportPrivResult->setPlainText("Export failed");
        updateStatus("Failed to export private key");
        QMessageBox::warning(this, "Error", "Failed to export private key");
    }
}

void WalletWidget::onExtKeystoreDeleteButtonClicked() {
    QString address = extKeystoreDeleteAddressInput->text();
    QString passphrase = extKeystoreDeletePassphraseInput->text();
    
    if (address.isEmpty() || passphrase.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields");
        return;
    }
    
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm Delete", 
        QString("Are you sure you want to delete account %1?").arg(address),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        updateStatus("Deleting account...");
        bool success = logos->accounts_module.extKeystoreDelete(address, passphrase);
        
        if (success) {
            updateStatus(QString("Account deleted: %1").arg(address));
            QMessageBox::information(this, "Success", "Account deleted successfully");
            extKeystoreDeleteAddressInput->clear();
            extKeystoreDeletePassphraseInput->clear();
            onExtKeystoreListAccountsButtonClicked();
        } else {
            updateStatus("Failed to delete account");
            QMessageBox::warning(this, "Error", "Failed to delete account");
        }
    }
}

void WalletWidget::onExtKeystoreHasAddressButtonClicked() {
    QString address = extKeystoreHasAddressInput->text();
    
    if (address.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter an address");
        return;
    }
    
    updateStatus("Checking if address exists...");
    bool hasAddress = logos->accounts_module.extKeystoreHasAddress(address);
    
    extKeystoreHasAddressResult->setText(hasAddress ? "true" : "false");
    updateStatus(hasAddress ? "Address exists" : "Address does not exist");
}

void WalletWidget::onExtKeystoreUnlockButtonClicked() {
    QString address = extKeystoreUnlockAddressInput->text();
    QString passphrase = extKeystoreUnlockPassphraseInput->text();
    
    if (address.isEmpty() || passphrase.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields");
        return;
    }
    
    updateStatus("Unlocking account...");
    bool success = logos->accounts_module.extKeystoreUnlock(address, passphrase);
    
    if (success) {
        updateStatus(QString("Account unlocked: %1").arg(address));
        QMessageBox::information(this, "Success", "Account unlocked successfully");
        extKeystoreUnlockPassphraseInput->clear();
    } else {
        updateStatus("Failed to unlock account");
        QMessageBox::warning(this, "Error", "Failed to unlock account");
    }
}

void WalletWidget::onExtKeystoreLockButtonClicked() {
    QString address = extKeystoreLockAddressInput->text();
    
    if (address.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter an address");
        return;
    }
    
    updateStatus("Locking account...");
    bool success = logos->accounts_module.extKeystoreLock(address);
    
    if (success) {
        updateStatus(QString("Account locked: %1").arg(address));
        QMessageBox::information(this, "Success", "Account locked successfully");
        extKeystoreLockAddressInput->clear();
    } else {
        updateStatus("Failed to lock account");
        QMessageBox::warning(this, "Error", "Failed to lock account");
    }
}

void WalletWidget::onExtKeystoreTimedUnlockButtonClicked() {
    QString address = extKeystoreTimedUnlockAddressInput->text();
    QString passphrase = extKeystoreTimedUnlockPassphraseInput->text();
    QString timeoutStr = extKeystoreTimedUnlockTimeoutInput->text();
    
    if (address.isEmpty() || passphrase.isEmpty() || timeoutStr.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields");
        return;
    }
    
    uint timeout = timeoutStr.toUInt();
    updateStatus("Unlocking account with timeout...");
    bool success = logos->accounts_module.extKeystoreTimedUnlock(address, passphrase, timeout);
    
    if (success) {
        updateStatus(QString("Account unlocked with timeout: %1").arg(address));
        QMessageBox::information(this, "Success", "Account unlocked successfully");
        extKeystoreTimedUnlockPassphraseInput->clear();
    } else {
        updateStatus("Failed to unlock account");
        QMessageBox::warning(this, "Error", "Failed to unlock account");
    }
}

void WalletWidget::onExtKeystoreUpdateButtonClicked() {
    QString address = extKeystoreUpdateAddressInput->text();
    QString passphrase = extKeystoreUpdatePassphraseInput->text();
    QString newPassphrase = extKeystoreUpdateNewPassphraseInput->text();
    
    if (address.isEmpty() || passphrase.isEmpty() || newPassphrase.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields");
        return;
    }
    
    updateStatus("Updating passphrase...");
    bool success = logos->accounts_module.extKeystoreUpdate(address, passphrase, newPassphrase);
    
    if (success) {
        updateStatus(QString("Passphrase updated: %1").arg(address));
        QMessageBox::information(this, "Success", "Passphrase updated successfully");
        extKeystoreUpdatePassphraseInput->clear();
        extKeystoreUpdateNewPassphraseInput->clear();
    } else {
        updateStatus("Failed to update passphrase");
        QMessageBox::warning(this, "Error", "Failed to update passphrase");
    }
}

void WalletWidget::onExtKeystoreSignHashButtonClicked() {
    QString address = extKeystoreSignHashAddressInput->text();
    QString hashHex = extKeystoreSignHashHashInput->text();
    
    if (address.isEmpty() || hashHex.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields");
        return;
    }
    
    updateStatus("Signing hash...");
    QString signature = logos->accounts_module.extKeystoreSignHash(address, hashHex);
    
    if (!signature.isEmpty()) {
        extKeystoreSignHashResult->setText(signature);
        updateStatus("Hash signed successfully");
    } else {
        extKeystoreSignHashResult->setText("Signing failed");
        updateStatus("Failed to sign hash");
        QMessageBox::warning(this, "Error", "Failed to sign hash");
    }
}

void WalletWidget::onExtKeystoreSignHashWithPassphraseButtonClicked() {
    QString address = extKeystoreSignHashWithPassphraseAddressInput->text();
    QString passphrase = extKeystoreSignHashWithPassphrasePassphraseInput->text();
    QString hashHex = extKeystoreSignHashWithPassphraseHashInput->text();
    
    if (address.isEmpty() || passphrase.isEmpty() || hashHex.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields");
        return;
    }
    
    updateStatus("Signing hash with passphrase...");
    QString signature = logos->accounts_module.extKeystoreSignHashWithPassphrase(address, passphrase, hashHex);
    
    if (!signature.isEmpty()) {
        extKeystoreSignHashWithPassphraseResult->setText(signature);
        updateStatus("Hash signed successfully");
    } else {
        extKeystoreSignHashWithPassphraseResult->setText("Signing failed");
        updateStatus("Failed to sign hash");
        QMessageBox::warning(this, "Error", "Failed to sign hash");
    }
}

void WalletWidget::onExtKeystoreSignTxButtonClicked() {
    QString address = extKeystoreSignTxAddressInput->text();
    QString txJSON = extKeystoreSignTxTxJSONInput->toPlainText();
    QString chainIDHex = extKeystoreSignTxChainIDInput->text();
    
    if (address.isEmpty() || txJSON.isEmpty() || chainIDHex.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields");
        return;
    }
    
    updateStatus("Signing transaction...");
    QString signedTx = logos->accounts_module.extKeystoreSignTx(address, txJSON, chainIDHex);
    
    if (!signedTx.isEmpty()) {
        extKeystoreSignTxResult->setPlainText(signedTx);
        updateStatus("Transaction signed successfully");
    } else {
        extKeystoreSignTxResult->setPlainText("Signing failed");
        updateStatus("Failed to sign transaction");
        QMessageBox::warning(this, "Error", "Failed to sign transaction");
    }
}

void WalletWidget::onExtKeystoreSignTxWithPassphraseButtonClicked() {
    QString address = extKeystoreSignTxWithPassphraseAddressInput->text();
    QString passphrase = extKeystoreSignTxWithPassphrasePassphraseInput->text();
    QString txJSON = extKeystoreSignTxWithPassphraseTxJSONInput->toPlainText();
    QString chainIDHex = extKeystoreSignTxWithPassphraseChainIDInput->text();
    
    if (address.isEmpty() || passphrase.isEmpty() || txJSON.isEmpty() || chainIDHex.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields");
        return;
    }
    
    updateStatus("Signing transaction with passphrase...");
    QString signedTx = logos->accounts_module.extKeystoreSignTxWithPassphrase(address, passphrase, txJSON, chainIDHex);
    
    if (!signedTx.isEmpty()) {
        extKeystoreSignTxWithPassphraseResult->setPlainText(signedTx);
        updateStatus("Transaction signed successfully");
    } else {
        extKeystoreSignTxWithPassphraseResult->setPlainText("Signing failed");
        updateStatus("Failed to sign transaction");
        QMessageBox::warning(this, "Error", "Failed to sign transaction");
    }
}

void WalletWidget::onExtKeystoreDeriveWithPassphraseButtonClicked() {
    QString address = extKeystoreDeriveWithPassphraseAddressInput->text();
    QString derivationPath = extKeystoreDeriveWithPassphrasePathInput->text();
    QString pinStr = extKeystoreDeriveWithPassphrasePinInput->text();
    QString passphrase = extKeystoreDeriveWithPassphrasePassphraseInput->text();
    QString newPassphrase = extKeystoreDeriveWithPassphraseNewPassphraseInput->text();
    
    if (address.isEmpty() || derivationPath.isEmpty() || pinStr.isEmpty() || passphrase.isEmpty() || newPassphrase.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields");
        return;
    }
    
    int pin = pinStr.toInt();
    updateStatus("Deriving address with passphrase...");
    QString derivedAddress = logos->accounts_module.extKeystoreDeriveWithPassphrase(address, derivationPath, pin, passphrase, newPassphrase);
    
    if (!derivedAddress.isEmpty()) {
        extKeystoreDeriveWithPassphraseResult->setText(derivedAddress);
        updateStatus("Address derived successfully");
    } else {
        extKeystoreDeriveWithPassphraseResult->setText("Derivation failed");
        updateStatus("Failed to derive address");
        QMessageBox::warning(this, "Error", "Failed to derive address");
    }
}

void WalletWidget::onExtKeystoreFindButtonClicked() {
    QString address = extKeystoreFindAddressInput->text();
    QString url = extKeystoreFindUrlInput->text();
    
    if (address.isEmpty() || url.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields");
        return;
    }
    
    updateStatus("Finding account...");
    QString result = logos->accounts_module.extKeystoreFind(address, url);
    
    if (!result.isEmpty()) {
        extKeystoreFindResult->setText(result);
        updateStatus("Account found");
    } else {
        extKeystoreFindResult->setText("Not found");
        updateStatus("Account not found");
    }
}

// Missing key operations handlers
void WalletWidget::onExtKeyToECDSAButtonClicked() {
    QString extKeyStr = extKeyToECDSAInput->text();
    
    if (extKeyStr.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter an extended key");
        return;
    }
    
    updateStatus("Converting extended key to ECDSA...");
    QString ecdsaKey = logos->accounts_module.extKeyToECDSA(extKeyStr);
    
    if (!ecdsaKey.isEmpty()) {
        extKeyToECDSAResult->setText(ecdsaKey);
        updateStatus("Conversion successful");
    } else {
        extKeyToECDSAResult->setText("Conversion failed");
        updateStatus("Failed to convert extended key");
        QMessageBox::warning(this, "Error", "Failed to convert extended key");
    }
}

void WalletWidget::onEcdsaToPublicKeyButtonClicked() {
    QString privateKeyHex = ecdsaToPublicKeyInput->text();
    
    if (privateKeyHex.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter an ECDSA private key");
        return;
    }
    
    updateStatus("Converting ECDSA to public key...");
    QString publicKey = logos->accounts_module.ecdsaToPublicKey(privateKeyHex);
    
    if (!publicKey.isEmpty()) {
        ecdsaToPublicKeyResult->setText(publicKey);
        updateStatus("Conversion successful");
    } else {
        ecdsaToPublicKeyResult->setText("Conversion failed");
        updateStatus("Failed to convert ECDSA key");
        QMessageBox::warning(this, "Error", "Failed to convert ECDSA key");
    }
}

void WalletWidget::onPublicKeyToAddressButtonClicked() {
    QString publicKeyHex = publicKeyToAddressInput->text();
    
    if (publicKeyHex.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter a public key");
        return;
    }
    
    updateStatus("Converting public key to address...");
    QString address = logos->accounts_module.publicKeyToAddress(publicKeyHex);
    
    if (!address.isEmpty()) {
        publicKeyToAddressResult->setText(address);
        updateStatus("Conversion successful");
    } else {
        publicKeyToAddressResult->setText("Conversion failed");
        updateStatus("Failed to convert public key");
        QMessageBox::warning(this, "Error", "Failed to convert public key");
    }
}

// Missing mnemonic handler
void WalletWidget::onLengthToEntropyStrengthButtonClicked() {
    QString lengthStr = lengthToEntropyStrengthInput->text();
    
    if (lengthStr.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter a length");
        return;
    }
    
    int length = lengthStr.toInt();
    updateStatus("Calculating entropy strength...");
    int strength = logos->accounts_module.lengthToEntropyStrength(length);
    
    lengthToEntropyStrengthResult->setText(QString::number(strength));
    updateStatus("Entropy strength calculated");
}