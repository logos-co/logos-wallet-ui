#include "WalletWidget.h"
#include <QDebug>
#include <QDateTime>
#include <QMessageBox>
#include <QJsonArray>
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

    // Create RPC Call frame
    QFrame* rpcCallFrame = new QFrame(this);
    QVBoxLayout* rpcCallLayout = new QVBoxLayout(rpcCallFrame);
    rpcCallFrame->setLayout(rpcCallLayout);
    rpcCallFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    rpcCallFrame->setLineWidth(1);

    // Create RPC Call button
    rpcCallButton = new QPushButton("RPC Call", this);

    // Create RPC Call method input
    rpcCallMethodInput = new QLineEdit(this);
    rpcCallMethodInput->setPlaceholderText("Enter RPC Call Method...");

    // Create RPC Call params input
    rpcCallParamsInput = new QLineEdit(this);
    rpcCallParamsInput->setPlaceholderText("Enter RPC Call Params...");

    // Create RPC Call result label
    rpcCallResultLabel = new QLabel("RPC Call Result: Not initialized", this);
    rpcCallResultLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // Add RPC Call components to layout
    rpcCallLayout->addWidget(rpcCallButton);
    rpcCallLayout->addWidget(rpcCallMethodInput);
    rpcCallLayout->addWidget(rpcCallParamsInput);
    rpcCallLayout->addWidget(rpcCallResultLabel);

    // Create Chain ID frame
    QFrame* chainIDFrame = new QFrame(this);
    QVBoxLayout* chainIDLayout = new QVBoxLayout(chainIDFrame);
    chainIDFrame->setLayout(chainIDLayout);
    chainIDFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    chainIDFrame->setLineWidth(1);

    // Create Chain ID button
    chainIDButton = new QPushButton("Get Chain ID", this);
    
    // Create Chain ID label
    chainIDLabel = new QLabel("Chain ID: Not initialized", this);

    // Add Chain ID components to layout
    chainIDLayout->addWidget(chainIDButton);
    chainIDLayout->addWidget(chainIDLabel);

    // Create ETH Balance frame
    QFrame* ethBalanceFrame = new QFrame(this);
    QVBoxLayout* ethBalanceLayout = new QVBoxLayout(ethBalanceFrame);
    ethBalanceFrame->setLayout(ethBalanceLayout);
    ethBalanceFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    ethBalanceFrame->setLineWidth(1);

    // Create ETH Balance button
    ethBalanceButton = new QPushButton("Get ETH Balance", this);
    
    // Create ETH Balance address input
    ethBalanceAddressInput = new QLineEdit(this);
    ethBalanceAddressInput->setPlaceholderText("Enter ETH Balance Address...");

    // Create ETH Balance label
    ethBalanceAddressLabel = new QLabel("ETH Balance Address: Not initialized", this);
    ethBalanceAddressLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // Create ETH Balance value label
    ethBalanceValueLabel = new QLabel("ETH Balance Value: Not initialized", this);
    ethBalanceValueLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // Add ETH Balance components to layout
    ethBalanceLayout->addWidget(ethBalanceAddressInput);
    ethBalanceLayout->addWidget(ethBalanceButton);
    ethBalanceLayout->addWidget(ethBalanceAddressLabel);
    ethBalanceLayout->addWidget(ethBalanceValueLabel);

    // Add all components to main layout
    mainLayout->addWidget(statusFrame);
    mainLayout->addWidget(rpcCallFrame);
    mainLayout->addWidget(chainIDFrame);
    mainLayout->addWidget(ethBalanceFrame);
    mainLayout->addStretch();

    // Set spacing and margins
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // Connect signals to slots
    connect(rpcCallButton, &QPushButton::clicked, this, &WalletWidget::onRpcCallButtonClicked);
    connect(chainIDButton, &QPushButton::clicked, this, &WalletWidget::onChainIDButtonClicked);
    connect(ethBalanceButton, &QPushButton::clicked, this, &WalletWidget::onEthBalanceButtonClicked);

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