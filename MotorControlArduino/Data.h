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
<button style="height: 50px; width: 80px; margin-right: 30px;" onclick="speedMotorForward('x')"> Forward 1 x </button>
<button style="height: 50px; width: 80px; margin-right: 30px;" onclick="speedMotorForward('y')"> Forward 2 y </button>
<button style="height: 50px; width: 80px; margin-right: 30px;" onclick="speedMotorForward('z')"> Forward 3 z </button>
<button style="height: 50px; width: 80px; margin-right: 30px;" onclick="speedMotorForward('a')"> Forward 4 a </button>
<button style="height: 50px; width: 80px; margin-right: 30px;" onclick="speedMotorLinks('a')"> links </button>
<br />
<button style="height: 50px; width: 80px; margin-right: 30px;" onclick="speedMotorBackward('x')"> Backward 1 </button>
<button style="height: 50px; width: 80px; margin-right: 30px;" onclick="speedMotorBackward('y')"> Backward 2 </button>
<button style="height: 50px; width: 80px; margin-right: 30px;" onclick="speedMotorBackward('z')"> Backward 3 </button>
<button style="height: 50px; width: 80px; margin-right: 30px;" onclick="speedMotorBackward('a')"> Backward 4 </button>
<button style="height: 50px; width: 80px; margin-right: 30px;" onclick="speedMotorRechts('a')"> rechts </button>
<br />
<div class="row">
	<div class="columnLateral">
			<div id="joy1Div" style="width:600px;height:600px;margin:50px;border:solid"></div>
			Position X:<input id="joy1PositionX" type="text" /><br />
			Position Y:<input id="joy1PositionY" type="text" /><br />
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
    //receive(parts[0], parts[1]);
  }
}

function disconnect() 
{
    ws.close();
    localStorage.removeItem("host");
}

function stop()
{
	//console.log("stop");
	ws.send("stop");
}

function speedMotorForward(motorNumber){ws.send("M03 "+motorNumber+"500");}
function speedMotorBackward(motorNumber){ws.send("M03 "+motorNumber+"500");}
function speedMotorRechts(motorNumber){ws.send("M03 x500 y500 z-500 a-500");}
function speedMotorLinks(motorNumber){ws.send("M03 x-500 y-500 z500 a500");}


</script>

<script type="text/javascript">

var joy1InputPosX = document.getElementById("joy1PositionX");
var joy1InputPosY = document.getElementById("joy1PositionY");
var joy1X = document.getElementById("joy1X");
var joy1Y = document.getElementById("joy1Y");

var oltX=0;
var oltY=0;

// Create JoyStick object into the DIV 'joy1Div'
var Joy1 = new JoyStick('joy1Div', {}, function(stickData) 
{
  
    if( stickData.x==0&&
        stickData.y==0&&
          oltX!=0&&
          oltY!=0)
    {
      document.getElementById("speedleft").value= "stopp";
      document.getElementById("speedright").value= "stopp";
      console.log("stop from Joystick");
      ws.send("stop joy");
    }
    else if( stickData.x==0&&
        stickData.y==0&&
          oltX==0&&
          oltY==0)
    {
      console.log("nix zu tun");
    }
    else
    {
      //darstellen und speichern
      joy1X.value = stickData.x;
      joy1Y.value = stickData.y;


      //robert
      var rightspeed=parseFloat(stickData.y);
      var leftspeed=parseFloat(stickData.y);

      var c = parseFloat(stickData.y)* (parseFloat(stickData.x)/100);

      if(stickData.y>=0&&stickData.x>=0)
      {
        quadrant=1;
        rightspeed=rightspeed-c;
      } 
      if(stickData.y<0&&stickData.x>=0)
      {
        quadrant=2;
        rightspeed=rightspeed-c;
      }
      if(stickData.y<0&&stickData.x<=0)
      {
        quadrant=3;
        leftspeed=leftspeed+c;
      }
      if(stickData.y>=0&&stickData.x<0)
      {
        quadrant=4;
        leftspeed=leftspeed+c;
      }

      if(leftspeed!=0)
      {
        if(leftspeed>0)
        {
          leftspeed = 1000 - (leftspeed * 8);
        }
        else
        {
          leftspeed = -(1000 + (leftspeed * 8));      
        }
      }  

      if(rightspeed!=0)
      {
        if(rightspeed>0)
        {
          rightspeed = 1000 - (rightspeed * 8);
        }
        else
        {
          rightspeed = -(1000 + (rightspeed * 8));  
        } 
      }
      joy1InputPosX.value=c;
      document.getElementById("speedright").value= parseInt(rightspeed);
      document.getElementById("speedleft").value= parseInt(leftspeed);

      ws.send("M03 y"+parseInt(leftspeed)+" a"+parseInt(leftspeed)+" z"+parseInt(rightspeed)+" x"+parseInt(rightspeed));
    }
    


    joy1InputPosX.value = stickData.x;
    joy1InputPosY.value = stickData.y;

    //darstellen und speichern
    oltX = stickData.x;
    oltY = stickData.y;



});

		</script>
</body>
</html>

)rawliteral";
