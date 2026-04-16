#include "WalletBackend.h"
#include <QDebug>

WalletBackend::WalletBackend(LogosAPI* logosAPI, QObject* parent)
    : WalletBackendSimpleSource(parent),
      m_logosAPI(logosAPI),
      m_logos(nullptr)
{
    setStatus(NotInitialized);
    setSelectedClientIndex(-1);

    if (!m_logosAPI) {
        m_logosAPI = new LogosAPI("core", this);
    }
    m_logos = new LogosModules(m_logosAPI);

    initializeDefaultClients();
}

void WalletBackend::setStatusMessageAndLog(const QString& message) {
    qDebug() << message;
    setStatusMessage(message);
}

void WalletBackend::initializeDefaultClients() {
    setStatus(Initializing);
    setStatusMessageAndLog("Initializing default EthClients...");

    bool success = m_logos->wallet_module.ethClientInit("https://ethereum-rpc.publicnode.com");
    success &= m_logos->wallet_module.ethClientInit("https://arbitrum-one-rpc.publicnode.com");
    success &= m_logos->wallet_module.ethClientInit("https://optimism-rpc.publicnode.com");
    success &= m_logos->wallet_module.ethClientInit("https://ethereum-sepolia-rpc.publicnode.com");
    success &= m_logos->wallet_module.ethClientInit("https://arbitrum-sepolia-rpc.publicnode.com");
    success &= m_logos->wallet_module.ethClientInit("https://optimism-sepolia-rpc.publicnode.com");
    success &= m_logos->wallet_module.ethClientInit("https://public.sepolia.rpc.status.network");

    if (!success) {
        setStatus(Error);
        setStatusMessageAndLog("Error: Failed to initialize default EthClients");
        return;
    }

    refreshClients();
    setStatus(Ready);
    setStatusMessageAndLog("Default EthClients initialized");
}

void WalletBackend::refreshClients() {
    QStringList list = m_logos->wallet_module.ethClientGetClients();
    if (clients() != list) {
        setClients(list);
    }
    setStatusMessageAndLog("EthClient list refreshed");
}

void WalletBackend::initEthClient(QString url) {
    if (url.trimmed().isEmpty()) {
        setStatusMessageAndLog("Error: Please enter an RPC URL");
        return;
    }

    if (clients().contains(url.trimmed())) {
        setStatusMessageAndLog("EthClient already initialized: " + url.trimmed());
        return;
    }

    setStatusMessageAndLog("Initializing EthClient...");
    bool success = m_logos->wallet_module.ethClientInit(url.trimmed());

    if (success) {
        refreshClients();
        // Select the newly initialized client
        int index = clients().indexOf(url.trimmed());
        if (index >= 0) {
            setSelectedClientIndex(index);
        }
        setStatusMessageAndLog("EthClient initialized: " + url.trimmed());
    } else {
        setStatusMessageAndLog("Failed to initialize EthClient");
    }
}

void WalletBackend::closeEthClient(QString url) {
    if (url.isEmpty()) {
        setStatusMessageAndLog("Error: No EthClient selected");
        return;
    }

    setStatusMessageAndLog("Closing EthClient...");
    bool success = m_logos->wallet_module.ethClientClose(url);

    if (success) {
        refreshClients();
        setSelectedClientIndex(-1);
        setStatusMessageAndLog("EthClient closed: " + url);
    } else {
        setStatusMessageAndLog("Failed to close EthClient");
    }
}

QString WalletBackend::rpcCall(QString url, QString method, QString params) {
    return m_logos->wallet_module.ethClientRpcCall(url, method, params);
}

QString WalletBackend::getChainId(QString url) {
    return m_logos->wallet_module.ethClientChainId(url);
}

QString WalletBackend::getBalance(QString url, QString address) {
    return m_logos->wallet_module.ethClientGetBalance(url, address);
}

QString WalletBackend::generateTransaction(QString txType, QString paramsJSON) {
    setStatusMessageAndLog("Generating transaction...");
    QString result;

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
    } else {
        setStatusMessageAndLog("Error: Unknown transaction type: " + txType);
        return "";
    }

    setStatusMessageAndLog("Transaction generated");
    return result;
}

QString WalletBackend::jsonToRlp(QString json) {
    setStatusMessageAndLog("Converting JSON to RLP...");
    QString result = m_logos->wallet_module.transactionJsonToRlp(json);
    setStatusMessageAndLog("JSON converted to RLP");
    return result;
}

QString WalletBackend::rlpToJson(QString rlp) {
    setStatusMessageAndLog("Converting RLP to JSON...");
    QString result = m_logos->wallet_module.transactionRlpToJson(rlp);
    setStatusMessageAndLog("RLP converted to JSON");
    return result;
}

QString WalletBackend::getHash(QString json) {
    setStatusMessageAndLog("Getting transaction hash...");
    QString result = m_logos->wallet_module.transactionGetHash(json);
    setStatusMessageAndLog("Transaction hash retrieved");
    return result;
}
