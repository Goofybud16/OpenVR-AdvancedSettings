import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import ovras.advsettings 1.0
import "../../common"

GroupBox {
    id: universeTurnGroupBox
    Layout.fillWidth: true

    label: MyText {
        leftPadding: 10
        text: "Universe Turn"
        bottomPadding: -10
    }
    background: Rectangle {
        color: "transparent"
        border.color: "#ffffff"
        radius: 8
    }

    ColumnLayout {
        anchors.fill: parent

        Rectangle {
            color: "#ffffff"
            height: 1
            Layout.fillWidth: true
            Layout.bottomMargin: 5
        }

        RowLayout {
            Layout.fillWidth: true

            MyToggleButton {
                id: turnBindLeft
                text: "Enable Left Hand"
                onCheckedChanged: {
                    MoveCenterTabController.turnBindLeft = this.checked
                }
            }

            MyToggleButton {
                id: turnBindRight
                text: "Enable Right Hand"
                onCheckedChanged: {
                    MoveCenterTabController.turnBindRight = this.checked
                }
            }

            Item {
                Layout.fillWidth: true
            }

            MyText {
                text: "Turn Comfort Mode:"
                horizontalAlignment: Text.AlignRight
                Layout.rightMargin: 10
            }

            MySlider {
                id: turnComfortSlider
                from: 0
                to: 10
                stepSize: 1
                value: 0
                Layout.preferredWidth: 200
                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                onValueChanged: {
                    turnComfortText.text = turnComfortSlider.value
                    MoveCenterTabController.turnComfortFactor = turnComfortSlider.value
                }
            }

            MyText {
                id: turnComfortText
                text: "0"
                horizontalAlignment: Text.AlignRight
                Layout.preferredWidth: 30
                Layout.rightMargin: 10
            }
        }
    }

    Component.onCompleted: {
        turnBindLeft.checked = MoveCenterTabController.turnBindLeft
        turnBindRight.checked = MoveCenterTabController.turnBindRight
        turnComfortSlider.value = MoveCenterTabController.turnComfortFactor
    }

    Connections {
        target: MoveCenterTabController

        onTurnBindLeftChanged: {
            turnBindLeft.checked = MoveCenterTabController.turnBindLeft
        }
        onTurnBindRightChanged: {
            turnBindRight.checked = MoveCenterTabController.turnBindRight
        }
        onTurnComfortFactorChanged: {
            turnComfortSlider.value = MoveCenterTabController.turnComfortFactor
        }
    }
}
