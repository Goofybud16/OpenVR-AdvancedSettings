import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import ovras.advsettings 1.0
import "../../common"

MyDialogOkCancelPopup {
    id: pttDeleteProfileDialog
    property int profileIndex: -1
    dialogTitle: "Delete Profile"
    dialogText: "Do you really want to delete this ptt profile?"
    onClosed: {
        if (okClicked) {
            AudioTabController.deletePttProfile(profileIndex)
        }
    }
}
