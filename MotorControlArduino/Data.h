#include <Arduino.h>


const char indexhtml[] PROGMEM = R"rawliteral(
<head>
<script>
let StickStatus =
{
    xPosition: 0,
    yPosition: 0,
    x: 0,
    y: 0,
};
var JoyStick = (function(container, parameters, callback)
{
    parameters = parameters || {};
    var title = (typeof parameters.title === "undefined" ? "joystick" : parameters.title),
        width = (typeof parameters.width === "undefined" ? 0 : parameters.width),
        height = (typeof parameters.height === "undefined" ? 0 : parameters.height),
        internalFillColor = (typeof parameters.internalFillColor === "undefined" ? "#00AA00" : parameters.internalFillColor),
        internalLineWidth = (typeof parameters.internalLineWidth === "undefined" ? 2 : parameters.internalLineWidth),
        internalStrokeColor = (typeof parameters.internalStrokeColor === "undefined" ? "#003300" : parameters.internalStrokeColor),
        externalLineWidth = (typeof parameters.externalLineWidth === "undefined" ? 2 : parameters.externalLineWidth),
        externalStrokeColor = (typeof parameters.externalStrokeColor ===  "undefined" ? "#008000" : parameters.externalStrokeColor),
        autoReturnToCenter = (typeof parameters.autoReturnToCenter === "undefined" ? true : parameters.autoReturnToCenter);

    callback = callback || function(StickStatus) {};

    // Create Canvas element and add it in the Container object
    var objContainer = document.getElementById(container);
    
    // Fixing Unable to preventDefault inside passive event listener due to target being treated as passive in Chrome [Thanks to https://github.com/artisticfox8 for this suggestion]
    objContainer.style.touchAction = "none";

    var canvas = document.createElement("canvas");
    canvas.id = title;
    if(width === 0) { width = objContainer.clientWidth; }
    if(height === 0) { height = objContainer.clientHeight; }
    canvas.width = width;
    canvas.height = height;
    objContainer.appendChild(canvas);
    var context=canvas.getContext("2d");

    var pressed = 0; // Bool - 1=Yes - 0=No
    var circumference = 2 * Math.PI;
    var internalRadius = (canvas.width-((canvas.width/2)+10))/2;
    var maxMoveStick = internalRadius + 5;
    var externalRadius = internalRadius + 30;
    var centerX = canvas.width / 2;
    var centerY = canvas.height / 2;
    var directionHorizontalLimitPos = canvas.width / 10;
    var directionHorizontalLimitNeg = directionHorizontalLimitPos * -1;
    var directionVerticalLimitPos = canvas.height / 10;
    var directionVerticalLimitNeg = directionVerticalLimitPos * -1;
    // Used to save current position of stick
    var movedX=centerX;
    var movedY=centerY;

    // Check if the device support the touch or not
    if("ontouchstart" in document.documentElement)
    {
        canvas.addEventListener("touchstart", onTouchStart, false);
        document.addEventListener("touchmove", onTouchMove, false);
        document.addEventListener("touchend", onTouchEnd, false);
    }
    else
    {
        canvas.addEventListener("mousedown", onMouseDown, false);
        document.addEventListener("mousemove", onMouseMove, false);
        document.addEventListener("mouseup", onMouseUp, false);
    }
    // Draw the object
    drawExternal();
    drawInternal();

    function drawExternal()
    {
        context.beginPath();
        context.arc(centerX, centerY, externalRadius, 0, circumference, false);
        context.lineWidth = externalLineWidth;
        context.strokeStyle = externalStrokeColor;
        context.stroke();
    }

    
    function drawInternal()
    {
        context.beginPath();
        if(movedX<internalRadius) { movedX=maxMoveStick; }
        if((movedX+internalRadius) > canvas.width) { movedX = canvas.width-(maxMoveStick); }
        if(movedY<internalRadius) { movedY=maxMoveStick; }
        if((movedY+internalRadius) > canvas.height) { movedY = canvas.height-(maxMoveStick); }
        context.arc(movedX, movedY, internalRadius, 0, circumference, false);
        // create radial gradient
        var grd = context.createRadialGradient(centerX, centerY, 5, centerX, centerY, 200);
        // Light color
        grd.addColorStop(0, internalFillColor);
        // Dark color
        grd.addColorStop(1, internalStrokeColor);
        context.fillStyle = grd;
        context.fill();
        context.lineWidth = internalLineWidth;
        context.strokeStyle = internalStrokeColor;
        context.stroke();
    }

    
    function onTouchStart(event) 
    {
        pressed = 1;
    }

    function onTouchMove(event)
    {
        if(pressed === 1 && event.targetTouches[0].target === canvas)
        {
            movedX = event.targetTouches[0].pageX;
            movedY = event.targetTouches[0].pageY;
            // Manage offset
            if(canvas.offsetParent.tagName.toUpperCase() === "BODY")
            {
                movedX -= canvas.offsetLeft;
                movedY -= canvas.offsetTop;
            }
            else
            {
                movedX -= canvas.offsetParent.offsetLeft;
                movedY -= canvas.offsetParent.offsetTop;
            }
            // Delete canvas
            context.clearRect(0, 0, canvas.width, canvas.height);
            // Redraw object
            drawExternal();
            drawInternal();

            // Set attribute of callback
            StickStatus.xPosition = movedX;
            StickStatus.yPosition = movedY;
            StickStatus.x = (100*((movedX - centerX)/maxMoveStick)).toFixed();
            StickStatus.y = ((100*((movedY - centerY)/maxMoveStick))*-1).toFixed();
            callback(StickStatus);
        }
    } 

    function onTouchEnd(event) 
    {
        pressed = 0;
        // If required reset position store variable
        if(autoReturnToCenter)
        {
            movedX = centerX;
            movedY = centerY;
        }
        // Delete canvas
        context.clearRect(0, 0, canvas.width, canvas.height);
        // Redraw object
        drawExternal();
        drawInternal();

        // Set attribute of callback
        StickStatus.xPosition = movedX;
        StickStatus.yPosition = movedY;
        StickStatus.x = (100*((movedX - centerX)/maxMoveStick)).toFixed();
        StickStatus.y = ((100*((movedY - centerY)/maxMoveStick))*-1).toFixed();
        callback(StickStatus);
    }

    
    function onMouseDown(event) 
    {
        pressed = 1;
    }

    function onMouseMove(event) 
    {
        if(pressed === 1)
        {
            movedX = event.pageX;
            movedY = event.pageY;
            // Manage offset
            if(canvas.offsetParent.tagName.toUpperCase() === "BODY")
            {
                movedX -= canvas.offsetLeft;
                movedY -= canvas.offsetTop;
            }
            else
            {
                movedX -= canvas.offsetParent.offsetLeft;
                movedY -= canvas.offsetParent.offsetTop;
            }
            // Delete canvas
            context.clearRect(0, 0, canvas.width, canvas.height);
            // Redraw object
            drawExternal();
            drawInternal();

            // Set attribute of callback
            StickStatus.xPosition = movedX;
            StickStatus.yPosition = movedY;
            StickStatus.x = (100*((movedX - centerX)/maxMoveStick)).toFixed();
            StickStatus.y = ((100*((movedY - centerY)/maxMoveStick))*-1).toFixed();
            callback(StickStatus);
        }
    }

    function onMouseUp(event) 
    {
        pressed = 0;
        // If required reset position store variable
        if(autoReturnToCenter)
        {
            movedX = centerX;
            movedY = centerY;
        }
        // Delete canvas
        context.clearRect(0, 0, canvas.width, canvas.height);
        // Redraw object
        drawExternal();
        drawInternal();

        // Set attribute of callback
        StickStatus.xPosition = movedX;
        StickStatus.yPosition = movedY;
        StickStatus.x = (100*((movedX - centerX)/maxMoveStick)).toFixed();
        StickStatus.y = ((100*((movedY - centerY)/maxMoveStick))*-1).toFixed();
        callback(StickStatus);
    }
});

</script>

</head>
<body><h1>ESP8266 Motor Steuerung</h1>
<button  style="height: 100px; width: 200px; margin-right: 30px;" onclick="connect()"> Connect </button>
<button  style="height: 100px; width: 200px; margin-right: 30px;" onclick="stop()"> Stopp Motor </button>
<button  style="height: 100px; width: 200px;" onclick="disconnect()"> Disconnect </button>
<br />
<br />
<button style="height: 50px; width: 80px; margin-right: 30px;" onclick="speedMotorForward(1)"> Forward 1 </button>
<button style="height: 50px; width: 80px; margin-right: 30px;" onclick="speedMotorForward(2)"> Forward 2 </button>
<br />
<button style="height: 50px; width: 80px; margin-right: 30px;" onclick="speedMotorBackward(1)"> Backward 1 </button>
<button style="height: 50px; width: 80px; margin-right: 30px;" onclick="speedMotorBackward(2)"> Backward 2 </button>
<br />
<div class="row">
	<div class="columnLateral">
			<div id="joy1Div" style="width:200px;height:200px;margin:50px;border:solid"></div>
			Posizioneewewr X:<input id="joy1PosizioneX" type="text" /><br />
			Posizione Y:<input id="joy1PosizioneY" type="text" /><br />
			X :<input id="joy1X" type="text" /><br />
			Y :<input id="joy1Y" type="text" /><br />
<br />
<br />
			Speed left: <input id="speedleft" type="text" /><br />
			Speed right:<input id="speedright" type="text" /><br />
			Qadrant:<input id="qua" type="text" /><br />
	</div>	
</div>

<script>
function connect() 
{
  let host = window.location.host;
  if (localStorage.getItem("host") != undefined) 
  {
    host = localStorage.getItem("host")
  } else {
    host = window.prompt("Please enter a IP adress", window.location.hostname);
    localStorage.setItem("host", host)
  }

  ws = new WebSocket("ws://" + host + ":81")
  ws.onopen = (event) => {ws.send("hi");};

  ws.onmessage = (event) => 
  {
    const parts = event.data.split(/:(.+)/) 
    console.log(parts);
    receive(parts[0], parts[1]);
  }
}

function disconnect() 
{
    ws.close();
    localStorage.removeItem("host");
}
function stop()
{
	console.log("stop");
	ws.send("stop-1");
}
function test(){ws.send("newspeed-1,200;");}

function speedMotorForward(motorNumber){ws.send("newspeed-"+motorNumber+",200;");}
function speedMotorBackward(motorNumber){ws.send("newspeed-"+motorNumber+",-200;");}

</script>

<script type="text/javascript">

var joy1IinputPosX = document.getElementById("joy1PosizioneX");
var joy1InputPosY = document.getElementById("joy1PosizioneY");
var joy1X = document.getElementById("joy1X");
var joy1Y = document.getElementById("joy1Y");

// Create JoyStick object into the DIV 'joy1Div'
var Joy1 = new JoyStick('joy1Div', {}, function(stickData) 
{
    joy1IinputPosX.value = (stickData.xPosition+10);
    joy1InputPosY.value = stickData.yPosition;
    joy1X.value = stickData.x;
    joy1Y.value = stickData.y;

if(stickData.x==0&&stickData.y==0)
{
  document.getElementById("speedleft").value= "stopp";
  document.getElementById("speedright").value= "stopp";
  ws.send("stop-1");
}
else
{
//quadranden bestimmen in dem ich bin
var quadrant=0;
var testright=0;
var testleft=0;
if(stickData.y>=0&&stickData.x>=0)
{
  quadrant=1;
  testright=parseInt(stickData.y)-parseInt(stickData.x);
  testleft=parseInt(stickData.y);
}
if(stickData.y<0&&stickData.x>=0)
{
  quadrant=2;
  testleft=parseInt(stickData.y);
  testright=-parseInt(stickData.x)+parseInt(stickData.y);
  if(testright<-100)
  {
  quadrant=5;
  var zw2=(testright+100);
  testright=-100-zw2;
  }
}
if(stickData.y<0&&stickData.x<=0)
{
  quadrant=3;
  testright=parseInt(stickData.y);
  testleft=parseInt(stickData.x)+parseInt(stickData.y);
  if(testleft<-100)
  {
    quadrant=6;
    var zw2=(testleft+100);
    testleft=-100-zw2;
  }
}
if(stickData.y>=0&&stickData.x<0)
{
  quadrant=4;
  testright=parseInt(stickData.y);
  testleft=parseInt(stickData.x)+parseInt(stickData.y);
}
 
		

  if(testright!=0)
  {
    if(testright>0)
    {
      testright = 1000 - (testright * 8);
    }
    else
    {
      testright = -(1000 + (testright * 8));      
    }
  }  

  if(testleft!=0)
  {
    if(testleft>0)
    {
      testleft = 1000 - (testleft * 8);
    }
    else
    {
      testleft = -(1000 + (testleft * 8));  
    } 
  }

  document.getElementById("speedright").value= testright;
  document.getElementById("speedleft").value=testleft ;
  
  ws.send("synnewspeed-1,2,"+testright+","+testleft);
}
});

		</script>
</body>
</html>

)rawliteral";
