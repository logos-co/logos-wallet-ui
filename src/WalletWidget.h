#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QLabel>
#include <QFrame>
#include <QTabWidget>
#include <QScrollArea>
#include <QComboBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QGroupBox>
#include <QMap>
#include <string>
#include "logos_api.h"
#include "logos_api_client.h"
#include "logos_sdk.h"

class WalletWidget : public QWidget {
    Q_OBJECT

public:
    explicit WalletWidget(QWidget* parent = nullptr);
    ~WalletWidget();

    // Wallet operations
    void initWallet();
    void stopWallet();

private slots:
    void onRpcCallButtonClicked();
    void onChainIDButtonClicked();
    void onEthBalanceButtonClicked();
    
    // Transaction generator methods
    void onTxTypeChanged(int index);
    void onFeeTypeChanged(int id);
    void onGenerateTransactionButtonClicked();
    
    // Transaction utility methods
    void onTransactionJsonToRlpButtonClicked();
    void onTransactionRlpToJsonButtonClicked();
    void onTransactionGetHashButtonClicked();

private:
    // UI elements
    QVBoxLayout* mainLayout;
    QTabWidget* tabWidget;
    QWidget* ethClientTab;
    QWidget* transactionsTab;
    QScrollArea* transactionsScrollArea;
    QWidget* transactionsScrollContent;
    QVBoxLayout* ethClientTabLayout;
    QVBoxLayout* transactionsTabLayout;

    // Status
    QLabel* statusLabel;

    // EthClient tab UI elements
    QPushButton* rpcCallButton;
    QLineEdit* rpcCallMethodInput;
    QLineEdit* rpcCallParamsInput;
    QLabel* rpcCallResultLabel;

    QPushButton* chainIDButton;
    QLabel* chainIDLabel;

    QLineEdit* ethBalanceAddressInput;
    QPushButton* ethBalanceButton;
    QLabel* ethBalanceAddressLabel;
    QLabel* ethBalanceValueLabel;
    
    // Transactions tab UI elements - TxGenerator (unified UI)
    QComboBox* txTypeComboBox;
    QButtonGroup* feeTypeButtonGroup;
    QRadioButton* feeLegacyRadio;
    QRadioButton* feeEIP1559Radio;
    
    // Common transaction fields
    QLineEdit* txNonceInput;
    QLineEdit* txGasLimitInput;
    QLineEdit* txChainIDInput;
    
    // Legacy fee fields
    QLineEdit* txGasPriceInput;
    QGroupBox* legacyFeeGroup;
    
    // EIP-1559 fee fields
    QLineEdit* txMaxFeePerGasInput;
    QLineEdit* txMaxPriorityFeePerGasInput;
    QGroupBox* eip1559FeeGroup;
    
    // Dynamic parameter fields container
    QWidget* txParamsContainer;
    QVBoxLayout* txParamsLayout;
    
    // Dynamic parameter inputs (will be created/destroyed based on tx type)
    QMap<QString, QLineEdit*> txParamInputs;
    QList<QWidget*> txParamWidgets; // Store all widgets (labels and inputs) for cleanup
    
    // Generate button and result
    QPushButton* txGenerateButton;
    QTextEdit* txResultOutput;
    
    // Transaction utility methods
    // Json To Rlp
    QFrame* txJsonToRlpFrame;
    QTextEdit* txJsonToRlpInput;
    QPushButton* txJsonToRlpButton;
    QLabel* txJsonToRlpResultLabel;
    QTextEdit* txJsonToRlpResult;
    
    // Rlp To Json
    QFrame* txRlpToJsonFrame;
    QLineEdit* txRlpToJsonInput;
    QPushButton* txRlpToJsonButton;
    QLabel* txRlpToJsonResultLabel;
    QTextEdit* txRlpToJsonResult;
    
    // Get Hash
    QFrame* txGetHashFrame;
    QTextEdit* txGetHashInput;
    QPushButton* txGetHashButton;
    QLabel* txGetHashResultLabel;
    QLineEdit* txGetHashResult;

    // LogosAPI instance for remote method calls
    LogosAPI* m_logosAPI;
    LogosModules* logos;
    
    // Connection status
    bool isWalletInitialized;

    // Helper methods
    void updateStatus(const QString& message);
    QFrame* createSeparator();
    void setupTransactionsTab();
    void updateTxParamsFields(const QString& txType);
    QString buildTxParamsJSON();
}; 
