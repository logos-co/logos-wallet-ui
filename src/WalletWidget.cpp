#include "WalletWidget.h"
#include <QDebug>
#include <QDateTime>
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
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
    
    // Create EthClient tab (first tab)
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

    // Add all EthClient components to tab layout
    ethClientTabLayout->addWidget(rpcCallFrame);
    ethClientTabLayout->addWidget(chainIDFrame);
    ethClientTabLayout->addWidget(ethBalanceFrame);
    ethClientTabLayout->addStretch();
    
    // Create Transactions tab with scroll area
    transactionsTab = new QWidget(this);
    transactionsScrollArea = new QScrollArea(transactionsTab);
    transactionsScrollArea->setWidgetResizable(true);
    transactionsScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    transactionsScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    transactionsScrollContent = new QWidget();
    transactionsTabLayout = new QVBoxLayout(transactionsScrollContent);
    transactionsTabLayout->setSpacing(10);
    transactionsTabLayout->setContentsMargins(10, 10, 10, 10);
    
    transactionsScrollArea->setWidget(transactionsScrollContent);
    
    QVBoxLayout* transactionsTabMainLayout = new QVBoxLayout(transactionsTab);
    transactionsTabMainLayout->setContentsMargins(0, 0, 0, 0);
    transactionsTabMainLayout->addWidget(transactionsScrollArea);
    
    // Create Transactions UI elements
    setupTransactionsTab();
    
    // Add tabs to tab widget (EthClient first, then Transactions)
    tabWidget->addTab(ethClientTab, "EthClient");
    tabWidget->addTab(transactionsTab, "Transactions");
    
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

// Transaction generator handlers
void WalletWidget::onTxTypeChanged(int index) {
    if (index > 0) {
        QString txType = txTypeComboBox->itemData(index).toString();
        updateTxParamsFields(txType);
    } else {
        // Clear params fields
        updateTxParamsFields("");
    }
}

void WalletWidget::onFeeTypeChanged(int id) {
    if (id == 0) { // Legacy
        legacyFeeGroup->setVisible(true);
        eip1559FeeGroup->setVisible(false);
    } else { // EIP-1559
        legacyFeeGroup->setVisible(false);
        eip1559FeeGroup->setVisible(true);
    }
}

void WalletWidget::onGenerateTransactionButtonClicked() {
    int txTypeIndex = txTypeComboBox->currentIndex();
    if (txTypeIndex == 0) {
        QMessageBox::warning(this, "Error", "Please select a transaction type");
        return;
    }
    
    QString paramsJSON = buildTxParamsJSON();
    if (paramsJSON.isEmpty()) {
        QMessageBox::warning(this, "Error", "Failed to build transaction parameters");
        return;
    }
    
    QString txType = txTypeComboBox->itemData(txTypeIndex).toString();
    QString result;
    
    updateStatus("Generating transaction...");
    
    if (txType == "transferETH") {
        result = logos->wallet_module.txGeneratorTransferETH(paramsJSON);
    } else if (txType == "transferERC20") {
        result = logos->wallet_module.txGeneratorTransferERC20(paramsJSON);
    } else if (txType == "approveERC20") {
        result = logos->wallet_module.txGeneratorApproveERC20(paramsJSON);
    } else if (txType == "transferFromERC721") {
        result = logos->wallet_module.txGeneratorTransferFromERC721(paramsJSON);
    } else if (txType == "safeTransferFromERC721") {
        result = logos->wallet_module.txGeneratorSafeTransferFromERC721(paramsJSON);
    } else if (txType == "approveERC721") {
        result = logos->wallet_module.txGeneratorApproveERC721(paramsJSON);
    } else if (txType == "setApprovalForAllERC721") {
        result = logos->wallet_module.txGeneratorSetApprovalForAllERC721(paramsJSON);
    } else if (txType == "transferERC1155") {
        result = logos->wallet_module.txGeneratorTransferERC1155(paramsJSON);
    } else if (txType == "batchTransferERC1155") {
        result = logos->wallet_module.txGeneratorBatchTransferERC1155(paramsJSON);
    } else if (txType == "setApprovalForAllERC1155") {
        result = logos->wallet_module.txGeneratorSetApprovalForAllERC1155(paramsJSON);
    }
    
    txResultOutput->setPlainText(result);
    updateStatus("Transaction generated");
}

// Transaction utility handlers
void WalletWidget::onTransactionJsonToRlpButtonClicked() {
    QString txJSON = txJsonToRlpInput->toPlainText();
    if (txJSON.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter transaction JSON");
        return;
    }
    updateStatus("Converting transaction JSON to RLP...");
    QString result = logos->wallet_module.transactionJsonToRlp(txJSON);
    txJsonToRlpResult->setPlainText(result);
    updateStatus("Transaction JSON converted to RLP");
}

void WalletWidget::onTransactionRlpToJsonButtonClicked() {
    QString rlpHex = txRlpToJsonInput->text();
    if (rlpHex.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter RLP hex");
        return;
    }
    updateStatus("Converting RLP to transaction JSON...");
    QString result = logos->wallet_module.transactionRlpToJson(rlpHex);
    txRlpToJsonResult->setPlainText(result);
    updateStatus("RLP converted to transaction JSON");
}

void WalletWidget::onTransactionGetHashButtonClicked() {
    QString txJSON = txGetHashInput->toPlainText();
    if (txJSON.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter transaction JSON");
        return;
    }
    updateStatus("Getting transaction hash...");
    QString result = logos->wallet_module.transactionGetHash(txJSON);
    txGetHashResult->setText(result);
    updateStatus("Transaction hash retrieved");
}

void WalletWidget::setupTransactionsTab() {
    // Create main transaction generator frame
    QFrame* txGeneratorFrame = new QFrame(transactionsScrollContent);
    QVBoxLayout* txGeneratorLayout = new QVBoxLayout(txGeneratorFrame);
    txGeneratorFrame->setLayout(txGeneratorLayout);
    txGeneratorFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    txGeneratorFrame->setLineWidth(1);
    
    QLabel* txGeneratorTitle = new QLabel("<b>🔷 Transaction Generator</b>", transactionsScrollContent);
    txGeneratorTitle->setAlignment(Qt::AlignCenter);
    txGeneratorLayout->addWidget(txGeneratorTitle);
    
    QLabel* txGeneratorInfo = new QLabel("Select a transaction type, choose fee type (Legacy or EIP-1559), fill in the required parameters, and click \"Generate Transaction\" to get the transaction in JSON format.", transactionsScrollContent);
    txGeneratorInfo->setWordWrap(true);
    txGeneratorInfo->setStyleSheet("background-color: #d1ecf1; color: #0c5460; padding: 10px; border-radius: 5px;");
    txGeneratorLayout->addWidget(txGeneratorInfo);
    
    // Transaction type selector
    QLabel* txTypeLabel = new QLabel("Transaction Type:", transactionsScrollContent);
    txGeneratorLayout->addWidget(txTypeLabel);
    txTypeComboBox = new QComboBox(transactionsScrollContent);
    txTypeComboBox->addItem("-- Select Transaction Type --", "");
    txTypeComboBox->addItem("Transfer ETH", "transferETH");
    txTypeComboBox->addItem("Transfer ERC20", "transferERC20");
    txTypeComboBox->addItem("Approve ERC20", "approveERC20");
    txTypeComboBox->addItem("Transfer ERC721 (transferFrom)", "transferFromERC721");
    txTypeComboBox->addItem("Transfer ERC721 (safeTransferFrom)", "safeTransferFromERC721");
    txTypeComboBox->addItem("Approve ERC721", "approveERC721");
    txTypeComboBox->addItem("Set Approval For All ERC721", "setApprovalForAllERC721");
    txTypeComboBox->addItem("Transfer ERC1155", "transferERC1155");
    txTypeComboBox->addItem("Batch Transfer ERC1155", "batchTransferERC1155");
    txTypeComboBox->addItem("Set Approval For All ERC1155", "setApprovalForAllERC1155");
    txGeneratorLayout->addWidget(txTypeComboBox);
    
    // Fee type selector
    QLabel* feeTypeLabel = new QLabel("Fee Type:", transactionsScrollContent);
    txGeneratorLayout->addWidget(feeTypeLabel);
    feeTypeButtonGroup = new QButtonGroup(transactionsScrollContent);
    feeLegacyRadio = new QRadioButton("Legacy (GasPrice)", transactionsScrollContent);
    feeEIP1559Radio = new QRadioButton("EIP-1559 (MaxFeePerGas + MaxPriorityFeePerGas)", transactionsScrollContent);
    feeLegacyRadio->setChecked(true);
    feeTypeButtonGroup->addButton(feeLegacyRadio, 0);
    feeTypeButtonGroup->addButton(feeEIP1559Radio, 1);
    QHBoxLayout* feeTypeLayout = new QHBoxLayout();
    feeTypeLayout->addWidget(feeLegacyRadio);
    feeTypeLayout->addWidget(feeEIP1559Radio);
    txGeneratorLayout->addLayout(feeTypeLayout);
    
    // Common fields
    QLabel* nonceLabel = new QLabel("Nonce:", transactionsScrollContent);
    txGeneratorLayout->addWidget(nonceLabel);
    txNonceInput = new QLineEdit(transactionsScrollContent);
    txNonceInput->setPlaceholderText("0");
    txNonceInput->setText("0");
    txGeneratorLayout->addWidget(txNonceInput);
    
    QLabel* gasLimitLabel = new QLabel("Gas Limit:", transactionsScrollContent);
    txGeneratorLayout->addWidget(gasLimitLabel);
    txGasLimitInput = new QLineEdit(transactionsScrollContent);
    txGasLimitInput->setPlaceholderText("21000");
    txGasLimitInput->setText("21000");
    txGeneratorLayout->addWidget(txGasLimitInput);
    
    QLabel* chainIDLabel = new QLabel("Chain ID:", transactionsScrollContent);
    txGeneratorLayout->addWidget(chainIDLabel);
    txChainIDInput = new QLineEdit(transactionsScrollContent);
    txChainIDInput->setPlaceholderText("1");
    txChainIDInput->setText("1");
    txGeneratorLayout->addWidget(txChainIDInput);
    
    // Legacy fee group
    legacyFeeGroup = new QGroupBox("Legacy Fee", transactionsScrollContent);
    QVBoxLayout* legacyFeeLayout = new QVBoxLayout(legacyFeeGroup);
    QLabel* gasPriceLabel = new QLabel("Gas Price (wei):", transactionsScrollContent);
    legacyFeeLayout->addWidget(gasPriceLabel);
    txGasPriceInput = new QLineEdit(transactionsScrollContent);
    txGasPriceInput->setPlaceholderText("20000000000");
    txGasPriceInput->setText("20000000000");
    legacyFeeLayout->addWidget(txGasPriceInput);
    txGeneratorLayout->addWidget(legacyFeeGroup);
    
    // EIP-1559 fee group
    eip1559FeeGroup = new QGroupBox("EIP-1559 Fee", transactionsScrollContent);
    QVBoxLayout* eip1559FeeLayout = new QVBoxLayout(eip1559FeeGroup);
    QLabel* maxFeePerGasLabel = new QLabel("Max Fee Per Gas (wei):", transactionsScrollContent);
    eip1559FeeLayout->addWidget(maxFeePerGasLabel);
    txMaxFeePerGasInput = new QLineEdit(transactionsScrollContent);
    txMaxFeePerGasInput->setPlaceholderText("30000000000");
    txMaxFeePerGasInput->setText("30000000000");
    eip1559FeeLayout->addWidget(txMaxFeePerGasInput);
    QLabel* maxPriorityFeePerGasLabel = new QLabel("Max Priority Fee Per Gas (wei):", transactionsScrollContent);
    eip1559FeeLayout->addWidget(maxPriorityFeePerGasLabel);
    txMaxPriorityFeePerGasInput = new QLineEdit(transactionsScrollContent);
    txMaxPriorityFeePerGasInput->setPlaceholderText("2000000000");
    txMaxPriorityFeePerGasInput->setText("2000000000");
    eip1559FeeLayout->addWidget(txMaxPriorityFeePerGasInput);
    eip1559FeeGroup->setVisible(false);
    txGeneratorLayout->addWidget(eip1559FeeGroup);
    
    // Dynamic parameter fields container
    QLabel* paramsLabel = new QLabel("Transaction Parameters:", transactionsScrollContent);
    txGeneratorLayout->addWidget(paramsLabel);
    txParamsContainer = new QWidget(transactionsScrollContent);
    txParamsLayout = new QVBoxLayout(txParamsContainer);
    txParamsContainer->setLayout(txParamsLayout);
    txGeneratorLayout->addWidget(txParamsContainer);
    
    // Generate button
    txGenerateButton = new QPushButton("Generate Transaction", transactionsScrollContent);
    txGeneratorLayout->addWidget(txGenerateButton);
    
    // Result output
    QLabel* resultLabel = new QLabel("Generated Transaction:", transactionsScrollContent);
    txGeneratorLayout->addWidget(resultLabel);
    txResultOutput = new QTextEdit(transactionsScrollContent);
    txResultOutput->setReadOnly(true);
    txResultOutput->setPlaceholderText("Transaction JSON will appear here...");
    txResultOutput->setMaximumHeight(200);
    txGeneratorLayout->addWidget(txResultOutput);
    
    transactionsTabLayout->addWidget(txGeneratorFrame);
    transactionsTabLayout->addWidget(createSeparator());
    
    // Connect signals
    connect(txTypeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &WalletWidget::onTxTypeChanged);
    connect(feeTypeButtonGroup, &QButtonGroup::idClicked, this, &WalletWidget::onFeeTypeChanged);
    connect(txGenerateButton, &QPushButton::clicked, this, &WalletWidget::onGenerateTransactionButtonClicked);
    
    // Transaction utility methods (keep these as they were)
    // Transaction Json To Rlp
    txJsonToRlpFrame = new QFrame(transactionsScrollContent);
    QVBoxLayout* txJsonToRlpLayout = new QVBoxLayout(txJsonToRlpFrame);
    txJsonToRlpFrame->setLayout(txJsonToRlpLayout);
    txJsonToRlpFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    txJsonToRlpFrame->setLineWidth(1);
    
    QLabel* txJsonToRlpTitle = new QLabel("<b>Transaction JSON to RLP</b>", transactionsScrollContent);
    txJsonToRlpLayout->addWidget(txJsonToRlpTitle);
    
    QLabel* txJsonToRlpInputLabel = new QLabel("Transaction JSON:", transactionsScrollContent);
    txJsonToRlpLayout->addWidget(txJsonToRlpInputLabel);
    txJsonToRlpInput = new QTextEdit(transactionsScrollContent);
    txJsonToRlpInput->setPlaceholderText("{\"nonce\": \"0x...\", \"gasPrice\": \"0x...\", ...}");
    txJsonToRlpInput->setMaximumHeight(80);
    txJsonToRlpLayout->addWidget(txJsonToRlpInput);
    
    txJsonToRlpButton = new QPushButton("Convert JSON to RLP", transactionsScrollContent);
    txJsonToRlpLayout->addWidget(txJsonToRlpButton);
    
    txJsonToRlpResultLabel = new QLabel("RLP Result:", transactionsScrollContent);
    txJsonToRlpLayout->addWidget(txJsonToRlpResultLabel);
    txJsonToRlpResult = new QTextEdit(transactionsScrollContent);
    txJsonToRlpResult->setReadOnly(true);
    txJsonToRlpResult->setPlaceholderText("RLP hex will appear here...");
    txJsonToRlpResult->setMaximumHeight(100);
    txJsonToRlpLayout->addWidget(txJsonToRlpResult);
    transactionsTabLayout->addWidget(txJsonToRlpFrame);
    transactionsTabLayout->addWidget(createSeparator());
    
    // Transaction Rlp To Json
    txRlpToJsonFrame = new QFrame(transactionsScrollContent);
    QVBoxLayout* txRlpToJsonLayout = new QVBoxLayout(txRlpToJsonFrame);
    txRlpToJsonFrame->setLayout(txRlpToJsonLayout);
    txRlpToJsonFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    txRlpToJsonFrame->setLineWidth(1);
    
    QLabel* txRlpToJsonTitle = new QLabel("<b>Transaction RLP to JSON</b>", transactionsScrollContent);
    txRlpToJsonLayout->addWidget(txRlpToJsonTitle);
    
    QLabel* txRlpToJsonInputLabel = new QLabel("RLP Hex:", transactionsScrollContent);
    txRlpToJsonLayout->addWidget(txRlpToJsonInputLabel);
    txRlpToJsonInput = new QLineEdit(transactionsScrollContent);
    txRlpToJsonInput->setPlaceholderText("0x...");
    txRlpToJsonLayout->addWidget(txRlpToJsonInput);
    
    txRlpToJsonButton = new QPushButton("Convert RLP to JSON", transactionsScrollContent);
    txRlpToJsonLayout->addWidget(txRlpToJsonButton);
    
    txRlpToJsonResultLabel = new QLabel("JSON Result:", transactionsScrollContent);
    txRlpToJsonLayout->addWidget(txRlpToJsonResultLabel);
    txRlpToJsonResult = new QTextEdit(transactionsScrollContent);
    txRlpToJsonResult->setReadOnly(true);
    txRlpToJsonResult->setPlaceholderText("Transaction JSON will appear here...");
    txRlpToJsonResult->setMaximumHeight(100);
    txRlpToJsonLayout->addWidget(txRlpToJsonResult);
    transactionsTabLayout->addWidget(txRlpToJsonFrame);
    transactionsTabLayout->addWidget(createSeparator());
    
    // Transaction Get Hash
    txGetHashFrame = new QFrame(transactionsScrollContent);
    QVBoxLayout* txGetHashLayout = new QVBoxLayout(txGetHashFrame);
    txGetHashFrame->setLayout(txGetHashLayout);
    txGetHashFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    txGetHashFrame->setLineWidth(1);
    
    QLabel* txGetHashTitle = new QLabel("<b>Get Transaction Hash</b>", transactionsScrollContent);
    txGetHashLayout->addWidget(txGetHashTitle);
    
    QLabel* txGetHashInputLabel = new QLabel("Transaction JSON:", transactionsScrollContent);
    txGetHashLayout->addWidget(txGetHashInputLabel);
    txGetHashInput = new QTextEdit(transactionsScrollContent);
    txGetHashInput->setPlaceholderText("{\"nonce\": \"0x...\", \"gasPrice\": \"0x...\", ...}");
    txGetHashInput->setMaximumHeight(80);
    txGetHashLayout->addWidget(txGetHashInput);
    
    txGetHashButton = new QPushButton("Get Transaction Hash", transactionsScrollContent);
    txGetHashLayout->addWidget(txGetHashButton);
    
    txGetHashResultLabel = new QLabel("Hash Result:", transactionsScrollContent);
    txGetHashLayout->addWidget(txGetHashResultLabel);
    txGetHashResult = new QLineEdit(transactionsScrollContent);
    txGetHashResult->setReadOnly(true);
    txGetHashResult->setPlaceholderText("Transaction hash will appear here...");
    txGetHashLayout->addWidget(txGetHashResult);
    transactionsTabLayout->addWidget(txGetHashFrame);
    
    // Connect utility button signals
    connect(txJsonToRlpButton, &QPushButton::clicked, this, &WalletWidget::onTransactionJsonToRlpButtonClicked);
    connect(txRlpToJsonButton, &QPushButton::clicked, this, &WalletWidget::onTransactionRlpToJsonButtonClicked);
    connect(txGetHashButton, &QPushButton::clicked, this, &WalletWidget::onTransactionGetHashButtonClicked);
}

QFrame* WalletWidget::createSeparator() {
    QFrame* separator = new QFrame(transactionsScrollContent);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setMaximumHeight(2);
    return separator;
}

void WalletWidget::updateTxParamsFields(const QString& txType) {
    // Clear existing parameter fields (both labels and inputs)
    for (QWidget* widget : txParamWidgets) {
        txParamsLayout->removeWidget(widget);
        delete widget;
    }
    txParamWidgets.clear();
    txParamInputs.clear();
    
    if (txType.isEmpty()) {
        return;
    }
    
    // Define parameters for each transaction type based on the example
    QList<QPair<QString, QString>> params; // name, placeholder
    
    if (txType == "transferETH") {
        params << QPair<QString, QString>("to", "0x...");
        params << QPair<QString, QString>("value", "1000000000000000000");
    } else if (txType == "transferERC20") {
        params << QPair<QString, QString>("tokenAddress", "0x...");
        params << QPair<QString, QString>("to", "0x...");
        params << QPair<QString, QString>("amount", "1000000");
    } else if (txType == "approveERC20") {
        params << QPair<QString, QString>("tokenAddress", "0x...");
        params << QPair<QString, QString>("spender", "0x...");
        params << QPair<QString, QString>("amount", "1000000");
    } else if (txType == "transferFromERC721") {
        params << QPair<QString, QString>("tokenAddress", "0x...");
        params << QPair<QString, QString>("from", "0x...");
        params << QPair<QString, QString>("to", "0x...");
        params << QPair<QString, QString>("tokenID", "1234");
    } else if (txType == "safeTransferFromERC721") {
        params << QPair<QString, QString>("tokenAddress", "0x...");
        params << QPair<QString, QString>("from", "0x...");
        params << QPair<QString, QString>("to", "0x...");
        params << QPair<QString, QString>("tokenID", "1234");
    } else if (txType == "approveERC721") {
        params << QPair<QString, QString>("tokenAddress", "0x...");
        params << QPair<QString, QString>("to", "0x...");
        params << QPair<QString, QString>("tokenID", "1234");
    } else if (txType == "setApprovalForAllERC721") {
        params << QPair<QString, QString>("tokenAddress", "0x...");
        params << QPair<QString, QString>("operator", "0x...");
        params << QPair<QString, QString>("approved", "true");
    } else if (txType == "transferERC1155") {
        params << QPair<QString, QString>("tokenAddress", "0x...");
        params << QPair<QString, QString>("from", "0x...");
        params << QPair<QString, QString>("to", "0x...");
        params << QPair<QString, QString>("tokenID", "1234");
        params << QPair<QString, QString>("value", "1");
    } else if (txType == "batchTransferERC1155") {
        params << QPair<QString, QString>("tokenAddress", "0x...");
        params << QPair<QString, QString>("from", "0x...");
        params << QPair<QString, QString>("to", "0x...");
        params << QPair<QString, QString>("tokenIDs", "1,2,3");
        params << QPair<QString, QString>("values", "1,2,3");
    } else if (txType == "setApprovalForAllERC1155") {
        params << QPair<QString, QString>("tokenAddress", "0x...");
        params << QPair<QString, QString>("operator", "0x...");
        params << QPair<QString, QString>("approved", "true");
    }
    
    // Create input fields for each parameter
    for (const auto& param : params) {
        QLabel* label = new QLabel(param.first + ":", transactionsScrollContent);
        txParamsLayout->addWidget(label);
        txParamWidgets.append(label);
        
        QLineEdit* input = new QLineEdit(transactionsScrollContent);
        input->setPlaceholderText(param.second);
        txParamsLayout->addWidget(input);
        txParamWidgets.append(input);
        txParamInputs[param.first] = input;
    }
}

QString WalletWidget::buildTxParamsJSON() {
    QJsonObject paramsObj;
    
    // Add common transaction fields
    // Nonce and GasLimit are uint64 in Go, so they should be numbers in JSON
    QString nonce = txNonceInput->text();
    QString gasLimit = txGasLimitInput->text();
    QString chainID = txChainIDInput->text();
    
    if (!nonce.isEmpty()) {
        bool ok;
        uint64_t nonceVal = nonce.toULongLong(&ok);
        if (ok) {
            paramsObj["nonce"] = static_cast<qint64>(nonceVal);
        } else {
            paramsObj["nonce"] = nonce.toInt();
        }
    }
    if (!gasLimit.isEmpty()) {
        bool ok;
        uint64_t gasLimitVal = gasLimit.toULongLong(&ok);
        if (ok) {
            paramsObj["gasLimit"] = static_cast<qint64>(gasLimitVal);
        } else {
            paramsObj["gasLimit"] = gasLimit.toInt();
        }
    }
    // ChainID is a string in Go (parsed as big.Int from decimal), so keep as decimal string
    if (!chainID.isEmpty()) {
        paramsObj["chainID"] = chainID;
    }
    
    // Add fee fields based on selected type
    // GasPrice, MaxFeePerGas, MaxPriorityFeePerGas are strings in Go (parsed as big.Int from decimal)
    bool useEIP1559 = feeEIP1559Radio->isChecked();
    if (useEIP1559) {
        QString maxFeePerGas = txMaxFeePerGasInput->text();
        QString maxPriorityFeePerGas = txMaxPriorityFeePerGasInput->text();
        if (!maxFeePerGas.isEmpty()) {
            paramsObj["maxFeePerGas"] = maxFeePerGas; // Decimal string
        }
        if (!maxPriorityFeePerGas.isEmpty()) {
            paramsObj["maxPriorityFeePerGas"] = maxPriorityFeePerGas; // Decimal string
        }
    } else {
        QString gasPrice = txGasPriceInput->text();
        if (!gasPrice.isEmpty()) {
            paramsObj["gasPrice"] = gasPrice; // Decimal string
        }
    }
    
    // Add dynamic parameters
    QMapIterator<QString, QLineEdit*> it(txParamInputs);
    while (it.hasNext()) {
        it.next();
        QString key = it.key();
        QString value = it.value()->text();
        if (!value.isEmpty()) {
            // Handle special cases
            if (key == "approved") {
                // Boolean value
                paramsObj[key] = (value.toLower() == "true");
            } else if (key == "tokenIDs" || key == "values") {
                // Arrays of decimal strings for batchTransferERC1155
                QStringList items = value.split(',', Qt::SkipEmptyParts);
                QJsonArray array;
                for (const QString& item : items) {
                    array.append(item.trimmed()); // Decimal strings
                }
                paramsObj[key] = array;
            } else if (key == "to" || key == "from" || key == "tokenAddress" || 
                       key == "spender" || key == "operator") {
                // Addresses - keep as hex strings (with 0x prefix)
                paramsObj[key] = value;
            } else {
                // Value, Amount, TokenID - these are strings in Go (parsed as big.Int from decimal)
                // So keep as decimal strings, not numbers
                paramsObj[key] = value;
            }
        }
    }
    
    QJsonDocument doc(paramsObj);
    return doc.toJson(QJsonDocument::Compact);
}

void WalletWidget::updateStatus(const QString& message) {
    statusLabel->setText(message);
    qDebug() << message;
}