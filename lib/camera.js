'use strict';

var _ = require('lodash'),
    promise = require('bluebird'),
    ptz = require('../build/Release/ptz');

class Camera {
    constructor(options) {
        this.vendorId = options.vendorId;
        this.productId = options.productId;
    }

    promisify(functionName, input) {
        if(!input) {
            input = {};
        }
        input.vendorId = this.vendorId;
        input.productId = this.productId;
        return new promise((resolve, reject) => {
            ptz[functionName](input, (err, result) => {
                if(err) {
                    reject(new Error(err));
                } else {
                    resolve(result);
                }
            });
        });
    }

    callbackify(functionName, input, callback) {
        if(!input) {
            input = {};
        }
        input.vendorId = this.vendorId;
        input.productId = this.productId;
        ptz[functionName](input, (err, result) => {
            if(err) {
                callback(new Error(err));
            } else {
                callback(null, result);
            }
        });
    }

    getCapabilities(callback) {
        if(_.isFunction(callback)) {
            this.callbackify('getCapabilities', {}, callback);
        } else {
            return this.promisify('getCapabilities');
        }
    }

    getAbsoluteZoom(callback) {
        if(_.isFunction(callback)) {
            this.callbackify('getAbsoluteZoom', {}, callback);
        } else {
            return this.promisify('getAbsoluteZoom');
        }
    }

    absoluteZoom(zoom, callback) {
        if(_.isFunction(callback)) {
            this.callbackify('absoluteZoom', {
                zoom
            }, callback);
        } else {
            return this.promisify('absoluteZoom', {
                zoom
            });
        }
    }

    getRelativeZoom(callback) {
        if(_.isFunction(callback)) {
            this.callbackify('getRelativeZoom', {}, callback);
        } else {
            return this.promisify('getRelativeZoom');
        }
    }

    relativeZoomIn(speed, callback) {
        if(_.isFunction(callback)) {
            this.callbackify('relativeZoom', {
                direction: 1,
                speed
            }, callback);
        } else {
            return this.promisify('relativeZoom', {
                direction: 1,
                speed
            });
        }
    }

    relativeZoomOut(speed, callback) {
        if(_.isFunction(callback)) {
            this.callbackify('relativeZoom', {
                direction: -1,
                speed
            }, callback);
        } else {
            return this.promisify('relativeZoom', {
                direction: -1,
                speed
            });
        }
    }

    relativeZoomStop(speed, callback) {
        if(_.isFunction(callback)) {
            this.callbackify('relativeZoom', {
                direction: 0,
                speed
            }, callback);
        } else {
            return this.promisify('relativeZoom', {
                direction: 0,
                speed
            });
        }
    }

    getAbsolutePanTilt(callback) {
        if(_.isFunction(callback)) {
            this.callbackify('getAbsolutePanTilt', {}, callback);
        } else {
            return this.promisify('getAbsolutePanTilt');
        }
    }

    absolutePanTilt(pan, tilt, callback) {
        if(_.isFunction(callback)) {
            this.callbackify('absolutePanTilt', {
                pan,
                tilt
            }, callback);
        } else {
            return this.promisify('absolutePanTilt', {
                pan,
                tilt
            });
        }
    }

    getRelativePanTilt(callback) {
        if(_.isFunction(callback)) {
            this.callbackify('getRelativePanTilt', {}, callback);
        } else {
            return this.promisify('getRelativePanTilt');
        }
    }

    relativePanTilt(panDirection, panSpeed, tiltDirection, tiltSpeed, callback) {
        if(_.isFunction(callback)) {
            this.callbackify('relativePanTilt', {
                panDirection,
                panSpeed,
                tiltDirection,
                tiltSpeed
            }, callback);
        } else {
            return this.promisify('relativePanTilt', {
                panDirection,
                panSpeed,
                tiltDirection,
                tiltSpeed
            });
        }
    }
}

module.exports = Camera;
