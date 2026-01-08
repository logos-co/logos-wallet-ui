#include "TransactionsTab.h"
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

TransactionsTab::TransactionsTab(LogosModules* logos, QWidget* parent)
    : QWidget(parent), m_logos(logos) {
    setupUI();
}

void TransactionsTab::setupUI() {
    // Create scroll area for the entire tab
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    QWidget* scrollContent = new QWidget();
    QVBoxLayout* scrollLayout = new QVBoxLayout(scrollContent);
    scrollLayout->setSpacing(10);
    scrollLayout->setContentsMargins(10, 10, 10, 10);

    setupTransactionGenerator(scrollLayout);
    scrollLayout->addWidget(createSeparator());
    setupTransactionUtilities(scrollLayout);

    scrollArea->setWidget(scrollContent);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(scrollArea);
}

void TransactionsTab::setupTransactionGenerator(QVBoxLayout* parentLayout) {
    QFrame* frame = new QFrame(this);
    QVBoxLayout* layout = new QVBoxLayout(frame);
    frame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    frame->setLineWidth(1);

    QLabel* title = new QLabel("<b>🔷 Transaction Generator</b>", this);
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    QLabel* info = new QLabel(
        "Select a transaction type, choose fee type (Legacy or EIP-1559), "
        "fill in the required parameters, and click \"Generate Transaction\" "
        "to get the transaction in JSON format.", this);
    info->setWordWrap(true);
    info->setStyleSheet("background-color: #d1ecf1; color: #0c5460; padding: 10px; border-radius: 5px;");
    layout->addWidget(info);

    // Transaction type selector
    QLabel* txTypeLabel = new QLabel("Transaction Type:", this);
    layout->addWidget(txTypeLabel);
    m_txTypeComboBox = new QComboBox(this);
    m_txTypeComboBox->addItem("-- Select Transaction Type --", "");
    m_txTypeComboBox->addItem("Transfer ETH", "transferETH");
    m_txTypeComboBox->addItem("Transfer ERC20", "transferERC20");
    m_txTypeComboBox->addItem("Approve ERC20", "approveERC20");
    m_txTypeComboBox->addItem("Transfer ERC721 (transferFrom)", "transferFromERC721");
    m_txTypeComboBox->addItem("Transfer ERC721 (safeTransferFrom)", "safeTransferFromERC721");
    m_txTypeComboBox->addItem("Approve ERC721", "approveERC721");
    m_txTypeComboBox->addItem("Set Approval For All ERC721", "setApprovalForAllERC721");
    m_txTypeComboBox->addItem("Transfer ERC1155", "transferERC1155");
    m_txTypeComboBox->addItem("Batch Transfer ERC1155", "batchTransferERC1155");
    m_txTypeComboBox->addItem("Set Approval For All ERC1155", "setApprovalForAllERC1155");
    layout->addWidget(m_txTypeComboBox);

    // Fee type selector
    QLabel* feeTypeLabel = new QLabel("Fee Type:", this);
    layout->addWidget(feeTypeLabel);
    m_feeTypeButtonGroup = new QButtonGroup(this);
    m_feeLegacyRadio = new QRadioButton("Legacy (GasPrice)", this);
    m_feeEIP1559Radio = new QRadioButton("EIP-1559 (MaxFeePerGas + MaxPriorityFeePerGas)", this);
    m_feeLegacyRadio->setChecked(true);
    m_feeTypeButtonGroup->addButton(m_feeLegacyRadio, 0);
    m_feeTypeButtonGroup->addButton(m_feeEIP1559Radio, 1);
    QHBoxLayout* feeTypeLayout = new QHBoxLayout();
    feeTypeLayout->addWidget(m_feeLegacyRadio);
    feeTypeLayout->addWidget(m_feeEIP1559Radio);
    layout->addLayout(feeTypeLayout);

    // Common fields
    QLabel* nonceLabel = new QLabel("Nonce:", this);
    layout->addWidget(nonceLabel);
    m_txNonceInput = new QLineEdit(this);
    m_txNonceInput->setPlaceholderText("0");
    m_txNonceInput->setText("0");
    layout->addWidget(m_txNonceInput);

    QLabel* gasLimitLabel = new QLabel("Gas Limit:", this);
    layout->addWidget(gasLimitLabel);
    m_txGasLimitInput = new QLineEdit(this);
    m_txGasLimitInput->setPlaceholderText("21000");
    m_txGasLimitInput->setText("21000");
    layout->addWidget(m_txGasLimitInput);

    QLabel* chainIDLabel = new QLabel("Chain ID:", this);
    layout->addWidget(chainIDLabel);
    m_txChainIDInput = new QLineEdit(this);
    m_txChainIDInput->setPlaceholderText("1");
    m_txChainIDInput->setText("1");
    layout->addWidget(m_txChainIDInput);

    // Legacy fee group
    m_legacyFeeGroup = new QGroupBox("Legacy Fee", this);
    QVBoxLayout* legacyFeeLayout = new QVBoxLayout(m_legacyFeeGroup);
    QLabel* gasPriceLabel = new QLabel("Gas Price (wei):", this);
    legacyFeeLayout->addWidget(gasPriceLabel);
    m_txGasPriceInput = new QLineEdit(this);
    m_txGasPriceInput->setPlaceholderText("20000000000");
    m_txGasPriceInput->setText("20000000000");
    legacyFeeLayout->addWidget(m_txGasPriceInput);
    layout->addWidget(m_legacyFeeGroup);

    // EIP-1559 fee group
    m_eip1559FeeGroup = new QGroupBox("EIP-1559 Fee", this);
    QVBoxLayout* eip1559FeeLayout = new QVBoxLayout(m_eip1559FeeGroup);
    QLabel* maxFeePerGasLabel = new QLabel("Max Fee Per Gas (wei):", this);
    eip1559FeeLayout->addWidget(maxFeePerGasLabel);
    m_txMaxFeePerGasInput = new QLineEdit(this);
    m_txMaxFeePerGasInput->setPlaceholderText("30000000000");
    m_txMaxFeePerGasInput->setText("30000000000");
    eip1559FeeLayout->addWidget(m_txMaxFeePerGasInput);
    QLabel* maxPriorityFeePerGasLabel = new QLabel("Max Priority Fee Per Gas (wei):", this);
    eip1559FeeLayout->addWidget(maxPriorityFeePerGasLabel);
    m_txMaxPriorityFeePerGasInput = new QLineEdit(this);
    m_txMaxPriorityFeePerGasInput->setPlaceholderText("2000000000");
    m_txMaxPriorityFeePerGasInput->setText("2000000000");
    eip1559FeeLayout->addWidget(m_txMaxPriorityFeePerGasInput);
    m_eip1559FeeGroup->setVisible(false);
    layout->addWidget(m_eip1559FeeGroup);

    // Dynamic parameter fields container
    QLabel* paramsLabel = new QLabel("Transaction Parameters:", this);
    layout->addWidget(paramsLabel);
    m_txParamsContainer = new QWidget(this);
    m_txParamsLayout = new QVBoxLayout(m_txParamsContainer);
    layout->addWidget(m_txParamsContainer);

    // Generate button
    m_txGenerateButton = new QPushButton("Generate Transaction", this);
    layout->addWidget(m_txGenerateButton);

    // Result output
    QLabel* resultLabel = new QLabel("Generated Transaction:", this);
    layout->addWidget(resultLabel);
    m_txResultOutput = new QTextEdit(this);
    m_txResultOutput->setReadOnly(true);
    m_txResultOutput->setPlaceholderText("Transaction JSON will appear here...");
    m_txResultOutput->setMaximumHeight(200);
    layout->addWidget(m_txResultOutput);

    parentLayout->addWidget(frame);

    // Connect signals
    connect(m_txTypeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TransactionsTab::onTxTypeChanged);
    connect(m_feeTypeButtonGroup, &QButtonGroup::idClicked,
            this, &TransactionsTab::onFeeTypeChanged);
    connect(m_txGenerateButton, &QPushButton::clicked,
            this, &TransactionsTab::onGenerateTransactionButtonClicked);
}

void TransactionsTab::setupTransactionUtilities(QVBoxLayout* parentLayout) {
    // Transaction JSON to RLP
    QFrame* jsonToRlpFrame = new QFrame(this);
    QVBoxLayout* jsonToRlpLayout = new QVBoxLayout(jsonToRlpFrame);
    jsonToRlpFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    jsonToRlpFrame->setLineWidth(1);

    QLabel* jsonToRlpTitle = new QLabel("<b>Transaction JSON to RLP</b>", this);
    jsonToRlpLayout->addWidget(jsonToRlpTitle);

    QLabel* jsonToRlpInputLabel = new QLabel("Transaction JSON:", this);
    jsonToRlpLayout->addWidget(jsonToRlpInputLabel);
    m_txJsonToRlpInput = new QTextEdit(this);
    m_txJsonToRlpInput->setPlaceholderText("{\"nonce\": \"0x...\", \"gasPrice\": \"0x...\", ...}");
    m_txJsonToRlpInput->setMaximumHeight(80);
    jsonToRlpLayout->addWidget(m_txJsonToRlpInput);

    m_txJsonToRlpButton = new QPushButton("Convert JSON to RLP", this);
    jsonToRlpLayout->addWidget(m_txJsonToRlpButton);

    QLabel* jsonToRlpResultLabel = new QLabel("RLP Result:", this);
    jsonToRlpLayout->addWidget(jsonToRlpResultLabel);
    m_txJsonToRlpResult = new QTextEdit(this);
    m_txJsonToRlpResult->setReadOnly(true);
    m_txJsonToRlpResult->setPlaceholderText("RLP hex will appear here...");
    m_txJsonToRlpResult->setMaximumHeight(100);
    jsonToRlpLayout->addWidget(m_txJsonToRlpResult);

    parentLayout->addWidget(jsonToRlpFrame);
    parentLayout->addWidget(createSeparator());

    // Transaction RLP to JSON
    QFrame* rlpToJsonFrame = new QFrame(this);
    QVBoxLayout* rlpToJsonLayout = new QVBoxLayout(rlpToJsonFrame);
    rlpToJsonFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    rlpToJsonFrame->setLineWidth(1);

    QLabel* rlpToJsonTitle = new QLabel("<b>Transaction RLP to JSON</b>", this);
    rlpToJsonLayout->addWidget(rlpToJsonTitle);

    QLabel* rlpToJsonInputLabel = new QLabel("RLP Hex:", this);
    rlpToJsonLayout->addWidget(rlpToJsonInputLabel);
    m_txRlpToJsonInput = new QLineEdit(this);
    m_txRlpToJsonInput->setPlaceholderText("0x...");
    rlpToJsonLayout->addWidget(m_txRlpToJsonInput);

    m_txRlpToJsonButton = new QPushButton("Convert RLP to JSON", this);
    rlpToJsonLayout->addWidget(m_txRlpToJsonButton);

    QLabel* rlpToJsonResultLabel = new QLabel("JSON Result:", this);
    rlpToJsonLayout->addWidget(rlpToJsonResultLabel);
    m_txRlpToJsonResult = new QTextEdit(this);
    m_txRlpToJsonResult->setReadOnly(true);
    m_txRlpToJsonResult->setPlaceholderText("Transaction JSON will appear here...");
    m_txRlpToJsonResult->setMaximumHeight(100);
    rlpToJsonLayout->addWidget(m_txRlpToJsonResult);

    parentLayout->addWidget(rlpToJsonFrame);
    parentLayout->addWidget(createSeparator());

    // Get Transaction Hash
    QFrame* getHashFrame = new QFrame(this);
    QVBoxLayout* getHashLayout = new QVBoxLayout(getHashFrame);
    getHashFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    getHashFrame->setLineWidth(1);

    QLabel* getHashTitle = new QLabel("<b>Get Transaction Hash</b>", this);
    getHashLayout->addWidget(getHashTitle);

    QLabel* getHashInputLabel = new QLabel("Transaction JSON:", this);
    getHashLayout->addWidget(getHashInputLabel);
    m_txGetHashInput = new QTextEdit(this);
    m_txGetHashInput->setPlaceholderText("{\"nonce\": \"0x...\", \"gasPrice\": \"0x...\", ...}");
    m_txGetHashInput->setMaximumHeight(80);
    getHashLayout->addWidget(m_txGetHashInput);

    m_txGetHashButton = new QPushButton("Get Transaction Hash", this);
    getHashLayout->addWidget(m_txGetHashButton);

    QLabel* getHashResultLabel = new QLabel("Hash Result:", this);
    getHashLayout->addWidget(getHashResultLabel);
    m_txGetHashResult = new QLineEdit(this);
    m_txGetHashResult->setReadOnly(true);
    m_txGetHashResult->setPlaceholderText("Transaction hash will appear here...");
    getHashLayout->addWidget(m_txGetHashResult);

    parentLayout->addWidget(getHashFrame);

    // Connect utility button signals
    connect(m_txJsonToRlpButton, &QPushButton::clicked,
            this, &TransactionsTab::onTransactionJsonToRlpButtonClicked);
    connect(m_txRlpToJsonButton, &QPushButton::clicked,
            this, &TransactionsTab::onTransactionRlpToJsonButtonClicked);
    connect(m_txGetHashButton, &QPushButton::clicked,
            this, &TransactionsTab::onTransactionGetHashButtonClicked);
}

QFrame* TransactionsTab::createSeparator() {
    QFrame* separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setMaximumHeight(2);
    return separator;
}

void TransactionsTab::onTxTypeChanged(int index) {
    if (index > 0) {
        QString txType = m_txTypeComboBox->itemData(index).toString();
        updateTxParamsFields(txType);
    } else {
        // Clear params fields
        updateTxParamsFields("");
    }
}

void TransactionsTab::onFeeTypeChanged(int id) {
    if (id == 0) { // Legacy
        m_legacyFeeGroup->setVisible(true);
        m_eip1559FeeGroup->setVisible(false);
    } else { // EIP-1559
        m_legacyFeeGroup->setVisible(false);
        m_eip1559FeeGroup->setVisible(true);
    }
}

void TransactionsTab::onGenerateTransactionButtonClicked() {
    int txTypeIndex = m_txTypeComboBox->currentIndex();
    if (txTypeIndex == 0) {
        QMessageBox::warning(this, "Error", "Please select a transaction type");
        return;
    }

    QString paramsJSON = buildTxParamsJSON();
    if (paramsJSON.isEmpty()) {
        QMessageBox::warning(this, "Error", "Failed to build transaction parameters");
        return;
    }

    QString txType = m_txTypeComboBox->itemData(txTypeIndex).toString();
    QString result;

    emit statusChanged("Generating transaction...");

    if (txType == "transferETH") {
        result = m_logos->wallet_module.txGeneratorTransferETH(paramsJSON);
    } else if (txType == "transferERC20") {
        result = m_logos->wallet_module.txGeneratorTransferERC20(paramsJSON);
    } else if (txType == "approveERC20") {
        result = m_logos->wallet_module.txGeneratorApproveERC20(paramsJSON);
    } else if (txType == "transferFromERC721") {
        result = m_logos->wallet_module.txGeneratorTransferFromERC721(paramsJSON);
    } else if (txType == "safeTransferFromERC721") {
        result = m_logos->wallet_module.txGeneratorSafeTransferFromERC721(paramsJSON);
    } else if (txType == "approveERC721") {
        result = m_logos->wallet_module.txGeneratorApproveERC721(paramsJSON);
    } else if (txType == "setApprovalForAllERC721") {
        result = m_logos->wallet_module.txGeneratorSetApprovalForAllERC721(paramsJSON);
    } else if (txType == "transferERC1155") {
        result = m_logos->wallet_module.txGeneratorTransferERC1155(paramsJSON);
    } else if (txType == "batchTransferERC1155") {
        result = m_logos->wallet_module.txGeneratorBatchTransferERC1155(paramsJSON);
    } else if (txType == "setApprovalForAllERC1155") {
        result = m_logos->wallet_module.txGeneratorSetApprovalForAllERC1155(paramsJSON);
    }

    m_txResultOutput->setPlainText(result);
    emit statusChanged("Transaction generated");
}

void TransactionsTab::onTransactionJsonToRlpButtonClicked() {
    QString txJSON = m_txJsonToRlpInput->toPlainText();
    if (txJSON.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter transaction JSON");
        return;
    }
    emit statusChanged("Converting transaction JSON to RLP...");
    QString result = m_logos->wallet_module.transactionJsonToRlp(txJSON);
    m_txJsonToRlpResult->setPlainText(result);
    emit statusChanged("Transaction JSON converted to RLP");
}

void TransactionsTab::onTransactionRlpToJsonButtonClicked() {
    QString rlpHex = m_txRlpToJsonInput->text();
    if (rlpHex.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter RLP hex");
        return;
    }
    emit statusChanged("Converting RLP to transaction JSON...");
    QString result = m_logos->wallet_module.transactionRlpToJson(rlpHex);
    m_txRlpToJsonResult->setPlainText(result);
    emit statusChanged("RLP converted to transaction JSON");
}

void TransactionsTab::onTransactionGetHashButtonClicked() {
    QString txJSON = m_txGetHashInput->toPlainText();
    if (txJSON.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter transaction JSON");
        return;
    }
    emit statusChanged("Getting transaction hash...");
    QString result = m_logos->wallet_module.transactionGetHash(txJSON);
    m_txGetHashResult->setText(result);
    emit statusChanged("Transaction hash retrieved");
}

void TransactionsTab::updateTxParamsFields(const QString& txType) {
    // Clear existing parameter fields (both labels and inputs)
    for (QWidget* widget : m_txParamWidgets) {
        m_txParamsLayout->removeWidget(widget);
        delete widget;
    }
    m_txParamWidgets.clear();
    m_txParamInputs.clear();

    if (txType.isEmpty()) {
        return;
    }

    // Define parameters for each transaction type
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
        QLabel* label = new QLabel(param.first + ":", this);
        m_txParamsLayout->addWidget(label);
        m_txParamWidgets.append(label);

        QLineEdit* input = new QLineEdit(this);
        input->setPlaceholderText(param.second);
        m_txParamsLayout->addWidget(input);
        m_txParamWidgets.append(input);
        m_txParamInputs[param.first] = input;
    }
}

QString TransactionsTab::buildTxParamsJSON() {
    QJsonObject paramsObj;

    // Add common transaction fields
    QString nonce = m_txNonceInput->text();
    QString gasLimit = m_txGasLimitInput->text();
    QString chainID = m_txChainIDInput->text();

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
    if (!chainID.isEmpty()) {
        paramsObj["chainID"] = chainID;
    }

    // Add fee fields based on selected type
    bool useEIP1559 = m_feeEIP1559Radio->isChecked();
    if (useEIP1559) {
        QString maxFeePerGas = m_txMaxFeePerGasInput->text();
        QString maxPriorityFeePerGas = m_txMaxPriorityFeePerGasInput->text();
        if (!maxFeePerGas.isEmpty()) {
            paramsObj["maxFeePerGas"] = maxFeePerGas;
        }
        if (!maxPriorityFeePerGas.isEmpty()) {
            paramsObj["maxPriorityFeePerGas"] = maxPriorityFeePerGas;
        }
    } else {
        QString gasPrice = m_txGasPriceInput->text();
        if (!gasPrice.isEmpty()) {
            paramsObj["gasPrice"] = gasPrice;
        }
    }

    // Add dynamic parameters
    QMapIterator<QString, QLineEdit*> it(m_txParamInputs);
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
                    array.append(item.trimmed());
                }
                paramsObj[key] = array;
            } else if (key == "to" || key == "from" || key == "tokenAddress" ||
                       key == "spender" || key == "operator") {
                // Addresses - keep as hex strings (with 0x prefix)
                paramsObj[key] = value;
            } else {
                // Value, Amount, TokenID - keep as decimal strings
                paramsObj[key] = value;
            }
        }
    }

    QJsonDocument doc(paramsObj);
    return doc.toJson(QJsonDocument::Compact);
}
