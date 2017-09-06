[![Build Status](https://secure.travis-ci.org/ghafran/ptz.png)](http://travis-ci.org/ghafran/ptz)

# PTZ

A camera control module for pan/tilt/zoom operations. Oh yea, in nodejs.

## Installing

Install dependencies first (see below).
```
npm install ptz
```

# Goals

Works with any plug & play UVC camera to perform pan/tilt/zoom operations.

# Reference

```
var ptz = require('ptz');
```

# List Devices

```
ptz.listDevices(function(err, devices){
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

# Get Camera Instance

Before any operations can be performed, get a camera instance using the code below. The camera details can be retrieved from **listDevices()**. Then you can call **getCapabilities()** to see what operations are supported by the camera.

```
var camera = ptz.getCamera({
    vendorId: 0, // use 0 to get the first camera
    productId: 0 // use 0 to get the first camera
});

// perform ptz operations here, or get camera ptz capabilities
camera.getCapabilities().then(function(capabilities){
    console.log(capabilities);
});
```

# Pan/Tilt/Zoom Operations

Let's get to the fun part.

## Zoom

There are two methods of performing zoom operations, **absolute** and **relative**.

Absolute allows you to specify the exact zoom level to go to and the camera goes there regardless of current zoom level.

Relative lets you send a zoom-in or zoom-out commands, then the camera will beginning changing its zoom level towards that direction. Once its where you want it to be, send a stop command.

### Absolute Zoom

```
// Get the camera's zoom info so you know the min, max, and resolution values for zoom level.
camera.getAbsoluteZoom().then(function(zoom){
    console.log(zoom);

    var zoomLevel = 10; // must be between min and max and divisible by resolution, without remainder.
    camera.absoluteZoom(zoomLevel).then(function(){
        console.log('absolute zoom command sent!');
    });
});
```

### Relative Zoom

```
// Get the camera's zoom info so you know the min, max, and resolution values for speed.
camera.getRelativeZoom().then(function(zoom){
    console.log(zoom);

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
});

```

## Pan/Tilt

There are two methods of performing pan/tilt operations, **absolute** and **relative**. For some reason both pan and tilt go hand-in-hand in the UVC standards specification. We cannot change these two independently. We must provide both values together when sending commands to the device. They are like stuck together, like, well, glue.

Absolute allows you to specify the exact pan and tilt location to go to and the camera goes there regardless of current location.

Relative lets you send up, down, left, right commands, then the camera will beginning changing its location towards those directions. Once its where you want it to be, send a stop command.

Also, pan and tilt settings are in arc second units. 1 arc second is 1/3600 of a degree. Values range from **–180*3600** arc second to **+180*3600** arc second. Positive values are clockwise from the origin (the camera rotates clockwise when viewed from above), and negative values are counterclockwise from the origin. For tilt, positive values point the imaging plane up, and negative values point the imaging plane down.


### Absolute Pan/Tilt

```
// Get the camera's pan and tilt info so you know the min, max, and resolution values for both.
camera.getAbsolutePanTilt().then(function(panTilt){
    console.log(panTilt);

    var pan = 90 * 3600;
    var tilt = 90 * 3600;
    camera.absolutePanTilt(pan, tilt).then(function(){
        console.log('absolute pan/tilt command sent!');
    });
});
```

### Relative Pan/Tilt

```
// Get the camera's pan and tilt info so you know the min, max, and resolution values for both speeds.
camera.getRelativePanTilt().then(function(panTilt){
    console.log(panTilt);

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
});
```

# Install dependencies
Need to install `libuvc` on the machine first.

## Mac
```
brew update
brew install libuvc
```

## Linux
```
sudo apt-get install -y build-essential libudev-dev libtool
sudo apt-get install -y libusb-dev
cd ~
git clone https://github.com/ktossell/libuvc
cd libuvc
mkdir build
cd build
cmake ..
make && sudo make install
```

## Windows
Not supported


# As A Side Note

Just as a side node, on every operation sent to the camera, a USB connection is opened and immediately closed. This is to minimize conflicts with the USB device and keep things easy to manage. We don't need to figure out how to keep USB connections open and maintain them.

Don't always trust the min max values from the cameras. For pan/tilt the min should always be (-180 * 3600) and max should be (180 * 3600). If these values are different, test the values before trusting them.
