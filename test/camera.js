'use strict';

var expect = require('chai').expect,
    ptz = require('../lib/ptz');

var _camera;
var _capabilities;
describe('camera', () => {

    before(() => {
        _camera = ptz.getCamera();
        return _camera.getCapabilities().then((capabilities) => {
            _capabilities = capabilities;
        });
    });

    it('got camera', () => {
        expect(_capabilities).to.exist;
    });

    it('getCapabilities promise', () => {
        return _camera.getCapabilities().then((capabilities) => {
            expect(capabilities).to.exist;
            expect(capabilities).to.have.property('absoluteZoom');
            expect(capabilities).to.have.property('relativeZoom');
            expect(capabilities).to.have.property('absolutePanTilt');
            expect(capabilities).to.have.property('relativePanTilt');
            expect(capabilities).to.have.property('absoluteRoll');
            expect(capabilities).to.have.property('relativeRoll');
        });
    });

    it('getCapabilities callback', () => {
        _camera.getCapabilities(function (err, capabilities) {
            expect(err).to.not.exist;
            expect(capabilities).to.exist;
            expect(capabilities).to.have.property('absoluteZoom');
            expect(capabilities).to.have.property('relativeZoom');
            expect(capabilities).to.have.property('absolutePanTilt');
            expect(capabilities).to.have.property('relativePanTilt');
            expect(capabilities).to.have.property('absoluteRoll');
            expect(capabilities).to.have.property('relativeRoll');
        });
    });

    it('getAbsoluteZoom', () => {
        if(_capabilities.absoluteZoom) {
            return _camera.getAbsoluteZoom().then((zoomInfo) => {
                expect(zoomInfo).to.exist;
                expect(zoomInfo).to.have.property('min');
                expect(zoomInfo).to.have.property('max');
                expect(zoomInfo).to.have.property('resolution');
                expect(zoomInfo).to.have.property('current');
                expect(zoomInfo).to.have.property('default');
            });
        }
    });

    it('absoluteZoom', () => {
        if(_capabilities.absoluteZoom) {
            return _camera.absoluteZoom(0);
        }
    });

    it('getRelativeZoom', () => {
        if(_capabilities.relativeZoom) {
            return _camera.getRelativeZoom().then((zoomInfo) => {
                expect(zoomInfo).to.exist;
                expect(zoomInfo).to.have.property('direction');
                expect(zoomInfo).to.have.property('digitalZoom');
                expect(zoomInfo).to.have.property('minSpeed');
                expect(zoomInfo).to.have.property('maxSpeed');
                expect(zoomInfo).to.have.property('resolutionSpeed');
                expect(zoomInfo).to.have.property('currentSpeed');
            });
        }
    });

    it('relativeZoomIn', () => {
        if(_capabilities.relativeZoom) {
            return _camera.relativeZoomIn(1);
        }
    });

    it('relativeZoomOut', () => {
        if(_capabilities.relativeZoom) {
            return _camera.relativeZoomOut(1);
        }
    });

    it('relativeZoomStop', () => {
        if(_capabilities.relativeZoom) {
            return _camera.relativeZoomStop(1);
        }
    });

    it('getAbsolutePanTilt', () => {
        if(_capabilities.absolutePanTilt) {
            return _camera.getAbsolutePanTilt().then((panTilt) => {
                expect(panTilt).to.exist;
                expect(panTilt).to.have.property('minPan');
                expect(panTilt).to.have.property('minTilt');
                expect(panTilt).to.have.property('maxPan');
                expect(panTilt).to.have.property('maxTilt');
                expect(panTilt).to.have.property('resolutionPan');
                expect(panTilt).to.have.property('resolutionTilt');
                expect(panTilt).to.have.property('currentPan');
                expect(panTilt).to.have.property('currentTilt');
                expect(panTilt).to.have.property('defaultPan');
                expect(panTilt).to.have.property('defaultTilt');
            });
        }
    });

    it('absolutePanTilt', () => {
        if(_capabilities.absolutePanTilt) {
            return _camera.absolutePanTilt(0, 0);
        }
    });

    it('getRelativePanTilt', () => {
        if(_capabilities.relativePanTilt) {
            return _camera.getRelativePanTilt().then((zoomInfo) => {
                expect(zoomInfo).to.exist;
                expect(zoomInfo).to.have.property('panDirection');
                expect(zoomInfo).to.have.property('tiltDirection');
                expect(zoomInfo).to.have.property('minPanSpeed');
                expect(zoomInfo).to.have.property('minTiltSpeed');
                expect(zoomInfo).to.have.property('maxPanSpeed');
                expect(zoomInfo).to.have.property('maxTiltSpeed');
                expect(zoomInfo).to.have.property('resolutionPanSpeed');
                expect(zoomInfo).to.have.property('resolutionTiltSpeed');
                expect(zoomInfo).to.have.property('defaultPanSpeed');
                expect(zoomInfo).to.have.property('defaultTiltSpeed');
                expect(zoomInfo).to.have.property('currentPanSpeed');
                expect(zoomInfo).to.have.property('currentTiltSpeed');
            });
        }
    });

    it('relativePanTilt', () => {
        if(_capabilities.relativePanTilt) {
            return _camera.relativePanTilt(0, 1, 0, 1);
        }
    });
});
