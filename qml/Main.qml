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

	    // constants and backend data
	    var margin = 40
	    var tickInterval  = 1.0 // graduation spacing in data units
	    var labelInterval = 2.0 // label spacing in data units
	    var tickSize      = 4.0 // tick length in pixels
	    var pts = scatterData.points
            var xMin = scatterData.xMin
            var xMax = scatterData.xMax
            var yMin = scatterData.yMin
            var yMax = scatterData.yMax
	    var gridSize = scatterData.gridSize
	    var grid = scatterData.grid

	    // Axis lines through (0, 0)
	    var origin = { x: margin + (0 - xMin) / (xMax - xMin) * (width  - 2 * margin),
               	       	   y: margin + (1.0 - (0 - yMin) / (yMax - yMin)) * (height - 2 * margin) }

	    ctx.strokeStyle = "white"
	    ctx.lineWidth = 1

	    // X axis
	    ctx.beginPath()
	    ctx.moveTo(margin, origin.y)
	    ctx.lineTo(width - margin, origin.y)
	    ctx.stroke()

	    // Y axis
	    ctx.beginPath()
	    ctx.moveTo(origin.x, margin)
	    ctx.lineTo(origin.x, height - margin)
	    ctx.stroke()

	    // Ticks, grid lines and labels — X axis
	    ctx.font = "10px monospace"
	    for (var v = xMin; v <= xMax; v += tickInterval) {
    	    	var sx = margin + (v - xMin) / (xMax - xMin) * (width - 2 * margin)

    		// Vertical grid line
    		ctx.strokeStyle = "rgba(255, 255, 255, 0.12)"
   		ctx.beginPath()
    		ctx.moveTo(sx, margin)
    		ctx.lineTo(sx, height - margin)
    		ctx.stroke()

    		// Tick
    		ctx.strokeStyle = "white"
    		ctx.beginPath()
    		ctx.moveTo(sx, origin.y - tickSize)
   		ctx.lineTo(sx, origin.y + tickSize)
   		ctx.stroke()

    		// Label
   		if (Math.abs(v % labelInterval) < 0.01 && Math.abs(v) > 0.01) {
        	   var label = v.toFixed(1)
        	   var labelW = ctx.measureText(label).width
        	   ctx.fillStyle = "white"
        	   ctx.fillText(label, sx - labelW / 2, origin.y + tickSize + 12)
    		}
	    }

	    // Ticks, grid lines and labels — Y axis
	    for (var vy = yMin; vy <= yMax; vy += tickInterval) {
   	       var sy = margin + (1.0 - (vy - yMin) / (yMax - yMin)) * (height - 2 * margin)

	       // Horizontal grid line
	       ctx.strokeStyle = "rgba(255, 255, 255, 0.12)"
    	       ctx.beginPath()
    	       ctx.moveTo(margin, sy)
    	       ctx.lineTo(width - margin, sy)
    	       ctx.stroke()

	       // Tick
    	       ctx.strokeStyle = "white"
    	       ctx.beginPath()
    	       ctx.moveTo(origin.x - tickSize, sy)
    	       ctx.lineTo(origin.x + tickSize, sy)
    	       ctx.stroke()

    	       // Label
    	       if (Math.abs(vy % labelInterval) < 0.01 && Math.abs(vy) > 0.01) {
               	  var labelY = vy.toFixed(1)
        	  var labelWY = ctx.measureText(labelY).width
        	  ctx.fillStyle = "white"
        	  ctx.fillText(labelY, origin.x - labelWY - tickSize - 2, sy + 4)
    		}
	     }

	     // Origin label
	     ctx.fillStyle = "white"
	     ctx.fillText("0", origin.x + tickSize + 2, origin.y + 12)

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