#ifndef WALLET_UI_PLUGIN_INTERFACE_H
#define WALLET_UI_PLUGIN_INTERFACE_H

#include <QtPlugin>          // for Q_DECLARE_INTERFACE
#include "interface.h"

// Marker interface used by Qt's plugin loader to identify the wallet UI
// plugin. The actual API surface (Q_INVOKABLE methods, properties, signals)
// lives in Wallet.rep — this header only carries the IID.
class WalletUiPluginInterface : public PluginInterface
{
public:
    virtual ~WalletUiPluginInterface() = default;
};

#define WalletUiPluginInterface_iid "org.logos.WalletUiPluginInterface"
Q_DECLARE_INTERFACE(WalletUiPluginInterface, WalletUiPluginInterface_iid)

#endif // WALLET_UI_PLUGIN_INTERFACE_H
