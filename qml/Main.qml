import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import ScivisQt

ApplicationWindow {
    width: 600
    height: 600 + header.height + footer.height
    visible: true
    title: "ScivisQt — QML"

    property string statusMessage: ""

    function loadPreset(name) {
	scatterBackend.loadModel("../models/" + name + ".pt")
	scatterBackend.loadData("../data/" + name + ".csv")
	statusMessage = "Preset: " + name
    }
    
    Component.onCompleted: {
        raise()
	requestActivate()  
    }

    header: ToolBar {
	RowLayout {
            anchors.fill: parent

            Button {
		text: "Load Model"
		flat: true
		onClicked: modelDialog.open()
            }
       	    Button {
		text: "Load Data"
		flat: true
		onClicked: dataDialog.open()
            }
	    
            ToolSeparator {}
	    
	    Button { text: "Separated"; flat: true;   onClicked: loadPreset("separated") }
	    Button { text: "Overlapping"; flat: true;  onClicked: loadPreset("overlapping") }
	    Button { text: "Moons";  flat: true;       onClicked: loadPreset("moons") }
	    Button { text: "3 Classes"; flat: true;    onClicked: loadPreset("three_classes") }

            Item { Layout.fillWidth: true }  //fill in the rest of the unused space
	}
    }
    
    FileDialog {
	id: modelDialog
	title: "Load Model"
	currentFolder: "file:../models"
	nameFilters: ["TorchScript models (*.pt)"]
	onAccepted:{
	    scatterBackend.loadModel(selectedFile.toString().replace("file:", ""))
	    statusMessage = "Model: " + selectedFile.toString().split("/").pop()
	}
    }

    FileDialog {
	id: dataDialog
	title: "Load Data"
	currentFolder: "file:../data"
	nameFilters: ["CSV files (*.csv)"]
	onAccepted: {
	    scatterBackend.loadData(selectedFile.toString().replace("file:", ""))
	    statusMessage = "Data: " + selectedFile.toString().split("/").pop()
	}
    }

    ScatterView{
	id: scatterView
	anchors.fill: parent
        scatterData: scatterBackend
	//title: "Custom title"
    }
    
    footer: Label {
	text: statusMessage
	color: "white"
	padding: 4
    }
}
