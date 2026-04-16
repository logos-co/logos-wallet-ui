#ifndef WALLET_UI_PLUGIN_H
#define WALLET_UI_PLUGIN_H

#include <QObject>
#include <QString>
#include <QtPlugin>          // for Q_PLUGIN_METADATA, Q_INTERFACES
#include "WalletUiPluginInterface.h"
#include "LogosViewPluginBase.h"

class LogosAPI;
class WalletBackend;

// Thin plugin entry point. Holds a WalletBackend and lets the
// generated view-plugin base expose it to ui-host.
class WalletUiPlugin : public QObject,
                       public WalletUiPluginInterface,
                       public WalletBackendViewPluginBase
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID WalletUiPluginInterface_iid FILE "../metadata.json")
    Q_INTERFACES(WalletUiPluginInterface)

public:
    explicit WalletUiPlugin(QObject* parent = nullptr);
    ~WalletUiPlugin() override;

    QString name()    const override { return "wallet_ui"; }
    QString version() const override { return "1.0.0"; }

    // Called by ui-host after plugin load. Creates the backend and wires
    // it up with the provided LogosAPI.
    Q_INVOKABLE void initLogos(LogosAPI* api);

private:
    WalletBackend* m_backend = nullptr;
};

#endif
