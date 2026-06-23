import QtQuick
import QtQuick.Window

Window {
    width: 600
    height: 600
    visible: true
    title: "ScivisQt — QML"

    Canvas {
        anchors.fill: parent
        id: canvas

        // Trigger repaint when backend data changes
        Connections {
            target: scatterData
            function onPointsChanged() { canvas.requestPaint() }
            function onGridChanged() { canvas.requestPaint() }
        }


        // Cursor state — updated by MouseArea, read in onPaint
        property real cursorX: -1 // cursor position in data coordinates
        property real cursorY: -1
	property real cursorScreenX: -1 // cursor position in screen coordinates
	property real cursorScreenY: -1 
        property int cursorClass: -1 // predicted class (-1 = outside widget)

    	onPaint: {
            var ctx = getContext("2d")

	    // Background
	    ctx.fillStyle = "black"
	    ctx.fillRect(0, 0, width, height)

	    //Title
	    ctx.fillStyle = "white"
	    ctx.font = "bold 14px monospace"
	    ctx.textAlign = "center"
	    ctx.fillText("Decision boundary — LibTorch + Qt6 QML", width / 2, 24)
	    ctx.textAlign = "left"  

	    // Read backend data and constants
	    var margin = 40
	    var pts = scatterData.points
            var xMin = scatterData.xMin
            var xMax = scatterData.xMax
            var yMin = scatterData.yMin
            var yMax = scatterData.yMax
	    var gridSize = scatterData.gridSize
	    var grid = scatterData.grid

	    // Decision region grid — one colored cell per inference result
	    if(grid.length != 0) {
    	        var cellW = (width - 2 * margin) / gridSize
    		var cellH = (height - 2 * margin) / gridSize
    		for (var row = 0; row < gridSize; row++) {
      		for (var col = 0; col < gridSize; col++) {
		    var cls = grid[row * gridSize + col]
		    ctx.fillStyle = (cls == 0) ? "rgba(0, 0, 150, 0.31)" : "rgba(150, 0, 0, 0.31)"
		    ctx.beginPath()
		    ctx.rect(margin + col * cellW, margin + row * cellH, cellW, cellH)
		    ctx.fill()
      		    }
    		 }
            }

	    // Data points — colored by ground truth label
            for (var i = 0; i < pts.length; i++) {
            	var p = pts[i]
                var px = margin + (p.x - xMin) / (xMax - xMin) * (width  - 2 * margin)
		var py = margin + (1.0 - (p.y - yMin) / (yMax - yMin)) * (height - 2 * margin)

                ctx.fillStyle = p.label === 0 ? "blue" : "red"
                ctx.beginPath()
                ctx.arc(px, py, 4, 0, 2 * Math.PI)
                ctx.fill()
            }

	    // Legend
	    var legendX = margin
	    var legendY = height - margin / 2

	    ctx.fillStyle = "blue"
	    ctx.beginPath()
	    ctx.arc(legendX, legendY, 5, 0, 2 * Math.PI)
	    ctx.fill()
	    ctx.fillStyle = "white"
	    ctx.font = "12px monospace"
	    ctx.fillText("Class 0", legendX + 10, legendY + 4)

	    ctx.fillStyle = "red"
	    ctx.beginPath()
	    ctx.arc(legendX + 80, legendY, 5, 0, 2 * Math.PI)
	    ctx.fill()
	    ctx.fillStyle = "white"
	    ctx.fillText("Class 1", legendX + 90, legendY + 4)

            // Cursor toolti. Data coordinates and predicted class color
            if (cursorClass >= 0) {
	       var text = "(" + cursorX.toFixed(2) + ", " + cursorY.toFixed(2) + ")"
	       var textWidth = ctx.measureText(text).width
	       var textHeight = 16 // approximation pour la hauteur, Canvas n'a pas de méthode directe
	       var padding = 8
	       var boxW = textWidth + padding
	       var boxH = textHeight + padding

	       // Position tooltip to stay within widget bounds
	       var labelX = cursorScreenX < width  / 2 ? cursorScreenX + 10 : cursorScreenX - boxW - 10
	       var labelY = cursorScreenY < height / 2 ? cursorScreenY + 10 : cursorScreenY - boxH - 10
	       
               ctx.fillStyle = cursorClass === 0 ? "blue" : "red"
               ctx.fillRect(labelX, labelY, boxW, boxH)
	       ctx.strokeStyle = "white"
    	       ctx.lineWidth = 1
    	       ctx.strokeRect(labelX, labelY, boxW, boxH)
               ctx.fillStyle = "white"
               ctx.font = "12px monospace"
               ctx.fillText(text, labelX + padding/2, labelY + textHeight)
            }
	}
     }
     
     MouseArea {
        anchors.fill: parent
        hoverEnabled: true  
	cursorShape: Qt.CrossCursor

        onPositionChanged: {
            // Convert screen coordinates to data coordinates
            var margin = 40
            var dataX = scatterData.xMin + (mouseX - margin) / (width - 2 * margin) * (scatterData.xMax - scatterData.xMin)
            var dataY = scatterData.yMin + (1.0 - (mouseY - margin) / (height - 2 * margin)) * (scatterData.yMax - scatterData.yMin)
            
            //  Run inference via C++ backend
            var cls = scatterData.predict(dataX, dataY)
            
            // Store cursor state for onPaint
	    canvas.cursorScreenX = mouseX
	    canvas.cursorScreenY = mouseY
	    canvas.cursorX = dataX
	    canvas.cursorY = dataY
            canvas.cursorClass = cls
            canvas.requestPaint()
        }
    }
}