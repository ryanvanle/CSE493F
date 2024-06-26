"use strict";

let pHtmlMsg;
let serialOptions = { baudRate: 115200  };
let serial;

let previousControllerValues = {};
let previousRawValue = null;

let currentRawValue;
let controllerValues = {};

let backgroundImage;
let xCurrentPosition = 0;

let cursor;

let previousStrokes = [];

let shakeCount = 0;
let currentMode = 1;

let modes = [
  "draw",
  "color",
  "size",
  "clear",
  "shape",
]

let currentShape = 0;
let shapes = [
  "circle",
  "square",
  "triangle"
]


function setup() {
  createCanvas(window.innerWidth, window.innerHeight-50);

  // Setup Web Serial using serial.js
  serial = new Serial();
  serial.on(SerialEvents.CONNECTION_OPENED, onSerialConnectionOpened);
  serial.on(SerialEvents.CONNECTION_CLOSED, onSerialConnectionClosed);
  serial.on(SerialEvents.DATA_RECEIVED, onSerialDataReceived);
  serial.on(SerialEvents.ERROR_OCCURRED, onSerialErrorOccurred);

  // serial.on(SerialEvents.DATA_RECEIVED, onSerialDataReceived);


  // If we have previously approved ports, attempt to connect with them
  // serial.autoConnectAndOpenPreviouslyApprovedPort(serialOptions);

  // Add in a lil <p> element to provide messages. This is optional
  id("data").textContent = "Click anywhere on this page to open the serial connection dialog"
  id("mode").textContent = "Connecting Mode"


  cursor = {
    x: width/2,
    y: height/2,
    size: 100,
    color: [50, 75, 25],
    shape: "circle"
  }


  colorMode(HSL);
}

function draw() {
  background(backgroundImage);
  // circle(width/2, height/2, 1)


  drawPreviousStrokes();
  drawCursor();
}


function drawCursor() {


  stroke("black");

  if (currentMode == 1) {
    stroke(cursor.color[0], cursor.color[1], cursor.color[2]);
    strokeWeight(10);

  } else {
    strokeWeight(5);

  }

  noFill();

  drawShape(cursor.shape, cursor);
  // circle(cursor.x, cursor.y, cursor.size);
}

function preload() {
  backgroundImage = loadImage("brick-wall.jpeg");
}


function drawShape(shapeString, currentCursor) {

  let x = currentCursor.x;
  let y = currentCursor.y;
  let size = currentCursor.size;

  let halfSize = Math.floor(size / 2)

  if (shapeString == "circle") {
    circle(x, y, size);
  } else if (shapeString == "square") {
    rect(x-halfSize, y-halfSize, size, size);
  } else if (shapeString == "triangle") {
    let x1 = x - halfSize;
    let y1 = y + halfSize;

    let x2 = x + halfSize;
    let y2 = y + halfSize;

    let x3 = x
    let y3 = y - halfSize;

    triangle(x1,y1,x2,y2,x3,y3);
  }

}


function processData(newData) {

  previousRawValue = currentRawValue;
  currentRawValue = newData;

  let dataArray = currentRawValue.split(",");
  let nameArray = ["x", "y", "z", "xAcceleration", "yAcceleration", "zAcceleration", "buttonPressed"]

  if (dataArray.length != nameArray.length) console.error("dataArray != nameArray in processData", dataArray, nameArray);

  previousControllerValues = structuredClone(controllerValues);
  controllerValues = {};
  for (let i = 0; i < dataArray.length; i++) {
    controllerValues[nameArray[i]] = Number(dataArray[i]);
  }

  let isShaking = checkShaking();
  let isButtonPressed = controllerValues.buttonPressed == 1.00;

  console.log(isButtonPressed)

  id("color").style["background-color"] = `hsl(${cursor.color[0]} ${cursor.color[1]} ${cursor.color[2]})`

  if (isShaking) shakeCount++;

  let hasEnoughShakes = shakeCount > 5;
  if (hasEnoughShakes && !isButtonPressed) {
    shakeCount = 0;
    currentMode = (currentMode + 1) % modes.length;
  } else if (isButtonPressed && currentMode != 4) {
    shakeCount = 0;
    currentMode = 0;
  }


  switchModes();
}

function switchModes() {


  if (currentMode == 0) {
    drawMode();
  }

  else if (currentMode == 1) {
    colorPickerMode();
  }

  else if (currentMode == 2) {
    sizePickerMode();
  }

  else if (currentMode == 3) {
    shapePickerMode();
  }

  else if (currentMode == 4) {
    clearPickerMode();
  }

}


function drawMode() {
  id("mode").textContent = "Draw Mode";

  updateCursorPosition();
  drawCursor();
  if (controllerValues.buttonPressed == 1.00) {
    drawAtCursor();
  }

}

function colorPickerMode() {
  id("mode").textContent = "Color Picker Mode";

  let tiltPosition = Math.floor(controllerValues.zAcceleration);

  let newHSLValues = getNewHSLValues(structuredClone(cursor.color), tiltPosition);

  cursor.color = newHSLValues;

  serialWriteLight(newHSLValues);



  id("color").style["background-color"] = `hsl(${cursor.color[0]} ${cursor.color[1]} ${cursor.color[2]})`

}


async function serialWriteLight(HSLColors) {

  if (serial.isOpen()) {

    let normalizedHue = map(HSLColors[0], 0.0, 360.0, 0.0, 1.0);
    let stringOutput = `${normalizedHue},${HSLColors[1]},${HSLColors[2]}`;
    console.log("sends", stringOutput);

    serial.writeLine(stringOutput);
  }
}





function sizePickerMode() {
  id("mode").textContent = "Change Brush Size Mode"
  let tiltPosition = Math.floor(controllerValues.zAcceleration);
  let newCursorSize = getNewCursorSize(cursor.size, tiltPosition);
  cursor.size = newCursorSize;
}

function shapePickerMode() {
  id("mode").textContent = "Change Shape Mode";
  let tiltPosition = Math.floor(controllerValues.zAcceleration);
  let newShape = getNewShape(tiltPosition);
  cursor.shape = newShape;
}


function clearPickerMode() {
  id("mode").textContent = "Clear Canvas Mode, on press to clear";

  let isButtonPressed = controllerValues.buttonPressed == 1.00;

  if (isButtonPressed) {
    previousStrokes = [];
    currentMode = 0;
  }

}


function getNewShape(tiltPosition) {

  if (-9 <= tiltPosition && tiltPosition <= -4) {
    return shapes[1];
  } else if (-3 <= tiltPosition && tiltPosition <= 4) {
    return shapes[0];
  } else if (5 <= tiltPosition && tiltPosition <= 10) {
    return shapes[2];
  }


}


function getNewCursorSize(currentSize, incrementAmount) {
  let initialSize = currentSize
  let isNegative = incrementAmount < 0;
  let increment = 1;
  if (isNegative) increment = -1;

  for (let i = 0; i < Math.abs(incrementAmount); i++) {

    initialSize += increment;

    if (initialSize < 10) {
      initialSize = 10;
    } else if (initialSize > 100) {
      initialSize = 100;
    }


  }
  return initialSize;

}


function getNewHSLValues(initialColors, incrementAmount) {


  let isNegative = incrementAmount < 0;
  let increment = 1;
  if (isNegative) increment = -1;

  for (let i = 0; i < Math.abs(incrementAmount); i++) {

    initialColors[0] += increment;

    if (initialColors[0] < 0) {
      initialColors[0] = 0;
    } else if (initialColors[0] > 360) {
      initialColors[0] = 360;
    }


  }


  return initialColors;

}


function checkShaking() {
  let currentX = controllerValues.xAcceleration;
  let previousX = previousControllerValues.xAcceleration;

  return Math.abs(currentX - previousX) > 25;


}


function drawAtCursor() {
  previousStrokes.push(structuredClone(cursor));
}


function updateCursorPosition() {

  // x is up and down
  // z is forward and back
  // y is left to right
  let xAcceleration = controllerValues.xAcceleration;
  let yAcceleration = controllerValues.yAcceleration;
  let zAcceleration = controllerValues.zAcceleration;
  // console.log(xAcceleration, yAcceleration, zAcceleration);

  processCoordinate("y", zAcceleration); // removes gravity vector of 9.81
  processCoordinate("x", yAcceleration);
}

function drawPreviousStrokes() {

  for (let currentCursor of previousStrokes) {
    noStroke();
    fill(currentCursor.color[0], currentCursor.color[1], currentCursor.color[2]);
    drawShape(currentCursor.shape, currentCursor);
  }

}


function processCoordinate(cursorField, acceleration) {

  let cursorValue = cursor[cursorField];

  let lowerBound;
  let higherBound;

  // have to do this or it'll crash for some reason even though its initialized properly?

  if (cursorField === "x") {
    lowerBound = 0;
    higherBound = width - cursor.size;
  } else if (cursorField === "y") {
    lowerBound = 0;
    higherBound = height - cursor.size;
  } else {
    lowerBound = 10;
    higherBound = 100;
  }



  // console.log(cursorValue, height, width)


  // let cursorBounds = cursorBounds[cursorField]
  let increment = acceleration * 3;
  cursorValue += increment




  let isWithinMovementThreshold = Math.abs(acceleration) > 1;
  if (!isWithinMovementThreshold) return;
  if (!(lowerBound <= cursorValue && cursorValue <= higherBound)) return;
  cursor[cursorField] += increment
}

function isBoundsCheck(value, bounds) {
  if (bounds.length != 2) console.error("bounds array is not proper interval", bounds);
  return bounds[0] <= value <= bounds[1];
}





/**
 * Callback function by serial.js when there is an error on web serial
 *
 * @param {} eventSender
 */
 function onSerialErrorOccurred(eventSender, error) {
  console.log("onSerialErrorOccurred", error);
  id("data").textContent = error;
}

/**
 * Callback function by serial.js when web serial connection is opened
 *
 * @param {} eventSender
 */
function onSerialConnectionOpened(eventSender) {
  console.log("onSerialConnectionOpened");
  id("data").textContent = "Serial connection opened successfully";
}

/**
 * Callback function by serial.js when web serial connection is closed
 *
 * @param {} eventSender
 */
function onSerialConnectionClosed(eventSender) {
  console.log("onSerialConnectionClosed");
  pHtmlMsg.html("onSerialConnectionClosed");
}

/**
 * Callback function serial.js when new web serial data is received
 *
 * @param {*} eventSender
 * @param {String} newData new data received over serial
 */
function onSerialDataReceived(eventSender, newData) {
  // console.log("onSerialDataReceived", newData);
  id("data").textContent = "onSerialDataReceived: " + newData;

  if (newData.startsWith("#")) return;
  processData(newData);
}

/**
 * Called automatically by the browser through p5.js when mouse clicked
 */
function mouseClicked() {
  if (!serial.isOpen()) {
    serial.connectAndOpen(null, serialOptions);
  }
}


/** ------------------------------ Helper Functions  ------------------------------ */
/**
 * Note: You may use these in your code, but remember that your code should not have
 * unused functions. Remove this comment in your own code.
 */

/**
 * Returns the element that has the ID attribute with the specified value.
 * @param {string} idName - element ID
 * @returns {object} DOM object associated with id.
 */
function id(idName) {
  return document.getElementById(idName);
}

/**
 * Returns the first element that matches the given CSS selector.
 * @param {string} selector - CSS query selector.
 * @returns {object} The first DOM object matching the query.
 */
function qs(selector) {
  return document.querySelector(selector);
}

/**
 * Returns the array of elements that match the given CSS selector.
 * @param {string} selector - CSS query selector
 * @returns {object[]} array of DOM objects matching the query.
 */
function qsa(selector) {
  return document.querySelectorAll(selector);
}

/**
 * Returns a new element with the given tag name.
 * @param {string} tagName - HTML tag name for new DOM element.
 * @returns {object} New DOM object for given HTML tag.
 */
function gen(tagName) {
  return document.createElement(tagName);
}

