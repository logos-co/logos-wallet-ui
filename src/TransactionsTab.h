#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QLabel>
#include <QFrame>
#include <QComboBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QGroupBox>
#include <QScrollArea>
#include <QMap>
#include "logos_sdk.h"

class TransactionsTab : public QWidget {
    Q_OBJECT

public:
    explicit TransactionsTab(LogosModules* logos, QWidget* parent = nullptr);
    ~TransactionsTab() override = default;

signals:
    void statusChanged(const QString& message);

private slots:
    // Transaction generator slots
    void onTxTypeChanged(int index);
    void onFeeTypeChanged(int id);
    void onGenerateTransactionButtonClicked();

    // Transaction utility slots
    void onTransactionJsonToRlpButtonClicked();
    void onTransactionRlpToJsonButtonClicked();
    void onTransactionGetHashButtonClicked();

private:
    // Setup methods
    void setupUI();
    void setupTransactionGenerator(QVBoxLayout* layout);
    void setupTransactionUtilities(QVBoxLayout* layout);
    QFrame* createSeparator();

    // Helper methods
    void updateTxParamsFields(const QString& txType);
    QString buildTxParamsJSON();

    // LogosModules instance
    LogosModules* m_logos;

    // Transaction Generator widgets
    QComboBox* m_txTypeComboBox;
    QButtonGroup* m_feeTypeButtonGroup;
    QRadioButton* m_feeLegacyRadio;
    QRadioButton* m_feeEIP1559Radio;

    // Common transaction fields
    QLineEdit* m_txNonceInput;
    QLineEdit* m_txGasLimitInput;
    QLineEdit* m_txChainIDInput;

    // Legacy fee fields
    QLineEdit* m_txGasPriceInput;
    QGroupBox* m_legacyFeeGroup;

    // EIP-1559 fee fields
    QLineEdit* m_txMaxFeePerGasInput;
    QLineEdit* m_txMaxPriorityFeePerGasInput;
    QGroupBox* m_eip1559FeeGroup;

    // Dynamic parameter fields container
    QWidget* m_txParamsContainer;
    QVBoxLayout* m_txParamsLayout;

    // Dynamic parameter inputs (will be created/destroyed based on tx type)
    QMap<QString, QLineEdit*> m_txParamInputs;
    QList<QWidget*> m_txParamWidgets; // Store all widgets for cleanup

    // Generate button and result
    QPushButton* m_txGenerateButton;
    QTextEdit* m_txResultOutput;

    // Transaction utility widgets
    // Json To Rlp
    QTextEdit* m_txJsonToRlpInput;
    QPushButton* m_txJsonToRlpButton;
    QTextEdit* m_txJsonToRlpResult;

    // Rlp To Json
    QLineEdit* m_txRlpToJsonInput;
    QPushButton* m_txRlpToJsonButton;
    QTextEdit* m_txRlpToJsonResult;

    // Get Hash
    QTextEdit* m_txGetHashInput;
    QPushButton* m_txGetHashButton;
    QLineEdit* m_txGetHashResult;
};
