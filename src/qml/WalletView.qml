import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

// WalletStatus enum (NotInitialized/Initializing/Ready/Error) declared in
// WalletBackend.rep — registered with QML by the replica factory plugin.
import Logos.WalletBackend 1.0

Rectangle {
    id: root
    color: _d.bgColor

    readonly property var backend: logos.module("wallet_ui")
    property bool ready: false

    Connections {
        target: logos
        function onViewModuleReadyChanged(moduleName, isReady) {
            if (moduleName === "wallet_ui")
                root.ready = isReady && root.backend !== null
        }
    }

    Component.onCompleted: {
        root.ready = root.backend !== null && logos.isViewModuleReady("wallet_ui")
    }

    QtObject {
        id: _d

        // Theme colors (matching logos-accounts-ui)
        readonly property color bgColor: "#1e1e1e"
        readonly property color panelColor: "#2d2d2d"
        readonly property color inputColor: "#3c3c3c"
        readonly property color borderColor: "#555555"
        readonly property color textColor: "#e0e0e0"
        readonly property color labelColor: "#b0b0b0"
        readonly property color accentColor: "#4fc3f7"
        readonly property color successColor: "#66bb6a"
        readonly property color errorColor: "#ef5350"
        readonly property color buttonColor: "#0d47a1"
        readonly property color buttonHoverColor: "#1565c0"
        readonly property color separatorColor: "#444444"

        // Font sizes
        readonly property int titleSize: 18
        readonly property int sectionSize: 14
        readonly property int textSize: 12

        // Helpers
        function getStatusString(status) {
            switch (status) {
                case WalletBackend.NotInitialized: return "Not Initialized"
                case WalletBackend.Initializing: return "Initializing..."
                case WalletBackend.Ready: return "Ready"
                case WalletBackend.Error: return "Error"
                default: return "Unknown"
            }
        }

        function getStatusColor(status) {
            switch (status) {
                case WalletBackend.Ready: return _d.successColor
                case WalletBackend.Error: return _d.errorColor
                case WalletBackend.Initializing: return _d.accentColor
                default: return _d.labelColor
            }
        }

        // Transaction type definitions
        readonly property var txTypes: [
            { text: "-- Select Transaction Type --", value: "" },
            { text: "Transfer ETH", value: "transferETH" },
            { text: "Transfer ERC20", value: "transferERC20" },
            { text: "Approve ERC20", value: "approveERC20" },
            { text: "Transfer ERC721 (transferFrom)", value: "transferFromERC721" },
            { text: "Transfer ERC721 (safeTransferFrom)", value: "safeTransferFromERC721" },
            { text: "Approve ERC721", value: "approveERC721" },
            { text: "Set Approval For All ERC721", value: "setApprovalForAllERC721" },
            { text: "Transfer ERC1155", value: "transferERC1155" },
            { text: "Batch Transfer ERC1155", value: "batchTransferERC1155" },
            { text: "Set Approval For All ERC1155", value: "setApprovalForAllERC1155" }
        ]

        function getTxParams(txType) {
            switch (txType) {
                case "transferETH":
                    return [
                        { name: "to", placeholder: "0x..." },
                        { name: "value", placeholder: "1000000000000000000" }
                    ]
                case "transferERC20":
                    return [
                        { name: "tokenAddress", placeholder: "0x..." },
                        { name: "to", placeholder: "0x..." },
                        { name: "amount", placeholder: "1000000" }
                    ]
                case "approveERC20":
                    return [
                        { name: "tokenAddress", placeholder: "0x..." },
                        { name: "spender", placeholder: "0x..." },
                        { name: "amount", placeholder: "1000000" }
                    ]
                case "transferFromERC721":
                case "safeTransferFromERC721":
                    return [
                        { name: "tokenAddress", placeholder: "0x..." },
                        { name: "from", placeholder: "0x..." },
                        { name: "to", placeholder: "0x..." },
                        { name: "tokenID", placeholder: "1234" }
                    ]
                case "approveERC721":
                    return [
                        { name: "tokenAddress", placeholder: "0x..." },
                        { name: "to", placeholder: "0x..." },
                        { name: "tokenID", placeholder: "1234" }
                    ]
                case "setApprovalForAllERC721":
                case "setApprovalForAllERC1155":
                    return [
                        { name: "tokenAddress", placeholder: "0x..." },
                        { name: "operator", placeholder: "0x..." },
                        { name: "approved", placeholder: "true" }
                    ]
                case "transferERC1155":
                    return [
                        { name: "tokenAddress", placeholder: "0x..." },
                        { name: "from", placeholder: "0x..." },
                        { name: "to", placeholder: "0x..." },
                        { name: "tokenID", placeholder: "1234" },
                        { name: "value", placeholder: "1" }
                    ]
                case "batchTransferERC1155":
                    return [
                        { name: "tokenAddress", placeholder: "0x..." },
                        { name: "from", placeholder: "0x..." },
                        { name: "to", placeholder: "0x..." },
                        { name: "tokenIDs", placeholder: "1,2,3" },
                        { name: "values", placeholder: "1,2,3" }
                    ]
                default:
                    return []
            }
        }

        function buildTxParamsJSON(txType, nonce, gasLimit, chainID, useEIP1559, gasPrice, maxFeePerGas, maxPriorityFeePerGas, dynamicParams) {
            var obj = {}

            if (nonce !== "") {
                var nonceVal = parseInt(nonce)
                if (!isNaN(nonceVal)) obj["nonce"] = nonceVal
            }
            if (gasLimit !== "") {
                var gasLimitVal = parseInt(gasLimit)
                if (!isNaN(gasLimitVal)) obj["gasLimit"] = gasLimitVal
            }
            if (chainID !== "") {
                obj["chainID"] = chainID
            }

            if (useEIP1559) {
                if (maxFeePerGas !== "") obj["maxFeePerGas"] = maxFeePerGas
                if (maxPriorityFeePerGas !== "") obj["maxPriorityFeePerGas"] = maxPriorityFeePerGas
            } else {
                if (gasPrice !== "") obj["gasPrice"] = gasPrice
            }

            for (var key in dynamicParams) {
                var value = dynamicParams[key]
                if (value === "") continue

                if (key === "approved") {
                    obj[key] = (value.toLowerCase() === "true")
                } else if (key === "tokenIDs" || key === "values") {
                    var items = value.split(",")
                    var arr = []
                    for (var i = 0; i < items.length; i++) {
                        arr.push(items[i].trim())
                    }
                    obj[key] = arr
                } else {
                    obj[key] = value
                }
            }

            return JSON.stringify(obj)
        }
    }

    // --- Reusable inline components ---

    component SectionHeader: Text {
        Layout.fillWidth: true
        color: _d.accentColor
        font.pixelSize: _d.sectionSize
        font.bold: true
        topPadding: 4
        bottomPadding: 4
    }

    component ResultLabel: Text {
        Layout.fillWidth: true
        color: _d.labelColor
        font.pixelSize: _d.textSize
    }

    component StyledInput: TextField {
        Layout.fillWidth: true
        color: _d.textColor
        font.pixelSize: _d.textSize
        leftPadding: 8
        rightPadding: 8
        topPadding: 6
        bottomPadding: 6
        background: Rectangle {
            color: _d.inputColor
            radius: 4
            border.color: parent.activeFocus ? _d.accentColor : _d.borderColor
            border.width: 1
        }
        placeholderTextColor: "#888888"
    }

    component StyledTextArea: ScrollView {
        property alias text: textAreaInner.text
        property alias placeholderText: textAreaInner.placeholderText
        Layout.fillWidth: true
        Layout.preferredHeight: 60
        background: Rectangle {
            color: _d.inputColor
            radius: 4
            border.color: _d.borderColor
            border.width: 1
        }
        TextArea {
            id: textAreaInner
            color: _d.textColor
            font.pixelSize: _d.textSize
            placeholderTextColor: "#888888"
            wrapMode: TextArea.Wrap
        }
    }

    component ResultField: TextField {
        Layout.fillWidth: true
        readOnly: true
        color: _d.textColor
        font.pixelSize: _d.textSize
        leftPadding: 8
        rightPadding: 8
        topPadding: 6
        bottomPadding: 6
        placeholderText: "Result will appear here..."
        placeholderTextColor: "#888888"
        background: Rectangle {
            color: _d.panelColor
            radius: 4
            border.color: _d.borderColor
            border.width: 1
        }
    }

    component ResultArea: ScrollView {
        property alias text: resultTextInner.text
        Layout.fillWidth: true
        Layout.preferredHeight: 60
        background: Rectangle {
            color: _d.panelColor
            radius: 4
            border.color: _d.borderColor
            border.width: 1
        }
        TextArea {
            id: resultTextInner
            readOnly: true
            color: _d.textColor
            font.pixelSize: _d.textSize
            placeholderText: "Result will appear here..."
            placeholderTextColor: "#888888"
            wrapMode: TextArea.Wrap
        }
    }

    component StyledButton: Button {
        contentItem: Text {
            text: parent.text
            color: _d.textColor
            font.pixelSize: _d.textSize
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        background: Rectangle {
            color: parent.hovered ? _d.buttonHoverColor : _d.buttonColor
            radius: 4
        }
        leftPadding: 16
        rightPadding: 16
        topPadding: 6
        bottomPadding: 6
    }

    component Separator: Rectangle {
        Layout.fillWidth: true
        height: 1
        color: _d.separatorColor
    }

    // --- Loading state ---
    ColumnLayout {
        anchors.centerIn: parent
        visible: !root.ready
        spacing: 12
        Text {
            Layout.alignment: Qt.AlignHCenter
            text: "Connecting to wallet backend..."
            color: _d.labelColor
            font.pixelSize: _d.textSize
        }
        BusyIndicator { Layout.alignment: Qt.AlignHCenter; running: !root.ready }
    }

    // --- Main layout ---
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12
        visible: root.ready

        // Status bar
        Rectangle {
            Layout.fillWidth: true
            height: 30
            color: _d.panelColor
            radius: 4

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 12
                anchors.rightMargin: 12
                spacing: 8

                Text {
                    text: "Status:"
                    color: _d.labelColor
                    font.pixelSize: _d.textSize
                }
                Text {
                    text: root.backend ? _d.getStatusString(root.backend.status) : "N/A"
                    color: root.backend ? _d.getStatusColor(root.backend.status) : _d.labelColor
                    font.pixelSize: _d.textSize
                    font.bold: true
                }
                Item { Layout.fillWidth: true }
                Text {
                    text: root.backend ? root.backend.statusMessage : ""
                    color: _d.labelColor
                    font.pixelSize: _d.textSize
                    elide: Text.ElideRight
                    Layout.maximumWidth: 400
                }
            }
        }

        // Tab bar
        TabBar {
            id: tabBar
            Layout.fillWidth: true
            background: Rectangle { color: _d.panelColor; radius: 4 }

            TabButton {
                text: "EthClient"
                font.pixelSize: _d.textSize
                contentItem: Text {
                    text: parent.text
                    color: tabBar.currentIndex === 0 ? _d.accentColor : _d.labelColor
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font: parent.font
                }
                background: Rectangle {
                    color: tabBar.currentIndex === 0 ? _d.inputColor : "transparent"
                    radius: 4
                }
            }

            TabButton {
                text: "Transactions"
                font.pixelSize: _d.textSize
                contentItem: Text {
                    text: parent.text
                    color: tabBar.currentIndex === 1 ? _d.accentColor : _d.labelColor
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font: parent.font
                }
                background: Rectangle {
                    color: tabBar.currentIndex === 1 ? _d.inputColor : "transparent"
                    radius: 4
                }
            }
        }

        // Tab content
        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: tabBar.currentIndex

            // === EthClient Tab ===
            ScrollView {
                id: ethClientScrollView
                clip: true

                ColumnLayout {
                    width: ethClientScrollView.availableWidth
                    spacing: 12

                    SectionHeader { text: "EthClient Management" }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        ResultLabel { text: "RPC URL:"; Layout.fillWidth: false }
                        StyledInput {
                            id: rpcUrlInput
                            placeholderText: "https://..."
                        }
                        StyledButton {
                            text: "Init EthClient"
                            onClicked: {
                                if (root.backend) root.backend.initEthClient(rpcUrlInput.text)
                                rpcUrlInput.text = ""
                            }
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        StyledButton {
                            text: "Refresh List"
                            onClicked: if (root.backend) root.backend.refreshClients()
                        }

                        ResultLabel { text: "Selected EthClient:"; Layout.fillWidth: false }

                        ComboBox {
                            id: clientSelector
                            Layout.fillWidth: true
                            model: root.backend
                                ? ["-- No EthClient Selected --"].concat(root.backend.clients)
                                : ["-- No EthClient Selected --"]
                            currentIndex: root.backend ? root.backend.selectedClientIndex + 1 : 0
                            onCurrentIndexChanged: {
                                if (root.backend)
                                    root.backend.selectedClientIndex = currentIndex - 1
                            }
                            font.pixelSize: _d.textSize
                            contentItem: Text {
                                leftPadding: 8
                                text: clientSelector.displayText
                                color: _d.textColor
                                font.pixelSize: _d.textSize
                                verticalAlignment: Text.AlignVCenter
                                elide: Text.ElideRight
                            }
                            background: Rectangle {
                                implicitHeight: 30
                                color: _d.inputColor
                                radius: 4
                                border.color: clientSelector.pressed ? _d.accentColor : _d.borderColor
                                border.width: 1
                            }
                            popup: Popup {
                                y: clientSelector.height
                                width: clientSelector.width
                                implicitHeight: contentItem.implicitHeight
                                padding: 1
                                contentItem: ListView {
                                    clip: true
                                    implicitHeight: contentHeight
                                    model: clientSelector.popup.visible ? clientSelector.delegateModel : null
                                    ScrollIndicator.vertical: ScrollIndicator {}
                                }
                                background: Rectangle {
                                    color: _d.panelColor
                                    border.color: _d.borderColor
                                    radius: 4
                                }
                            }
                            delegate: ItemDelegate {
                                width: clientSelector.width
                                contentItem: Text {
                                    text: modelData
                                    color: highlighted ? _d.textColor : _d.labelColor
                                    font.pixelSize: _d.textSize
                                    elide: Text.ElideRight
                                    verticalAlignment: Text.AlignVCenter
                                }
                                highlighted: clientSelector.highlightedIndex === index
                                background: Rectangle {
                                    color: highlighted ? _d.inputColor : "transparent"
                                }
                            }
                        }

                        StyledButton {
                            text: "Close Selected"
                            enabled: root.backend && root.backend.selectedClientIndex >= 0
                            onClicked: {
                                if (!root.backend) return
                                var url = root.backend.clients[root.backend.selectedClientIndex]
                                root.backend.closeEthClient(url)
                            }
                        }
                    }

                    Separator {}

                    // RPC Call Section
                    SectionHeader { text: "RPC Call" }

                    ResultLabel { text: "Method:" }
                    StyledInput {
                        id: rpcMethodInput
                        placeholderText: "Enter RPC Call Method..."
                        enabled: root.backend && root.backend.selectedClientIndex >= 0
                    }

                    ResultLabel { text: "Params:" }
                    StyledInput {
                        id: rpcParamsInput
                        placeholderText: "Enter RPC Call Params..."
                        enabled: root.backend && root.backend.selectedClientIndex >= 0
                    }

                    StyledButton {
                        text: "RPC Call"
                        enabled: root.backend && root.backend.selectedClientIndex >= 0
                        onClicked: {
                            var url = root.backend.clients[root.backend.selectedClientIndex]
                            logos.watch(
                                root.backend.rpcCall(url, rpcMethodInput.text, rpcParamsInput.text),
                                function(result) { rpcCallResult.text = result },
                                function(error) { rpcCallResult.text = "Error: " + error }
                            )
                        }
                    }

                    ResultLabel { text: "RPC Call Result:" }
                    ResultArea { id: rpcCallResult }

                    Separator {}

                    // Chain ID Section
                    SectionHeader { text: "Chain ID" }

                    StyledButton {
                        text: "Get Chain ID"
                        enabled: root.backend && root.backend.selectedClientIndex >= 0
                        onClicked: {
                            var url = root.backend.clients[root.backend.selectedClientIndex]
                            logos.watch(
                                root.backend.getChainId(url),
                                function(result) { chainIdOutput.text = result },
                                function(error) { chainIdOutput.text = "Error: " + error }
                            )
                        }
                    }

                    ResultLabel { text: "Chain ID:" }
                    ResultField { id: chainIdOutput; placeholderText: "Chain ID will appear here..." }

                    Separator {}

                    // Balance Section
                    SectionHeader { text: "ETH Balance" }

                    ResultLabel { text: "Address:" }
                    StyledInput {
                        id: balanceAddressInput
                        placeholderText: "Enter ETH Balance Address..."
                        enabled: root.backend && root.backend.selectedClientIndex >= 0
                    }

                    StyledButton {
                        text: "Get ETH Balance"
                        enabled: root.backend && root.backend.selectedClientIndex >= 0
                        onClicked: {
                            var url = root.backend.clients[root.backend.selectedClientIndex]
                            balanceAddressOutput.text = balanceAddressInput.text
                            logos.watch(
                                root.backend.getBalance(url, balanceAddressInput.text),
                                function(result) { balanceValueOutput.text = result },
                                function(error) { balanceValueOutput.text = "Error: " + error }
                            )
                        }
                    }

                    ResultLabel { text: "ETH Balance Address:" }
                    ResultField { id: balanceAddressOutput; placeholderText: "Address will appear here..." }

                    ResultLabel { text: "ETH Balance:" }
                    ResultField { id: balanceValueOutput; placeholderText: "Balance will appear here..." }

                    Item { Layout.fillHeight: true }
                }
            }

            // === Transactions Tab ===
            ScrollView {
                id: txScrollView
                clip: true

                ColumnLayout {
                    width: txScrollView.availableWidth
                    spacing: 12

                    SectionHeader { text: "Transaction Generator" }

                    ResultLabel {
                        text: "Select a transaction type, choose fee type (Legacy or EIP-1559), fill in the required parameters, and click \"Generate Transaction\" to get the transaction in JSON format."
                        wrapMode: Text.Wrap
                    }

                    ResultLabel { text: "Transaction Type:" }
                    ComboBox {
                        id: txTypeCombo
                        Layout.fillWidth: true
                        model: _d.txTypes.map(function(t) { return t.text })
                        font.pixelSize: _d.textSize
                        onCurrentIndexChanged: {
                            txParamsRepeater.model = _d.getTxParams(
                                currentIndex > 0 ? _d.txTypes[currentIndex].value : ""
                            )
                            txResultOutput.text = ""
                        }
                        contentItem: Text {
                            leftPadding: 8
                            text: txTypeCombo.displayText
                            color: _d.textColor
                            font.pixelSize: _d.textSize
                            verticalAlignment: Text.AlignVCenter
                            elide: Text.ElideRight
                        }
                        background: Rectangle {
                            implicitHeight: 30
                            color: _d.inputColor
                            radius: 4
                            border.color: txTypeCombo.pressed ? _d.accentColor : _d.borderColor
                            border.width: 1
                        }
                        popup: Popup {
                            y: txTypeCombo.height
                            width: txTypeCombo.width
                            implicitHeight: contentItem.implicitHeight
                            padding: 1
                            contentItem: ListView {
                                clip: true
                                implicitHeight: contentHeight
                                model: txTypeCombo.popup.visible ? txTypeCombo.delegateModel : null
                                ScrollIndicator.vertical: ScrollIndicator {}
                            }
                            background: Rectangle {
                                color: _d.panelColor
                                border.color: _d.borderColor
                                radius: 4
                            }
                        }
                        delegate: ItemDelegate {
                            width: txTypeCombo.width
                            contentItem: Text {
                                text: modelData
                                color: highlighted ? _d.textColor : _d.labelColor
                                font.pixelSize: _d.textSize
                                elide: Text.ElideRight
                                verticalAlignment: Text.AlignVCenter
                            }
                            highlighted: txTypeCombo.highlightedIndex === index
                            background: Rectangle {
                                color: highlighted ? _d.inputColor : "transparent"
                            }
                        }
                    }

                    // Fee type selector
                    ResultLabel { text: "Fee Type:" }
                    RowLayout {
                        spacing: 16

                        RadioButton {
                            id: legacyRadio
                            text: "Legacy (GasPrice)"
                            checked: true
                            contentItem: Text {
                                text: parent.text
                                color: _d.textColor
                                font.pixelSize: _d.textSize
                                leftPadding: parent.indicator.width + parent.spacing
                            }
                            indicator: Rectangle {
                                implicitWidth: 16
                                implicitHeight: 16
                                x: parent.leftPadding
                                y: parent.height / 2 - height / 2
                                radius: 8
                                color: "transparent"
                                border.color: _d.borderColor
                                border.width: 1
                                Rectangle {
                                    width: 8; height: 8
                                    x: 4; y: 4
                                    radius: 4
                                    color: _d.accentColor
                                    visible: legacyRadio.checked
                                }
                            }
                        }

                        RadioButton {
                            id: eip1559Radio
                            text: "EIP-1559 (MaxFeePerGas + MaxPriorityFeePerGas)"
                            contentItem: Text {
                                text: parent.text
                                color: _d.textColor
                                font.pixelSize: _d.textSize
                                leftPadding: parent.indicator.width + parent.spacing
                            }
                            indicator: Rectangle {
                                implicitWidth: 16
                                implicitHeight: 16
                                x: parent.leftPadding
                                y: parent.height / 2 - height / 2
                                radius: 8
                                color: "transparent"
                                border.color: _d.borderColor
                                border.width: 1
                                Rectangle {
                                    width: 8; height: 8
                                    x: 4; y: 4
                                    radius: 4
                                    color: _d.accentColor
                                    visible: eip1559Radio.checked
                                }
                            }
                        }
                    }

                    // Common fields
                    ResultLabel { text: "Nonce:" }
                    StyledInput { id: txNonceInput; text: "0"; placeholderText: "0" }

                    ResultLabel { text: "Gas Limit:" }
                    StyledInput { id: txGasLimitInput; text: "21000"; placeholderText: "21000" }

                    ResultLabel { text: "Chain ID:" }
                    StyledInput { id: txChainIdInput; text: "1"; placeholderText: "1" }

                    // Legacy Fee Group
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 8
                        visible: legacyRadio.checked

                        SectionHeader { text: "Legacy Fee" }
                        ResultLabel { text: "Gas Price (wei):" }
                        StyledInput { id: txGasPriceInput; text: "20000000000"; placeholderText: "20000000000" }
                    }

                    // EIP-1559 Fee Group
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 8
                        visible: eip1559Radio.checked

                        SectionHeader { text: "EIP-1559 Fee" }
                        ResultLabel { text: "Max Fee Per Gas (wei):" }
                        StyledInput { id: txMaxFeeInput; text: "30000000000"; placeholderText: "30000000000" }
                        ResultLabel { text: "Max Priority Fee Per Gas (wei):" }
                        StyledInput { id: txMaxPriorityFeeInput; text: "2000000000"; placeholderText: "2000000000" }
                    }

                    // Dynamic Transaction Parameters
                    SectionHeader {
                        text: "Transaction Parameters"
                        visible: txParamsRepeater.count > 0
                    }

                    Repeater {
                        id: txParamsRepeater
                        model: []

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 4

                            ResultLabel { text: modelData.name + ":" }
                            StyledInput {
                                id: paramField
                                placeholderText: modelData.placeholder
                                objectName: modelData.name
                            }
                        }
                    }

                    // Generate button
                    StyledButton {
                        text: "Generate Transaction"
                        enabled: txTypeCombo.currentIndex > 0
                        onClicked: {
                            if (!root.backend) return
                            var txType = _d.txTypes[txTypeCombo.currentIndex].value

                            var dynamicParams = {}
                            for (var i = 0; i < txParamsRepeater.count; i++) {
                                var item = txParamsRepeater.itemAt(i)
                                var field = item.children[1]
                                if (field && field.objectName && field.text) {
                                    dynamicParams[field.objectName] = field.text
                                }
                            }

                            var paramsJSON = _d.buildTxParamsJSON(
                                txType,
                                txNonceInput.text,
                                txGasLimitInput.text,
                                txChainIdInput.text,
                                eip1559Radio.checked,
                                txGasPriceInput.text,
                                txMaxFeeInput.text,
                                txMaxPriorityFeeInput.text,
                                dynamicParams
                            )

                            logos.watch(
                                root.backend.generateTransaction(txType, paramsJSON),
                                function(result) { txResultOutput.text = result },
                                function(error) { txResultOutput.text = "Error: " + error }
                            )
                        }
                    }

                    ResultLabel { text: "Generated Transaction:" }
                    ResultArea { id: txResultOutput }

                    Separator {}

                    // Transaction JSON to RLP
                    SectionHeader { text: "Transaction JSON to RLP" }

                    ResultLabel { text: "Transaction JSON:" }
                    StyledTextArea { id: jsonToRlpInput; placeholderText: "{\"nonce\": \"0x...\", \"gasPrice\": \"0x...\", ...}" }

                    StyledButton {
                        text: "Convert JSON to RLP"
                        onClicked: {
                            if (!root.backend) return
                            logos.watch(
                                root.backend.jsonToRlp(jsonToRlpInput.text),
                                function(result) { jsonToRlpResult.text = result },
                                function(error) { jsonToRlpResult.text = "Error: " + error }
                            )
                        }
                    }

                    ResultLabel { text: "RLP Result:" }
                    ResultArea { id: jsonToRlpResult }

                    Separator {}

                    // Transaction RLP to JSON
                    SectionHeader { text: "Transaction RLP to JSON" }

                    ResultLabel { text: "RLP Hex:" }
                    StyledInput { id: rlpToJsonInput; placeholderText: "0x..." }

                    StyledButton {
                        text: "Convert RLP to JSON"
                        onClicked: {
                            if (!root.backend) return
                            logos.watch(
                                root.backend.rlpToJson(rlpToJsonInput.text),
                                function(result) { rlpToJsonResult.text = result },
                                function(error) { rlpToJsonResult.text = "Error: " + error }
                            )
                        }
                    }

                    ResultLabel { text: "JSON Result:" }
                    ResultArea { id: rlpToJsonResult }

                    Separator {}

                    // Get Transaction Hash
                    SectionHeader { text: "Get Transaction Hash" }

                    ResultLabel { text: "Transaction JSON:" }
                    StyledTextArea { id: getHashInput; placeholderText: "{\"nonce\": \"0x...\", \"gasPrice\": \"0x...\", ...}" }

                    StyledButton {
                        text: "Get Transaction Hash"
                        onClicked: {
                            if (!root.backend) return
                            logos.watch(
                                root.backend.getHash(getHashInput.text),
                                function(result) { getHashResult.text = result },
                                function(error) { getHashResult.text = "Error: " + error }
                            )
                        }
                    }

                    ResultLabel { text: "Hash Result:" }
                    ResultField { id: getHashResult; placeholderText: "Transaction hash will appear here..." }

                    Item { Layout.fillHeight: true }
                }
            }
        }
    }
}
