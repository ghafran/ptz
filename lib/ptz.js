'use strict';

var _ = require('lodash'),
    promise = require('bluebird'),
    ptz = require('../build/Release/ptz'),
    camera = require('./camera');

class PTZ {
    static listDevices(callback) {
        var p = new promise((resolve, reject) => {
            ptz.listDevices((err, devices) => {
                if(err) {
                    if(_.isFunction(callback)) {
                        callback(err);
                    } else {
                        reject(err);
                    }

                } else {
                    if(_.isFunction(callback)) {
                        callback(null, devices);
                    } else {
                        resolve(devices);
                    }
                }
            });
        });
        if(!_.isFunction(callback)) {
            return p;
        }
    }

    static getCamera(options) {
        if(!options) {
            options = {};
        }
        options.vendorId = options.vendorId || 0;
        options.productId = options.productId || 0;
        return new camera(options);
    }
}

module.exports = PTZ;
