#include "EthClientTab.h"
#include <QMessageBox>
#include <QDebug>

EthClientTab::EthClientTab(LogosModules* logos, QWidget* parent)
    : QWidget(parent), m_logos(logos) {
    setupUI();
}

void EthClientTab::setupUI() {
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);

    createManagementSection();
    m_mainLayout->addWidget(createSeparator());
    createRpcCallSection();
    createChainIDSection();
    createBalanceSection();
    m_mainLayout->addStretch();

    // Initially disable all operation UI components (no client selected)
    updateUIState();
}

void EthClientTab::createManagementSection() {
    QFrame* frame = new QFrame(this);
    QVBoxLayout* layout = new QVBoxLayout(frame);
    frame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    frame->setLineWidth(1);

    QLabel* title = new QLabel("<b>EthClient Management</b>", this);
    layout->addWidget(title);

    // RPC URL input and Init button
    QHBoxLayout* initLayout = new QHBoxLayout();
    QLabel* rpcUrlLabel = new QLabel("RPC URL:", this);
    initLayout->addWidget(rpcUrlLabel);
    m_rpcUrlInput = new QLineEdit(this);
    m_rpcUrlInput->setPlaceholderText("https://...");
    initLayout->addWidget(m_rpcUrlInput, 1);
    m_initButton = new QPushButton("Init EthClient", this);
    initLayout->addWidget(m_initButton);
    layout->addLayout(initLayout);

    // Refresh and Selector
    QHBoxLayout* selectorLayout = new QHBoxLayout();
    m_refreshButton = new QPushButton("Refresh List", this);
    selectorLayout->addWidget(m_refreshButton);
    QLabel* selectorLabel = new QLabel("Selected EthClient:", this);
    selectorLayout->addWidget(selectorLabel);
    m_clientSelector = new QComboBox(this);
    m_clientSelector->setEditable(false);
    m_clientSelector->addItem("-- No EthClient Selected --", "");
    selectorLayout->addWidget(m_clientSelector, 1);
    m_closeButton = new QPushButton("Close Selected", this);
    selectorLayout->addWidget(m_closeButton);
    layout->addLayout(selectorLayout);

    m_mainLayout->addWidget(frame);

    // Connect signals
    connect(m_initButton, &QPushButton::clicked, this, &EthClientTab::onInitEthClientButtonClicked);
    connect(m_refreshButton, &QPushButton::clicked, this, &EthClientTab::onRefreshEthClientsButtonClicked);
    connect(m_closeButton, &QPushButton::clicked, this, &EthClientTab::onCloseEthClientButtonClicked);
    connect(m_clientSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &EthClientTab::onEthClientSelectionChanged);
}

void EthClientTab::createRpcCallSection() {
    QFrame* frame = new QFrame(this);
    QVBoxLayout* layout = new QVBoxLayout(frame);
    frame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    frame->setLineWidth(1);

    m_rpcCallButton = new QPushButton("RPC Call", this);
    m_rpcCallMethodInput = new QLineEdit(this);
    m_rpcCallMethodInput->setPlaceholderText("Enter RPC Call Method...");
    m_rpcCallParamsInput = new QLineEdit(this);
    m_rpcCallParamsInput->setPlaceholderText("Enter RPC Call Params...");

    QLabel* resultLabel = new QLabel("RPC Call Result:", this);
    m_rpcCallResultOutput = new QTextEdit(this);
    m_rpcCallResultOutput->setReadOnly(true);
    m_rpcCallResultOutput->setPlaceholderText("RPC call result will appear here...");
    m_rpcCallResultOutput->setMaximumHeight(100);

    layout->addWidget(m_rpcCallButton);
    layout->addWidget(m_rpcCallMethodInput);
    layout->addWidget(m_rpcCallParamsInput);
    layout->addWidget(resultLabel);
    layout->addWidget(m_rpcCallResultOutput);

    m_mainLayout->addWidget(frame);

    connect(m_rpcCallButton, &QPushButton::clicked, this, &EthClientTab::onRpcCallButtonClicked);
}

void EthClientTab::createChainIDSection() {
    QFrame* frame = new QFrame(this);
    QVBoxLayout* layout = new QVBoxLayout(frame);
    frame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    frame->setLineWidth(1);

    m_chainIDButton = new QPushButton("Get Chain ID", this);
    QLabel* label = new QLabel("Chain ID:", this);
    m_chainIDOutput = new QLineEdit(this);
    m_chainIDOutput->setReadOnly(true);
    m_chainIDOutput->setPlaceholderText("Chain ID will appear here...");

    layout->addWidget(m_chainIDButton);
    layout->addWidget(label);
    layout->addWidget(m_chainIDOutput);

    m_mainLayout->addWidget(frame);

    connect(m_chainIDButton, &QPushButton::clicked, this, &EthClientTab::onChainIDButtonClicked);
}

void EthClientTab::createBalanceSection() {
    QFrame* frame = new QFrame(this);
    QVBoxLayout* layout = new QVBoxLayout(frame);
    frame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    frame->setLineWidth(1);

    m_balanceButton = new QPushButton("Get ETH Balance", this);
    m_balanceAddressInput = new QLineEdit(this);
    m_balanceAddressInput->setPlaceholderText("Enter ETH Balance Address...");

    QLabel* addressLabel = new QLabel("ETH Balance Address:", this);
    m_balanceAddressOutput = new QLineEdit(this);
    m_balanceAddressOutput->setReadOnly(true);
    m_balanceAddressOutput->setPlaceholderText("Address will appear here...");

    QLabel* valueLabel = new QLabel("ETH Balance:", this);
    m_balanceValueOutput = new QLineEdit(this);
    m_balanceValueOutput->setReadOnly(true);
    m_balanceValueOutput->setPlaceholderText("Balance will appear here...");

    layout->addWidget(m_balanceAddressInput);
    layout->addWidget(m_balanceButton);
    layout->addWidget(addressLabel);
    layout->addWidget(m_balanceAddressOutput);
    layout->addWidget(valueLabel);
    layout->addWidget(m_balanceValueOutput);

    m_mainLayout->addWidget(frame);

    connect(m_balanceButton, &QPushButton::clicked, this, &EthClientTab::onEthBalanceButtonClicked);
}

QFrame* EthClientTab::createSeparator() {
    QFrame* separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setMaximumHeight(2);
    return separator;
}

void EthClientTab::initializeDefaultClients() {
    emit statusChanged("Initializing default EthClients...");

    bool success = m_logos->wallet_module.ethClientInit("https://ethereum-rpc.publicnode.com");
    success &= m_logos->wallet_module.ethClientInit("https://arbitrum-one-rpc.publicnode.com");
    success &= m_logos->wallet_module.ethClientInit("https://optimism-rpc.publicnode.com");
    success &= m_logos->wallet_module.ethClientInit("https://ethereum-sepolia-rpc.publicnode.com");
    success &= m_logos->wallet_module.ethClientInit("https://arbitrum-sepolia-rpc.publicnode.com");
    success &= m_logos->wallet_module.ethClientInit("https://optimism-sepolia-rpc.publicnode.com");
    success &= m_logos->wallet_module.ethClientInit("https://public.sepolia.rpc.status.network");

    if (!success) {
        emit statusChanged("Error: Failed to initialize default EthClients");
        return;
    }

    refreshEthClientList();
    updateUIState();
    emit statusChanged("Default EthClients initialized");
}

QString EthClientTab::getSelectedRpcUrl() {
    int index = m_clientSelector->currentIndex();
    if (index > 0) {
        return m_clientSelector->itemData(index).toString();
    }
    return "";
}

void EthClientTab::refreshEthClientList() {
    // Get the list of initialized clients from the module
    QStringList clients = m_logos->wallet_module.ethClientGetClients();

    // Update our local list
    m_initializedClients = clients;

    // Store current selection
    QString currentSelection = getSelectedRpcUrl();

    // Update the dropdown
    m_clientSelector->clear();
    m_clientSelector->addItem("-- No EthClient Selected --", "");

    for (const QString& rpcUrl : m_initializedClients) {
        m_clientSelector->addItem(rpcUrl, rpcUrl);
    }

    // Restore selection if it still exists
    if (!currentSelection.isEmpty()) {
        int index = m_clientSelector->findData(currentSelection);
        if (index >= 0) {
            m_clientSelector->setCurrentIndex(index);
        } else {
            // Selection was removed, reset to no selection
            m_clientSelector->setCurrentIndex(0);
        }
    } else {
        // No previous selection, ensure index 0
        m_clientSelector->setCurrentIndex(0);
    }

    // Explicitly update UI state after restoring selection
    updateUIState();
}

void EthClientTab::onEthClientSelectionChanged(int /*index*/) {
    // Reset results when selection changes
    resetResults();

    // Update UI state based on selection
    updateUIState();
}

void EthClientTab::updateUIState() {
    int currentIndex = m_clientSelector->currentIndex();
    bool hasSelection = (currentIndex > 0);

    // Enable/disable operation buttons and inputs
    m_rpcCallButton->setEnabled(hasSelection);
    m_rpcCallMethodInput->setEnabled(hasSelection);
    m_rpcCallParamsInput->setEnabled(hasSelection);

    m_chainIDButton->setEnabled(hasSelection);

    m_balanceButton->setEnabled(hasSelection);
    m_balanceAddressInput->setEnabled(hasSelection);

    m_closeButton->setEnabled(hasSelection);
}

void EthClientTab::resetResults() {
    // Reset all result outputs to empty/default state
    m_rpcCallResultOutput->clear();
    m_chainIDOutput->clear();
    m_balanceAddressOutput->clear();
    m_balanceValueOutput->clear();
}

bool EthClientTab::isClientAlreadyInitialized(const QString& rpcUrl) {
    return m_initializedClients.contains(rpcUrl);
}

void EthClientTab::onInitEthClientButtonClicked() {
    QString rpcUrl = m_rpcUrlInput->text().trimmed();
    if (rpcUrl.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter an RPC URL");
        return;
    }

    // Check for duplicate initialization (PR review suggestion)
    if (isClientAlreadyInitialized(rpcUrl)) {
        QMessageBox::information(this, "Already Initialized",
            "This EthClient is already initialized. Please select it from the dropdown.");
        // Select the existing client
        int index = m_clientSelector->findData(rpcUrl);
        if (index >= 0) {
            m_clientSelector->setCurrentIndex(index);
        }
        m_rpcUrlInput->clear();
        return;
    }

    emit statusChanged("Initializing EthClient...");

    // Initialize ethclient using ethClientInit
    bool success = m_logos->wallet_module.ethClientInit(rpcUrl);

    if (success) {
        refreshEthClientList();
        // Select the newly initialized ethclient
        int index = m_clientSelector->findData(rpcUrl);
        if (index >= 0) {
            m_clientSelector->setCurrentIndex(index);
        }
        m_rpcUrlInput->clear();
        emit statusChanged("EthClient initialized: " + rpcUrl);
        QMessageBox::information(this, "Success", "EthClient initialized successfully");
    } else {
        emit statusChanged("Failed to initialize EthClient");
        QMessageBox::warning(this, "Error", "Failed to initialize EthClient. Please check the RPC URL.");
    }
}

void EthClientTab::onRefreshEthClientsButtonClicked() {
    refreshEthClientList();
    emit statusChanged("EthClient list refreshed");
}

void EthClientTab::onCloseEthClientButtonClicked() {
    QString rpcUrl = getSelectedRpcUrl();
    if (rpcUrl.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please select an EthClient to close");
        return;
    }

    emit statusChanged("Closing EthClient...");

    // Close ethclient using ethClientClose
    bool success = m_logos->wallet_module.ethClientClose(rpcUrl);

    if (success) {
        refreshEthClientList();
        emit statusChanged("EthClient closed: " + rpcUrl);
        QMessageBox::information(this, "Success", "EthClient closed");
    } else {
        emit statusChanged("Failed to close EthClient");
        QMessageBox::warning(this, "Error", "Failed to close EthClient");
    }
}

void EthClientTab::onRpcCallButtonClicked() {
    QString rpcUrl = getSelectedRpcUrl();
    if (rpcUrl.isEmpty()) {
        return; // Should not happen as button is disabled, but check anyway
    }

    m_rpcCallResultOutput->setPlainText("Calling...");
    QString method = m_rpcCallMethodInput->text();
    QString paramsString = m_rpcCallParamsInput->text();
    QString result = m_logos->wallet_module.ethClientRpcCall(rpcUrl, method, paramsString);
    m_rpcCallResultOutput->setPlainText(result);
}

void EthClientTab::onChainIDButtonClicked() {
    QString rpcUrl = getSelectedRpcUrl();
    if (rpcUrl.isEmpty()) {
        return; // Should not happen as button is disabled, but check anyway
    }

    m_chainIDOutput->setText("Getting Chain ID...");
    QString chainID = m_logos->wallet_module.ethClientChainId(rpcUrl);
    m_chainIDOutput->setText(chainID);
}

void EthClientTab::onEthBalanceButtonClicked() {
    QString rpcUrl = getSelectedRpcUrl();
    if (rpcUrl.isEmpty()) {
        return; // Should not happen as button is disabled, but check anyway
    }

    QString address = m_balanceAddressInput->text();
    m_balanceAddressOutput->setText(address);
    m_balanceValueOutput->setText("Getting ETH Balance...");
    QString ethBalance = m_logos->wallet_module.ethClientGetBalance(rpcUrl, address);
    m_balanceValueOutput->setText(ethBalance);
}
