#include "WalletBackend.h"
#include <QDebug>

WalletBackend::WalletBackend(LogosAPI* logosAPI, QObject* parent)
    : QObject(parent),
      m_logosAPI(nullptr),
      m_logos(nullptr),
      m_status(NotInitialized),
      m_selectedClientIndex(-1)
{
    if (logosAPI) {
        m_logosAPI = logosAPI;
    } else {
        m_logosAPI = new LogosAPI("core", this);
    }
    m_logos = new LogosModules(m_logosAPI);

    initializeDefaultClients();
}

void WalletBackend::setStatus(WalletStatus status) {
    if (m_status != status) {
        m_status = status;
        emit statusChanged();
    }
}

void WalletBackend::setStatusMessage(const QString& message) {
    if (m_statusMessage != message) {
        m_statusMessage = message;
        emit statusMessageChanged();
        qDebug() << message;
    }
}

void WalletBackend::setSelectedClientIndex(int index) {
    if (m_selectedClientIndex != index) {
        m_selectedClientIndex = index;
        emit selectedClientIndexChanged();
    }
}

void WalletBackend::initializeDefaultClients() {
    setStatus(Initializing);
    setStatusMessage("Initializing default EthClients...");

    bool success = m_logos->wallet_module.ethClientInit("https://ethereum-rpc.publicnode.com");
    success &= m_logos->wallet_module.ethClientInit("https://arbitrum-one-rpc.publicnode.com");
    success &= m_logos->wallet_module.ethClientInit("https://optimism-rpc.publicnode.com");
    success &= m_logos->wallet_module.ethClientInit("https://ethereum-sepolia-rpc.publicnode.com");
    success &= m_logos->wallet_module.ethClientInit("https://arbitrum-sepolia-rpc.publicnode.com");
    success &= m_logos->wallet_module.ethClientInit("https://optimism-sepolia-rpc.publicnode.com");
    success &= m_logos->wallet_module.ethClientInit("https://public.sepolia.rpc.status.network");

    if (!success) {
        setStatus(Error);
        setStatusMessage("Error: Failed to initialize default EthClients");
        return;
    }

    refreshClients();
    setStatus(Ready);
    setStatusMessage("Default EthClients initialized");
}

void WalletBackend::refreshClients() {
    QStringList clients = m_logos->wallet_module.ethClientGetClients();
    if (m_clients != clients) {
        m_clients = clients;
        emit clientsChanged();
    }
    setStatusMessage("EthClient list refreshed");
}

void WalletBackend::initEthClient(const QString& url) {
    if (url.trimmed().isEmpty()) {
        setStatusMessage("Error: Please enter an RPC URL");
        return;
    }

    if (m_clients.contains(url.trimmed())) {
        setStatusMessage("EthClient already initialized: " + url.trimmed());
        return;
    }

    setStatusMessage("Initializing EthClient...");
    bool success = m_logos->wallet_module.ethClientInit(url.trimmed());

    if (success) {
        refreshClients();
        // Select the newly initialized client
        int index = m_clients.indexOf(url.trimmed());
        if (index >= 0) {
            setSelectedClientIndex(index);
        }
        setStatusMessage("EthClient initialized: " + url.trimmed());
    } else {
        setStatusMessage("Failed to initialize EthClient");
    }
}

void WalletBackend::closeEthClient(const QString& url) {
    if (url.isEmpty()) {
        setStatusMessage("Error: No EthClient selected");
        return;
    }

    setStatusMessage("Closing EthClient...");
    bool success = m_logos->wallet_module.ethClientClose(url);

    if (success) {
        refreshClients();
        setSelectedClientIndex(-1);
        setStatusMessage("EthClient closed: " + url);
    } else {
        setStatusMessage("Failed to close EthClient");
    }
}

QString WalletBackend::rpcCall(const QString& url, const QString& method, const QString& params) {
    return m_logos->wallet_module.ethClientRpcCall(url, method, params);
}

QString WalletBackend::getChainId(const QString& url) {
    return m_logos->wallet_module.ethClientChainId(url);
}

QString WalletBackend::getBalance(const QString& url, const QString& address) {
    return m_logos->wallet_module.ethClientGetBalance(url, address);
}

QString WalletBackend::generateTransaction(const QString& txType, const QString& paramsJSON) {
    setStatusMessage("Generating transaction...");
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
        setStatusMessage("Error: Unknown transaction type: " + txType);
        return "";
    }

    setStatusMessage("Transaction generated");
    return result;
}

QString WalletBackend::jsonToRlp(const QString& json) {
    setStatusMessage("Converting JSON to RLP...");
    QString result = m_logos->wallet_module.transactionJsonToRlp(json);
    setStatusMessage("JSON converted to RLP");
    return result;
}

QString WalletBackend::rlpToJson(const QString& rlp) {
    setStatusMessage("Converting RLP to JSON...");
    QString result = m_logos->wallet_module.transactionRlpToJson(rlp);
    setStatusMessage("RLP converted to JSON");
    return result;
}

QString WalletBackend::getHash(const QString& json) {
    setStatusMessage("Getting transaction hash...");
    QString result = m_logos->wallet_module.transactionGetHash(json);
    setStatusMessage("Transaction hash retrieved");
    return result;
}
