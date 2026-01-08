#include "WalletWidget.h"
#include <QDebug>
#include <QFrame>

// Static pointer to the active WalletWidget for callbacks
static WalletWidget* activeWidget = nullptr;

WalletWidget::WalletWidget(QWidget* parent)
    : QWidget(parent),
      m_logosAPI(nullptr),
      m_logos(nullptr),
      m_ethClientTab(nullptr),
      m_transactionsTab(nullptr) {

    // Set as the active widget
    activeWidget = this;

    m_logosAPI = new LogosAPI("core", this);
    m_logos = new LogosModules(m_logosAPI);

    // Main vertical layout
    m_mainLayout = new QVBoxLayout(this);

    // Create status frame
    QFrame* statusFrame = new QFrame(this);
    QVBoxLayout* statusLayout = new QVBoxLayout(statusFrame);
    statusFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    statusFrame->setLineWidth(1);

    // Create status label
    m_statusLabel = new QLabel("Status: Not initialized", this);
    m_statusLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // Add status label to layout
    statusLayout->addWidget(m_statusLabel);

    // Create tab widget
    m_tabWidget = new QTabWidget(this);

    // Create tab instances
    m_ethClientTab = new EthClientTab(m_logos, this);
    m_transactionsTab = new TransactionsTab(m_logos, this);

    // Connect status signals from tabs
    connect(m_ethClientTab, &EthClientTab::statusChanged,
            this, &WalletWidget::updateStatus);
    connect(m_transactionsTab, &TransactionsTab::statusChanged,
            this, &WalletWidget::updateStatus);

    // Add tabs to tab widget
    m_tabWidget->addTab(m_ethClientTab, "EthClient");
    m_tabWidget->addTab(m_transactionsTab, "Transactions");

    // Add status frame and tab widget to main layout
    m_mainLayout->addWidget(statusFrame);
    m_mainLayout->addWidget(m_tabWidget);

    // Set spacing and margins
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);

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

void WalletWidget::initWallet() {
    updateStatus("Status: Initializing Wallet...");

    // Initialize default ethClients via the EthClientTab
    m_ethClientTab->initializeDefaultClients();

    updateStatus("Status: Wallet initialized");
}

void WalletWidget::stopWallet() {
    updateStatus("Status: Stopping Wallet...");
    updateStatus("Status: Wallet stopped");
}

void WalletWidget::updateStatus(const QString& message) {
    m_statusLabel->setText(message);
    qDebug() << message;
}
