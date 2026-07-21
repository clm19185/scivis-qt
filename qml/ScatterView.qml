import QtQuick
import QtQuick.Layouts
import ScivisQt

ScatterViewBase {
    id: root
    
    // As Defauts
    margin: 40
    pointRadius: 4.0
    gridOpacity: 0.31
    classColors: ["#0072B2", "#D55E00", "#009E73", "#CC79A7"] //Okabe-Ito palette (Wong, Nature Methods 2011)
    //for testing
    //margin: 70
    //pointRadius: 8
    //gridOpacity: 0.5
    //classColors: ["#E69F00", "#56B4E9", "#CC79A7", "#808080"]

    property string title: "Decision boundary — LibTorch + Qt6 QML"

    // Returns the color for a given class index
    // Falls back to last color (default) if index is out of bounds
    function classColor(cls) {
        var defaultIdx = classColors.length - 1
        return classColors[cls >= 0 && cls < defaultIdx ? cls : defaultIdx]
    }

    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 8
        text: root.title
        color: "white"
        font.family: "Monospace"
        font.pixelSize: 14
        font.bold: true
	layer.enabled: true
    }

    Row {
        anchors.left: parent.left
        anchors.leftMargin: root.margin
        anchors.bottom: parent.bottom
        anchors.bottomMargin: root.margin / 2 - 8
        spacing: 20

        Repeater {
            model: root.scatterData ? root.scatterData.numClasses : 0

            Row {
                spacing: 6

                Rectangle {
                    width: 10
                    height: 10
                    radius: 5
                    color: root.classColor(index)
                    anchors.verticalCenter: parent.verticalCenter
                }

                Text {
		    z: 1
                    text: "Class " + index
                    color: "white"
                    font.family: "Monospace"
                    font.pixelSize: 12
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }
    }
}
