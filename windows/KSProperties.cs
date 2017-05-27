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
    class KSProperties
    {
        public enum CameraControlFeature
        {
            KSPROPERTY_CAMERACONTROL_PAN,
            KSPROPERTY_CAMERACONTROL_TILT,
            KSPROPERTY_CAMERACONTROL_ROLL,
            KSPROPERTY_CAMERACONTROL_ZOOM,
            KSPROPERTY_CAMERACONTROL_EXPOSURE,
            KSPROPERTY_CAMERACONTROL_IRIS,
            KSPROPERTY_CAMERACONTROL_FOCUS,
            KSPROPERTY_CAMERACONTROL_SCANMODE,
            KSPROPERTY_CAMERACONTROL_PRIVACY,
            KSPROPERTY_CAMERACONTROL_PANTILT,
            KSPROPERTY_CAMERACONTROL_PAN_RELATIVE,
            KSPROPERTY_CAMERACONTROL_TILT_RELATIVE,
            KSPROPERTY_CAMERACONTROL_ROLL_RELATIVE,
            KSPROPERTY_CAMERACONTROL_ZOOM_RELATIVE,
            KSPROPERTY_CAMERACONTROL_EXPOSURE_RELATIVE,
            KSPROPERTY_CAMERACONTROL_IRIS_RELATIVE,
            KSPROPERTY_CAMERACONTROL_FOCUS_RELATIVE,
            KSPROPERTY_CAMERACONTROL_PANTILT_RELATIVE,
            KSPROPERTY_CAMERACONTROL_FOCAL_LENGTH,
            KSPROPERTY_CAMERACONTROL_AUTO_EXPOSURE_PRIORITY
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct KSPROPERTY
        {
            // size Guid is long + 2 short + 8 byte = 4 longs
            Guid Set;
            [MarshalAs(UnmanagedType.U4)]
            int Id;
            [MarshalAs(UnmanagedType.U4)]
            int Flags;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct KSPROPERTY_CAMERACONTROL_S
        {
            /// <summary> Property Guid </summary>
            public KSPROPERTY Property;
            public KSPROPERTY_CAMERACONTROL Instance;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct KSPROPERTY_CAMERACONTROL
        {
            [MarshalAs(UnmanagedType.I4)]
            public int Value;

            [MarshalAs(UnmanagedType.U4)]
            public int Flags;

            [MarshalAs(UnmanagedType.U4)]
            public int Capabilities;

            [MarshalAs(UnmanagedType.U4)]
            public int Dummy;
            // Dummy added to get a succesful return of the Get, Set function
        }
    }
}