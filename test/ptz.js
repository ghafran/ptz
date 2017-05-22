'use strict';

var expect = require('chai').expect,
    ptz = require('../../lib/ptz');

describe('ptz', () => {

    it('listDevices promise', () => {
        return ptz.listDevices().then((devices) => {
            expect(devices).to.exist;
            expect(devices).to.be.instanceof(Array);
        });
    });

    it('listDevices callback', (done) => {
        ptz.listDevices((err, devices) => {
            expect(err).to.not.exist;
            expect(devices).to.exist;
            expect(devices).to.be.instanceof(Array);
            done();
        });
    });

    it('getCamera', () => {
        var camera = ptz.getCamera();
        expect(camera).to.exist;
    });
});
