#include "WalletUiPlugin.h"
#include "WalletBackend.h"
#include <QQuickWidget>
#include <QQmlContext>
#include <QQmlEngine>
#include <QFile>
#include <QFileInfo>

QWidget* WalletUiPlugin::createWidget(LogosAPI* logosAPI) {
    QQuickWidget* quickWidget = new QQuickWidget();
    quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);

    qmlRegisterType<WalletBackend>("WalletBackend", 1, 0, "WalletBackend");

    WalletBackend* backend = new WalletBackend(logosAPI, quickWidget);
    quickWidget->rootContext()->setContextProperty("backend", backend);

    // Allow QML hot reload via environment variable
    QString qmlPath = "qrc:/WalletView.qml";
    QString envPath = qgetenv("WALLET_UI_QML_PATH");
    if (!envPath.isEmpty() && QFile::exists(envPath)) {
        qmlPath = QUrl::fromLocalFile(QFileInfo(envPath).absoluteFilePath()).toString();
    }

    quickWidget->setSource(QUrl(qmlPath));

    return quickWidget;
}

void WalletUiPlugin::destroyWidget(QWidget* widget) {
    delete widget;
}
