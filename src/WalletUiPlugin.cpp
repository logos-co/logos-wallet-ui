#include "WalletUiPlugin.h"
#include "WalletBackend.h"

#include <QDebug>

WalletUiPlugin::WalletUiPlugin(QObject* parent)
    : QObject(parent)
{
}

WalletUiPlugin::~WalletUiPlugin() = default;

void WalletUiPlugin::initLogos(LogosAPI* api)
{
    if (m_backend) return;
    m_backend = new WalletBackend(api, this);
    setBackend(m_backend);
    qDebug() << "WalletUiPlugin: backend initialized";
}
