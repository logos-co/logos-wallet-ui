#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include "logos_api.h"
#include "logos_api_client.h"
#include "logos_sdk.h"
#include "rep_WalletBackend_source.h"

// WalletBackend is our implementation of the WalletBackend QtRO source
// declared in WalletBackend.rep. Inherits from the generated
// WalletBackendSimpleSource so we get a real metaobject + property setters.
class WalletBackend : public WalletBackendSimpleSource {
    Q_OBJECT

public:
    explicit WalletBackend(LogosAPI* logosAPI = nullptr, QObject* parent = nullptr);
    ~WalletBackend() override = default;

public slots:
    // Overrides of the pure-virtual slots generated from the .rep.
    void initEthClient(QString url) override;
    void closeEthClient(QString url) override;
    void refreshClients() override;
    void initializeDefaultClients() override;
    QString rpcCall(QString url, QString method, QString params) override;
    QString getChainId(QString url) override;
    QString getBalance(QString url, QString address) override;
    QString generateTransaction(QString txType, QString paramsJSON) override;
    QString jsonToRlp(QString json) override;
    QString rlpToJson(QString rlp) override;
    QString getHash(QString json) override;

private:
    void setStatusMessageAndLog(const QString& message);

    LogosAPI* m_logosAPI;
    LogosModules* m_logos;
};
