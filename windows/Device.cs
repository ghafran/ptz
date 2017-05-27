using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;
using System.Text;
using System.Threading;
using DirectShowLib;

namespace PTZ
{
    public class Device
    {
        private readonly Guid PROPSETID_VIDCAP_CAMERACONTROL = new Guid(0xc6e13370, 0x30ac, 0x11d0, 0xa1, 0x8c, 0x00, 0xa0, 0xc9, 0x11, 0x89, 0x56);
        
        private DsDevice _device;
        private IAMCameraControl _camControl;
        private IKsPropertySet _ksPropertySet;

        public bool SupportsAbsoluteZoom { get; set; }
        public bool SupportsRelativeZoom { get; set; }
        public bool SupportsAbsolutePanTilt { get; set; }
        public bool SupportsRelativePanTilt { get; set; }

        public int ZoomMin { get; set; }
        public int ZoomMax { get; set; }
        public int ZoomStep { get; set; }
        public int ZoomDefault { get; set; }

        public int PanMin { get; set; }
        public int PanMax { get; set; }
        public int PanStep { get; set; }
        public int PanDefault { get; set; }

        public int TiltMin { get; set; }
        public int TiltMax { get; set; }
        public int TiltStep { get; set; }
        public int TiltDefault { get; set; }

        private Device(string name)
        {
            var devices = DsDevice.GetDevicesOfCat(FilterCategory.VideoInputDevice);
            var device = devices.Where(d => d.Name == name).FirstOrDefault();

            _device = device;

            if (_device == null) throw new ApplicationException(String.Format("Couldn't find device named {0}!", name));

            IFilterGraph2 graphBuilder = new FilterGraph() as IFilterGraph2;
            IBaseFilter filter = null;
            IMoniker i = _device.Mon as IMoniker;

            graphBuilder.AddSourceFilterForMoniker(i, null, _device.Name, out filter);
            _camControl = filter as IAMCameraControl;
            _ksPropertySet = filter as IKsPropertySet;

            if (_camControl == null) throw new ApplicationException("Couldn't get ICamControl!");
            if (_ksPropertySet == null) throw new ApplicationException("Couldn't get IKsPropertySet!");

            // Get Capabilities
            SupportsAbsoluteZoom = SupportFor(KSProperties.CameraControlFeature.KSPROPERTY_CAMERACONTROL_ZOOM);
            SupportsRelativeZoom = SupportFor(KSProperties.CameraControlFeature.KSPROPERTY_CAMERACONTROL_ZOOM_RELATIVE);
            SupportsAbsolutePanTilt = SupportFor(KSProperties.CameraControlFeature.KSPROPERTY_CAMERACONTROL_PANTILT);

            // Get Absolute Ranges
            InitZoomRanges();
            InitPanRanges();
            InitTiltRanges();
        }

        private bool SupportFor(KSProperties.CameraControlFeature feature)
        {
            KSPropertySupport supported = new KSPropertySupport();
            _ksPropertySet.QuerySupported(PROPSETID_VIDCAP_CAMERACONTROL,(int)feature, out supported);
            return (supported.HasFlag(KSPropertySupport.Set) && supported.HasFlag(KSPropertySupport.Get));
        }

        private void InitZoomRanges()
        {
            int iMin, iMax, iStep, iDefault;
            CameraControlFlags flag;
            _camControl.GetRange(CameraControlProperty.Zoom, out iMin, out iMax, out iStep, out iDefault, out flag);
            ZoomMin = iMin;
            ZoomMax = iMax;
            ZoomDefault = iDefault;
            ZoomStep = iStep;
        }
        private void InitPanRanges()
        {
            int iMin, iMax, iStep, iDefault;
            CameraControlFlags flag;
            _camControl.GetRange(CameraControlProperty.Pan, out iMin, out iMax, out iStep, out iDefault, out flag);
            PanMin = iMin;
            PanMax = iMax;
            PanDefault = iDefault;
            PanStep = iStep;
        }

        private void InitTiltRanges()
        {
            int iMin, iMax, iStep, iDefault;
            CameraControlFlags flag;
            _camControl.GetRange(CameraControlProperty.Tilt, out iMin, out iMax, out iStep, out iDefault, out flag);
            TiltMin = iMin;
            TiltMax = iMax;
            TiltDefault = iDefault;
            TiltStep = iStep;
        }

        public int GetZoom()
        {
            int old = 0;
            CameraControlFlags oldFlags = CameraControlFlags.Manual;
            var e = _camControl.Get(CameraControlProperty.Zoom, out old, out oldFlags);
            return old;
        }

        public int GetPan()
        {
            int old = 0;
            CameraControlFlags oldFlags = CameraControlFlags.Manual;
            var e = _camControl.Get(CameraControlProperty.Pan, out old, out oldFlags);
            return old;
        }

        public int GetTilt()
        {
            int old = 0;
            CameraControlFlags oldFlags = CameraControlFlags.Manual;
            var e = _camControl.Get(CameraControlProperty.Tilt, out old, out oldFlags);
            return old;
        }

        public int AbsoluteZoom(int zoom)
        {
            return _camControl.Set(CameraControlProperty.Zoom, zoom, CameraControlFlags.Manual);
        }

        public int AbsolutePan(int pan)
        {
            return _camControl.Set(CameraControlProperty.Pan, pan, CameraControlFlags.Manual);
        }

        public int AbsoluteTilt(int tilt)
        {
            return _camControl.Set(CameraControlProperty.Tilt, tilt, CameraControlFlags.Manual);
        }

        public int AbsolutePanTilt(int pan, int tilt)
        {
            int res1 = _camControl.Set(CameraControlProperty.Pan, pan, CameraControlFlags.Manual);
            int res2 = _camControl.Set(CameraControlProperty.Tilt, tilt, CameraControlFlags.Manual);
            return res1;
        }

        public static Device GetDevice(string name)
        {
            return new Device(name);
        }

        public static string[] GetDevices()
        {
            var devices = DsDevice.GetDevicesOfCat(FilterCategory.VideoInputDevice);
            return devices.Select(x => x.Name).ToArray();
        }
    }
}