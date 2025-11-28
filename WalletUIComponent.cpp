#include "WalletUIComponent.h"
#include "src/WalletWidget.h"

QWidget* WalletUIComponent::createWidget(LogosAPI* logosAPI) {
    // LogosAPI parameter available but not used - WalletWidget creates its own
    return new WalletWidget();
}

void WalletUIComponent::destroyWidget(QWidget* widget) {
    delete widget;
}
