#include "WalletWidget.h"
#include <QDebug>
#include <QDateTime>
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonValue>
#include <QDir>
#include <QVariant>
#include <iostream>
#include <csignal>
#include <QTimer>
#include "logos_api_client.h"

// Static pointer to the active WalletWidget for callbacks
static WalletWidget* activeWidget = nullptr;

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
    
    // Keystore List Accounts
    QHBoxLayout* keystoreListLayout = new QHBoxLayout();
    keystoreListAccountsButton = new QPushButton("List Accounts", accountsScrollContent);
    keystoreAccountsList = new QListWidget(accountsScrollContent);
    keystoreAccountsList->setMaximumHeight(100);
    keystoreListLayout->addWidget(keystoreListAccountsButton);
    keystoreListLayout->addWidget(keystoreAccountsList, 1);
    keystoreLayout->addLayout(keystoreListLayout);
    
    // Keystore New Account
    QHBoxLayout* keystoreNewAccountLayout = new QHBoxLayout();
    keystoreNewAccountPassphraseInput = new QLineEdit(accountsScrollContent);
    keystoreNewAccountPassphraseInput->setPlaceholderText("Passphrase...");
    keystoreNewAccountPassphraseInput->setEchoMode(QLineEdit::Password);
    keystoreNewAccountButton = new QPushButton("New Account", accountsScrollContent);
    keystoreNewAccountLayout->addWidget(keystoreNewAccountPassphraseInput);
    keystoreNewAccountLayout->addWidget(keystoreNewAccountButton);
    keystoreLayout->addLayout(keystoreNewAccountLayout);
    
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
    
    // Keystore Import ECDSA
    QLabel* keystoreImportECDSALabel = new QLabel("Import ECDSA Key:", accountsScrollContent);
    keystoreLayout->addWidget(keystoreImportECDSALabel);
    QHBoxLayout* keystoreImportECDSALayout = new QHBoxLayout();
    keystoreImportECDSAKeyInput = new QLineEdit(accountsScrollContent);
    keystoreImportECDSAKeyInput->setPlaceholderText("ECDSA Private Key (hex)...");
    keystoreImportECDSAPassphraseInput = new QLineEdit(accountsScrollContent);
    keystoreImportECDSAPassphraseInput->setPlaceholderText("New Passphrase...");
    keystoreImportECDSAPassphraseInput->setEchoMode(QLineEdit::Password);
    keystoreImportECDSAButton = new QPushButton("Import ECDSA", accountsScrollContent);
    keystoreImportECDSALayout->addWidget(keystoreImportECDSAKeyInput);
    keystoreImportECDSALayout->addWidget(keystoreImportECDSAPassphraseInput);
    keystoreImportECDSALayout->addWidget(keystoreImportECDSAButton);
    keystoreLayout->addLayout(keystoreImportECDSALayout);
    
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
    keystoreExportResult = new QTextEdit(accountsScrollContent);
    keystoreExportResult->setPlaceholderText("Export result will appear here...");
    keystoreExportResult->setMaximumHeight(80);
    keystoreExportResult->setReadOnly(true);
    keystoreLayout->addWidget(keystoreExportResult);
    
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
    keystoreSignHashResult = new QLabel("Signature result will appear here...", accountsScrollContent);
    keystoreLayout->addWidget(keystoreSignHashResult);
    
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
    
    // Extended Keystore List Accounts
    QHBoxLayout* extKeystoreListLayout = new QHBoxLayout();
    extKeystoreListAccountsButton = new QPushButton("List Accounts", accountsScrollContent);
    extKeystoreAccountsList = new QListWidget(accountsScrollContent);
    extKeystoreAccountsList->setMaximumHeight(100);
    extKeystoreListLayout->addWidget(extKeystoreListAccountsButton);
    extKeystoreListLayout->addWidget(extKeystoreAccountsList, 1);
    extKeystoreLayout->addLayout(extKeystoreListLayout);
    
    // Extended Keystore New Account
    QHBoxLayout* extKeystoreNewAccountLayout = new QHBoxLayout();
    extKeystoreNewAccountPassphraseInput = new QLineEdit(accountsScrollContent);
    extKeystoreNewAccountPassphraseInput->setPlaceholderText("Passphrase...");
    extKeystoreNewAccountPassphraseInput->setEchoMode(QLineEdit::Password);
    extKeystoreNewAccountButton = new QPushButton("New Account", accountsScrollContent);
    extKeystoreNewAccountLayout->addWidget(extKeystoreNewAccountPassphraseInput);
    extKeystoreNewAccountLayout->addWidget(extKeystoreNewAccountButton);
    extKeystoreLayout->addLayout(extKeystoreNewAccountLayout);
    
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
    
    // Extended Keystore Import Extended Key
    QLabel* extKeystoreImportExtendedKeyLabel = new QLabel("Import Extended Key:", accountsScrollContent);
    extKeystoreLayout->addWidget(extKeystoreImportExtendedKeyLabel);
    QHBoxLayout* extKeystoreImportExtendedKeyLayout = new QHBoxLayout();
    extKeystoreImportExtendedKeyInput = new QLineEdit(accountsScrollContent);
    extKeystoreImportExtendedKeyInput->setPlaceholderText("Extended Key...");
    extKeystoreImportExtendedKeyPassphraseInput = new QLineEdit(accountsScrollContent);
    extKeystoreImportExtendedKeyPassphraseInput->setPlaceholderText("New Passphrase...");
    extKeystoreImportExtendedKeyPassphraseInput->setEchoMode(QLineEdit::Password);
    extKeystoreImportExtendedKeyButton = new QPushButton("Import Extended Key", accountsScrollContent);
    extKeystoreImportExtendedKeyLayout->addWidget(extKeystoreImportExtendedKeyInput);
    extKeystoreImportExtendedKeyLayout->addWidget(extKeystoreImportExtendedKeyPassphraseInput);
    extKeystoreImportExtendedKeyLayout->addWidget(extKeystoreImportExtendedKeyButton);
    extKeystoreLayout->addLayout(extKeystoreImportExtendedKeyLayout);
    
    // Extended Keystore Export
    QHBoxLayout* extKeystoreExportLayout = new QHBoxLayout();
    extKeystoreExportAddressInput = new QLineEdit(accountsScrollContent);
    extKeystoreExportAddressInput->setPlaceholderText("Address...");
    extKeystoreExportPassphraseInput = new QLineEdit(accountsScrollContent);
    extKeystoreExportPassphraseInput->setPlaceholderText("Passphrase...");
    extKeystoreExportPassphraseInput->setEchoMode(QLineEdit::Password);
    extKeystoreExportNewPassphraseInput = new QLineEdit(accountsScrollContent);
    extKeystoreExportNewPassphraseInput->setPlaceholderText("New Passphrase...");
    extKeystoreExportNewPassphraseInput->setEchoMode(QLineEdit::Password);
    extKeystoreExportButton = new QPushButton("Export Extended Key", accountsScrollContent);
    extKeystoreExportLayout->addWidget(extKeystoreExportAddressInput);
    extKeystoreExportLayout->addWidget(extKeystoreExportPassphraseInput);
    extKeystoreExportLayout->addWidget(extKeystoreExportNewPassphraseInput);
    extKeystoreExportLayout->addWidget(extKeystoreExportButton);
    extKeystoreLayout->addLayout(extKeystoreExportLayout);
    extKeystoreExportResult = new QTextEdit(accountsScrollContent);
    extKeystoreExportResult->setPlaceholderText("Export result will appear here...");
    extKeystoreExportResult->setMaximumHeight(80);
    extKeystoreExportResult->setReadOnly(true);
    extKeystoreLayout->addWidget(extKeystoreExportResult);
    
    // Extended Keystore Export Private Key
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
    extKeystoreExportPrivResult = new QTextEdit(accountsScrollContent);
    extKeystoreExportPrivResult->setPlaceholderText("Private key export result will appear here...");
    extKeystoreExportPrivResult->setMaximumHeight(80);
    extKeystoreExportPrivResult->setReadOnly(true);
    extKeystoreLayout->addWidget(extKeystoreExportPrivResult);
    
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
    
    // Extended Keystore Unlock/Lock
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
    
    QHBoxLayout* extKeystoreLockLayout = new QHBoxLayout();
    extKeystoreLockAddressInput = new QLineEdit(accountsScrollContent);
    extKeystoreLockAddressInput->setPlaceholderText("Address...");
    extKeystoreLockButton = new QPushButton("Lock", accountsScrollContent);
    extKeystoreLockLayout->addWidget(extKeystoreLockAddressInput);
    extKeystoreLockLayout->addWidget(extKeystoreLockButton);
    extKeystoreLayout->addLayout(extKeystoreLockLayout);
    
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
    extKeystoreSignHashResult = new QLabel("Signature result will appear here...", accountsScrollContent);
    extKeystoreLayout->addWidget(extKeystoreSignHashResult);
    
    // Extended Keystore Derive
    QHBoxLayout* extKeystoreDeriveLayout = new QHBoxLayout();
    extKeystoreDeriveAddressInput = new QLineEdit(accountsScrollContent);
    extKeystoreDeriveAddressInput->setPlaceholderText("Address...");
    extKeystoreDerivePathInput = new QLineEdit(accountsScrollContent);
    extKeystoreDerivePathInput->setPlaceholderText("Derivation Path (e.g., m/44'/60'/0'/0/0)...");
    extKeystoreDerivePassphraseInput = new QLineEdit(accountsScrollContent);
    extKeystoreDerivePassphraseInput->setPlaceholderText("Passphrase...");
    extKeystoreDerivePassphraseInput->setEchoMode(QLineEdit::Password);
    extKeystoreDeriveNewPassphraseInput = new QLineEdit(accountsScrollContent);
    extKeystoreDeriveNewPassphraseInput->setPlaceholderText("New Passphrase...");
    extKeystoreDeriveNewPassphraseInput->setEchoMode(QLineEdit::Password);
    extKeystoreDeriveButton = new QPushButton("Derive", accountsScrollContent);
    extKeystoreDeriveLayout->addWidget(extKeystoreDeriveAddressInput);
    extKeystoreDeriveLayout->addWidget(extKeystoreDerivePathInput);
    extKeystoreDeriveLayout->addWidget(extKeystoreDerivePassphraseInput);
    extKeystoreDeriveLayout->addWidget(extKeystoreDeriveNewPassphraseInput);
    extKeystoreDeriveLayout->addWidget(extKeystoreDeriveButton);
    extKeystoreLayout->addLayout(extKeystoreDeriveLayout);
    extKeystoreDeriveResult = new QLabel("Derived address will appear here...", accountsScrollContent);
    extKeystoreLayout->addWidget(extKeystoreDeriveResult);
    
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
    mnemonicResult = new QLabel("Mnemonic will appear here...", accountsScrollContent);
    mnemonicResult->setWordWrap(true);
    mnemonicLayout->addWidget(mnemonicResult);
    
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
    createExtKeyResult = new QLabel("Extended key will appear here...", accountsScrollContent);
    createExtKeyResult->setWordWrap(true);
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
    deriveExtKeyResult = new QLabel("Derived key will appear here...", accountsScrollContent);
    deriveExtKeyResult->setWordWrap(true);
    keyOpsLayout->addWidget(deriveExtKeyResult);
    
    // Add accounts-related frames to Accounts tab (Mnemonic and Key Operations at top)
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
    
    // Connect keystore signals
    connect(keystoreInitButton, &QPushButton::clicked, this, &WalletWidget::onKeystoreInitButtonClicked);
    connect(keystoreListAccountsButton, &QPushButton::clicked, this, &WalletWidget::onKeystoreListAccountsButtonClicked);
    connect(keystoreNewAccountButton, &QPushButton::clicked, this, &WalletWidget::onKeystoreNewAccountButtonClicked);
    connect(keystoreImportButton, &QPushButton::clicked, this, &WalletWidget::onKeystoreImportButtonClicked);
    connect(keystoreImportECDSAButton, &QPushButton::clicked, this, &WalletWidget::onKeystoreImportECDSAButtonClicked);
    connect(keystoreExportButton, &QPushButton::clicked, this, &WalletWidget::onKeystoreExportButtonClicked);
    connect(keystoreDeleteButton, &QPushButton::clicked, this, &WalletWidget::onKeystoreDeleteButtonClicked);
    connect(keystoreUnlockButton, &QPushButton::clicked, this, &WalletWidget::onKeystoreUnlockButtonClicked);
    connect(keystoreLockButton, &QPushButton::clicked, this, &WalletWidget::onKeystoreLockButtonClicked);
    connect(keystoreSignHashButton, &QPushButton::clicked, this, &WalletWidget::onKeystoreSignHashButtonClicked);
    
    // Connect extended keystore signals
    connect(extKeystoreInitButton, &QPushButton::clicked, this, &WalletWidget::onExtKeystoreInitButtonClicked);
    connect(extKeystoreListAccountsButton, &QPushButton::clicked, this, &WalletWidget::onExtKeystoreListAccountsButtonClicked);
    connect(extKeystoreNewAccountButton, &QPushButton::clicked, this, &WalletWidget::onExtKeystoreNewAccountButtonClicked);
    connect(extKeystoreImportButton, &QPushButton::clicked, this, &WalletWidget::onExtKeystoreImportButtonClicked);
    connect(extKeystoreImportExtendedKeyButton, &QPushButton::clicked, this, &WalletWidget::onExtKeystoreImportExtendedKeyButtonClicked);
    connect(extKeystoreExportButton, &QPushButton::clicked, this, &WalletWidget::onExtKeystoreExportButtonClicked);
    connect(extKeystoreExportPrivButton, &QPushButton::clicked, this, &WalletWidget::onExtKeystoreExportPrivButtonClicked);
    connect(extKeystoreDeleteButton, &QPushButton::clicked, this, &WalletWidget::onExtKeystoreDeleteButtonClicked);
    connect(extKeystoreUnlockButton, &QPushButton::clicked, this, &WalletWidget::onExtKeystoreUnlockButtonClicked);
    connect(extKeystoreLockButton, &QPushButton::clicked, this, &WalletWidget::onExtKeystoreLockButtonClicked);
    connect(extKeystoreSignHashButton, &QPushButton::clicked, this, &WalletWidget::onExtKeystoreSignHashButtonClicked);
    connect(extKeystoreDeriveButton, &QPushButton::clicked, this, &WalletWidget::onExtKeystoreDeriveButtonClicked);
    
    // Connect mnemonic signals
    connect(createMnemonicButton, &QPushButton::clicked, this, &WalletWidget::onCreateMnemonicButtonClicked);
    
    // Connect key operations signals
    connect(createExtKeyFromMnemonicButton, &QPushButton::clicked, this, &WalletWidget::onCreateExtKeyFromMnemonicButtonClicked);
    connect(deriveExtKeyButton, &QPushButton::clicked, this, &WalletWidget::onDeriveExtKeyButtonClicked);

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
    bool success = logos->wallet_module.initKeystore(dir);
    
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
    QString accounts = logos->wallet_module.keystoreAccounts();
    
    keystoreAccountsList->clear();
    keystoreAccountsList->addItem(accounts);
}

void WalletWidget::onKeystoreNewAccountButtonClicked() {
    QString passphrase = keystoreNewAccountPassphraseInput->text();
    
    if (passphrase.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter a passphrase");
        return;
    }
    
    updateStatus("Creating new account...");
    QString address = logos->wallet_module.keystoreNewAccount(passphrase);
    
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
    QString address = logos->wallet_module.keystoreImport(keyJSON, passphrase, newPassphrase);
    
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

void WalletWidget::onKeystoreImportECDSAButtonClicked() {
    QString privateKey = keystoreImportECDSAKeyInput->text();
    QString newPassphrase = keystoreImportECDSAPassphraseInput->text();
    
    if (privateKey.isEmpty() || newPassphrase.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields");
        return;
    }
    
    updateStatus("Importing ECDSA key...");
    QString address = logos->wallet_module.keystoreImportECDSA(privateKey, newPassphrase);
    
    if (!address.isEmpty()) {
        updateStatus(QString("ECDSA key imported: %1").arg(address));
        QMessageBox::information(this, "Success", QString("ECDSA key imported:\n%1").arg(address));
        keystoreImportECDSAKeyInput->clear();
        keystoreImportECDSAPassphraseInput->clear();
        onKeystoreListAccountsButtonClicked(); // Refresh list
    } else {
        updateStatus("Failed to import ECDSA key");
        QMessageBox::warning(this, "Error", "Failed to import ECDSA key");
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
    QString keyJSON = logos->wallet_module.keystoreExport(address, passphrase, newPassphrase);
    
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
        bool success = logos->wallet_module.keystoreDelete(address, passphrase);
        
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
    bool success = logos->wallet_module.keystoreUnlock(address, passphrase);
    
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
    bool success = logos->wallet_module.keystoreLock(address);
    
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
    QString signature = logos->wallet_module.keystoreSignHash(address, hashHex);
    
    if (!signature.isEmpty()) {
        keystoreSignHashResult->setText(QString("Signature: %1").arg(signature));
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
    bool success = logos->wallet_module.initExtKeystore(dir);
    
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
    QString accounts = logos->wallet_module.extKeystoreAccounts();
    
    extKeystoreAccountsList->clear();
    extKeystoreAccountsList->addItem(accounts);
}

void WalletWidget::onExtKeystoreNewAccountButtonClicked() {
    QString passphrase = extKeystoreNewAccountPassphraseInput->text();
    
    if (passphrase.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter a passphrase");
        return;
    }
    
    updateStatus("Creating new extended keystore account...");
    QString address = logos->wallet_module.extKeystoreNewAccount(passphrase);
    
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
    QString address = logos->wallet_module.extKeystoreImport(keyJSON, passphrase, newPassphrase);
    
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

void WalletWidget::onExtKeystoreImportExtendedKeyButtonClicked() {
    QString extendedKey = extKeystoreImportExtendedKeyInput->text();
    QString newPassphrase = extKeystoreImportExtendedKeyPassphraseInput->text();
    
    if (extendedKey.isEmpty() || newPassphrase.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields");
        return;
    }
    
    updateStatus("Importing extended key...");
    QString address = logos->wallet_module.extKeystoreImportExtendedKey(extendedKey, newPassphrase);
    
    if (!address.isEmpty()) {
        updateStatus(QString("Extended key imported: %1").arg(address));
        QMessageBox::information(this, "Success", QString("Extended key imported:\n%1").arg(address));
        extKeystoreImportExtendedKeyInput->clear();
        extKeystoreImportExtendedKeyPassphraseInput->clear();
        onExtKeystoreListAccountsButtonClicked(); // Refresh list
    } else {
        updateStatus("Failed to import extended key");
        QMessageBox::warning(this, "Error", "Failed to import extended key");
    }
}

void WalletWidget::onExtKeystoreExportButtonClicked() {
    QString address = extKeystoreExportAddressInput->text();
    QString passphrase = extKeystoreExportPassphraseInput->text();
    QString newPassphrase = extKeystoreExportNewPassphraseInput->text();
    
    if (address.isEmpty() || passphrase.isEmpty() || newPassphrase.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields");
        return;
    }
    
    updateStatus("Exporting extended keystore account...");
    QString keyJSON = logos->wallet_module.extKeystoreExportExt(address, passphrase, newPassphrase);
    
    if (!keyJSON.isEmpty()) {
        extKeystoreExportResult->setPlainText(keyJSON);
        updateStatus(QString("Extended keystore account exported: %1").arg(address));
    } else {
        extKeystoreExportResult->setPlainText("Export failed");
        updateStatus("Failed to export extended keystore account");
        QMessageBox::warning(this, "Error", "Failed to export extended keystore account");
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
    
    updateStatus("Exporting extended keystore private key...");
    QString keyJSON = logos->wallet_module.extKeystoreExportPriv(address, passphrase, newPassphrase);
    
    if (!keyJSON.isEmpty()) {
        extKeystoreExportPrivResult->setPlainText(keyJSON);
        updateStatus(QString("Extended keystore private key exported: %1").arg(address));
    } else {
        extKeystoreExportPrivResult->setPlainText("Export failed");
        updateStatus("Failed to export extended keystore private key");
        QMessageBox::warning(this, "Error", "Failed to export extended keystore private key");
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
        QString("Are you sure you want to delete extended keystore account %1?").arg(address),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        updateStatus("Deleting extended keystore account...");
        bool success = logos->wallet_module.extKeystoreDelete(address, passphrase);
        
        if (success) {
            updateStatus(QString("Extended keystore account deleted: %1").arg(address));
            QMessageBox::information(this, "Success", "Extended keystore account deleted successfully");
            extKeystoreDeleteAddressInput->clear();
            extKeystoreDeletePassphraseInput->clear();
            onExtKeystoreListAccountsButtonClicked(); // Refresh list
        } else {
            updateStatus("Failed to delete extended keystore account");
            QMessageBox::warning(this, "Error", "Failed to delete extended keystore account");
        }
    }
}

void WalletWidget::onExtKeystoreUnlockButtonClicked() {
    QString address = extKeystoreUnlockAddressInput->text();
    QString passphrase = extKeystoreUnlockPassphraseInput->text();
    
    if (address.isEmpty() || passphrase.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields");
        return;
    }
    
    updateStatus("Unlocking extended keystore account...");
    bool success = logos->wallet_module.extKeystoreUnlock(address, passphrase);
    
    if (success) {
        updateStatus(QString("Extended keystore account unlocked: %1").arg(address));
        QMessageBox::information(this, "Success", "Extended keystore account unlocked successfully");
        extKeystoreUnlockPassphraseInput->clear();
    } else {
        updateStatus("Failed to unlock extended keystore account");
        QMessageBox::warning(this, "Error", "Failed to unlock extended keystore account");
    }
}

void WalletWidget::onExtKeystoreLockButtonClicked() {
    QString address = extKeystoreLockAddressInput->text();
    
    if (address.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter an address");
        return;
    }
    
    updateStatus("Locking extended keystore account...");
    bool success = logos->wallet_module.extKeystoreLock(address);
    
    if (success) {
        updateStatus(QString("Extended keystore account locked: %1").arg(address));
        QMessageBox::information(this, "Success", "Extended keystore account locked successfully");
        extKeystoreLockAddressInput->clear();
    } else {
        updateStatus("Failed to lock extended keystore account");
        QMessageBox::warning(this, "Error", "Failed to lock extended keystore account");
    }
}

void WalletWidget::onExtKeystoreSignHashButtonClicked() {
    QString address = extKeystoreSignHashAddressInput->text();
    QString hashHex = extKeystoreSignHashHashInput->text();
    
    if (address.isEmpty() || hashHex.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields");
        return;
    }
    
    updateStatus("Signing hash with extended keystore...");
    QString signature = logos->wallet_module.extKeystoreSignHash(address, hashHex);
    
    if (!signature.isEmpty()) {
        extKeystoreSignHashResult->setText(QString("Signature: %1").arg(signature));
        updateStatus("Hash signed successfully with extended keystore");
    } else {
        extKeystoreSignHashResult->setText("Signing failed");
        updateStatus("Failed to sign hash with extended keystore");
        QMessageBox::warning(this, "Error", "Failed to sign hash with extended keystore");
    }
}

void WalletWidget::onExtKeystoreDeriveButtonClicked() {
    QString address = extKeystoreDeriveAddressInput->text();
    QString derivationPath = extKeystoreDerivePathInput->text();
    QString passphrase = extKeystoreDerivePassphraseInput->text();
    QString newPassphrase = extKeystoreDeriveNewPassphraseInput->text();
    
    if (address.isEmpty() || derivationPath.isEmpty() || passphrase.isEmpty() || newPassphrase.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields");
        return;
    }
    
    updateStatus("Deriving address...");
    QString derivedAddress = logos->wallet_module.extKeystoreDeriveWithPassphrase(address, derivationPath, passphrase, newPassphrase);
    
    if (!derivedAddress.isEmpty()) {
        extKeystoreDeriveResult->setText(QString("Derived Address: %1").arg(derivedAddress));
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
        mnemonic = logos->wallet_module.createRandomMnemonicWithDefaultLength();
    } else {
        int length = lengthStr.toInt();
        mnemonic = logos->wallet_module.createRandomMnemonic(length);
    }

    qDebug() << "Mnemonic: " << mnemonic;
    
    if (!mnemonic.isEmpty()) {
        mnemonicResult->setText(QString("Mnemonic: %1").arg(mnemonic));
        updateStatus("Mnemonic created successfully");
    } else {
        mnemonicResult->setText("Failed to create mnemonic");
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
    QString extKey = logos->wallet_module.createExtKeyFromMnemonic(phrase, passphrase);
    
    if (!extKey.isEmpty()) {
        createExtKeyResult->setText(QString("Extended Key: %1").arg(extKey));
        updateStatus("Extended key created successfully");
    } else {
        createExtKeyResult->setText("Failed to create extended key");
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
    QString derivedKey = logos->wallet_module.deriveExtKey(extKeyStr, pathStr);
    
    if (!derivedKey.isEmpty()) {
        deriveExtKeyResult->setText(QString("Derived Key: %1").arg(derivedKey));
        updateStatus("Extended key derived successfully");
    } else {
        deriveExtKeyResult->setText("Derivation failed");
        updateStatus("Failed to derive extended key");
        QMessageBox::warning(this, "Error", "Failed to derive extended key");
    }
}