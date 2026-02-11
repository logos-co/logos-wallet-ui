#pragma once

#include <QObject>
#include <QStringList>
#include "logos_api.h"
#include "logos_api_client.h"
#include "logos_sdk.h"

class WalletBackend : public QObject {
    Q_OBJECT

public:
    enum WalletStatus {
        NotInitialized = 0,
        Initializing,
        Ready,
        Error
    };
    Q_ENUM(WalletStatus)

    Q_PROPERTY(WalletStatus status READ status NOTIFY statusChanged)
    Q_PROPERTY(QStringList clients READ clients NOTIFY clientsChanged)
    Q_PROPERTY(int selectedClientIndex READ selectedClientIndex WRITE setSelectedClientIndex NOTIFY selectedClientIndexChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)

    explicit WalletBackend(LogosAPI* logosAPI = nullptr, QObject* parent = nullptr);
    ~WalletBackend() override = default;

    // Property getters
    WalletStatus status() const { return m_status; }
    QStringList clients() const { return m_clients; }
    int selectedClientIndex() const { return m_selectedClientIndex; }
    QString statusMessage() const { return m_statusMessage; }

    // Property setter
    void setSelectedClientIndex(int index);

    // EthClient methods
    Q_INVOKABLE void initEthClient(const QString& url);
    Q_INVOKABLE void closeEthClient(const QString& url);
    Q_INVOKABLE void refreshClients();
    Q_INVOKABLE void initializeDefaultClients();
    Q_INVOKABLE QString rpcCall(const QString& url, const QString& method, const QString& params);
    Q_INVOKABLE QString getChainId(const QString& url);
    Q_INVOKABLE QString getBalance(const QString& url, const QString& address);

    // Transaction methods
    Q_INVOKABLE QString generateTransaction(const QString& txType, const QString& paramsJSON);
    Q_INVOKABLE QString jsonToRlp(const QString& json);
    Q_INVOKABLE QString rlpToJson(const QString& rlp);
    Q_INVOKABLE QString getHash(const QString& json);

signals:
    void statusChanged();
    void clientsChanged();
    void selectedClientIndexChanged();
    void statusMessageChanged();

private:
    void setStatus(WalletStatus status);
    void setStatusMessage(const QString& message);

    LogosAPI* m_logosAPI;
    LogosModules* m_logos;
    WalletStatus m_status;
    QStringList m_clients;
    int m_selectedClientIndex;
    QString m_statusMessage;
};
