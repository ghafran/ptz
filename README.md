[![Build Status](https://secure.travis-ci.org/ghafran/ptz.png)](http://travis-ci.org/ghafran/ptz)
# THIS IS STILL BEING DEVELOPED. NOT READY FOR USE YET!
# PTZ
A camera control module for pan/tilt/zoom operations. Oh yea, in nodejs.

## Installing
```
npm install ptz
```

# Goals
Works with any plug & play UVC or VISCA cameras to perform pan/tilt/zoom operations.

# Reference
```
var ptz = require('ptz');
```

# List Devices

```
ptz.listDevices(function(devices){
    devices.forEach(function(device){
        console.log(device.type, device.vendorId, device.productId, device.description);
    });
});
```
Or, using promises. Generally, all operations can be done via callback or promises, whatever floats your boat :)
```
ptz.listDevices().then(function(devices){
    devices.forEach(function(device){
        console.log(device.type, device.vendorId, device.productId, device.description);
    });
});
```

# Open Camera
Before any operations can be performed, open a camera using the code below. The camera details can be retrieved from **listDevices()**. Then you can call **getCapabilities()** to see what operations are supported by the camera.
```
ptz.openCamera({
    vendorId: 0, // use 0 to get the first camera
    productId: 0 // use 0 to get the first camera
    }).then(function(camera){

    // perform ptz operations here
    camera.getCapabilities().then(function(capabilities){
        console.log(capabilities);
    });
});
```

# Close Camera

Always remember to close the camera, otherwise you will not be able to reconnect to it. USB connections are weird like that :(
```
camera.close();
```
Just to make sure, we will always close the camera, ensure this on process exit and crazy errors.
```
function closeCamera(){
    if(camera && camera.isOpen){
        camera.close();
    }
}
process.on('SIGTERM', function () { // ctrl c
    closeCamera();
});

process.on('SIGINT', function () { // ctrl z
    closeCamera();
});

process.on('uncaughtException', function (err) {
    closeCamera();
    process.exit(1);
});
```

# Pan/Tilt/Zoom Operations
Let's get to the fun part.

## Zoom
There are two methods of performing zoom operations, **absolute** and **relative**.

Absolute allows you to specify the exact zoom level to go to and the camera goes there regardless of current zoom level.

Relative lets you send a zoom-in or zoom-out commands, then the camera will beginning changing its zoom level towards that direction. Once its where you want it to be, send a stop command.

### Get Current Zoom Info
```
camera.getZoomInfo().then(function(zoomInfo){
    console.log(zoomInfo);
});
```

### Absolute Zoom
```
// Get the camera's zoom info so you know the min, max, and resolution values for zoom level.
var zoomLevel = 10; // must be between min and max and divisible by resolution, without remainder.
camera.absoluteZoom(zoomLevel).then(function(){
    console.log('absolute zoom command sent!');
});
```

### Relative Zoom
```
// Get the camera's zoom info so you know the min, max, and resolution values for speed.
var speed = 10; // must be between min and max and divisible by resolution, without remainder.
camera.relativeZoomIn(speed).then(function(){
    console.log('relative zoom-in command sent!');
    camera.relativeZoomStop().then(function(){ // usually we would wait for a bit before stopping
        console.log('relative zoom stop command sent!');
        camera.relativeZoomOut(speed).then(function(){
            console.log('relative zoom-out command sent!');
            camera.relativeZoomStop().then(function(){
                console.log('relative zoom stop command sent!');
            });
        });
    });
});
```

## Pan/Tilt
There are two methods of performing pan/tilt operations, **absolute** and **relative**. For some reason both pan and tilt go hand-in-hand in the UVC standards specification. We cannot change these two independently. We must provide both values together when sending commands to the device. They are like stuck together, like, well, glue.

Absolute allows you to specify the exact pan and tilt location to go to and the camera goes there regardless of current location.

Relative lets you send up, down, left, right commands, then the camera will beginning changing its location towards those directions. Once its where you want it to be, send a stop command.

Also, pan and tilt settings are in arc second units. 1 arc second is 1/3600 of a degree. Values range from **–180*3600** arc second to **+180*3600** arc second. Positive values are clockwise from the origin (the camera rotates clockwise when viewed from above), and negative values are counterclockwise from the origin. For tilt, positive values point the imaging plane up, and negative values point the imaging plane down.

### Get Current Pan/Tilt Info
```
camera.getPanTiltInfo().then(function(ptInfo){
    console.log(ptInfo);
});
```

### Absolute Pan/Tilt
```
// Get the camera's pan and tilt info so you know the min, max, and resolution values for both.
var pan = 90 * 3600;
var tilt = 90 * 3600;
camera.absolutePanTilt(pan, tilt).then(function(){
    console.log('absolute pan/tilt command sent!');
});
```

### Relative Pan/Tilt
```
// Get the camera's pan and tilt info so you know the min, max, and resolution values for both speeds.
var pan_speed = 10; // must be between min and max and divisible by resolution, without remainder.
var tilt_speed = 10; // must be between min and max and divisible by resolution, without remainder.
var pan_direction = 1; // 1 right, -1 left, 0 no change
var tilt_direction = 1; // 1 up, -1 down, 0 no change
camera.relativePanTilt(pan_direction, pan_speed, tilt_direction, tilt_speed).then(function(){
    console.log('relative pan/tilt command sent!');
    camera.relativeZoomStop().then(function(){ // usually we would wait for a bit before stopping
        console.log('relative pan/tilt stop command sent!');
    });
});
```

## Roll
There are two methods of performing roll operations, **absolute** and **relative**.

Absolute allows you to specify the exact roll to go to and the camera goes there regardless of current roll location.

Relative lets you send a roll command, then the camera will beginning changing its roll towards that direction. Once its where you want it to be, send a stop command.

The roll specified in degrees. Values range from **–180** to **+180**. Positive values cause a clockwise rotation of the camera along the image viewing axis, and negative values cause a counterclockwise rotation of the camera.

### Get Current Roll Info
```
camera.getRollInfo().then(function(rollInfo){
    console.log(rollInfo);
});
```

### Absolute Roll
```
// Get the camera's roll info so you know the min, max, and resolution values for roll.
var roll = 10; // must be between min and max and divisible by resolution, without remainder.
camera.absoluteRoll(roll).then(function(){
    console.log('absolute roll command sent!');
});
```

### Relative Roll
```
// Get the camera's roll info so you know the min, max, and resolution values for roll speed.
var speed = 10; // must be between min and max and divisible by resolution, without remainder.
camera.relativeRollLeft(speed).then(function(){
    console.log('relative roll left command sent!');
    camera.relativeRollStop().then(function(){ // usually we would wait for a bit before stopping
        console.log('relative roll stop command sent!');
        camera.relativeRollRight(speed).then(function(){
            console.log('relative roll right command sent!');
            camera.relativeRollStop().then(function(){ // usually we would wait for a bit before stopping
                console.log('relative roll stop command sent!');
            });
        });
    });
});
```
