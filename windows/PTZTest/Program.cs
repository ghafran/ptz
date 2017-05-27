using System;
using System.Runtime.InteropServices.ComTypes;
using System.Runtime.InteropServices;
using System.Threading;
using PTZ;
using System.Configuration;

namespace PTZTest
{
    class Program
    {
        static void Main(string[] args)
        {
            var device = PTZ.Device.GetDevice(ConfigurationManager.AppSettings["DeviceName"]);

            Console.WriteLine("Capabilities: Absolute Zoom: {0}, Relative Zoom: {1}, Absolute PanTilt: {2}, Relative PanTilt: {3}", device.SupportsAbsoluteZoom, device.SupportsRelativeZoom, device.SupportsAbsolutePanTilt, device.SupportsRelativePanTilt);
            while (true)
            {
                Console.WriteLine("Enter Command: (q, z, p, t, pt)");
                string command = Console.ReadLine();
                if (command == "q") {
                    break;
                }

                if (command == "za")
                {
                    int currentZoom = device.GetZoom();
                    Console.WriteLine("Current Zoom: {0}, Min: {1}, Max: {2}, Step: {3}, Default: {4}", currentZoom, device.ZoomMin, device.ZoomMax, device.ZoomStep, device.ZoomDefault);
                    string value = Console.ReadLine();
                    int newvalue = int.Parse(value);
                    int result = device.AbsoluteZoom(newvalue);
                    if(result == 0)
                    {
                        Console.WriteLine("Set Zoom Successfully!");
                    } else
                    {
                        Console.WriteLine("Failed! %d", result);
                    }
                }

                if (command == "pa")
                {
                    int currentPan = device.GetPan();
                    Console.WriteLine("Current Pan: {0}, Min: {1}, Max: {2}, Step: {3}, Default: {4}", currentPan, device.PanMin, device.PanMax, device.PanStep, device.PanDefault);
                    string value = Console.ReadLine();
                    int newvalue = int.Parse(value);
                    int result = device.AbsolutePan(newvalue);
                    if (result == 0)
                    {
                        Console.WriteLine("Set Pan Successfully!");
                    }
                    else
                    {
                        Console.WriteLine("Failed! {0}", result);
                    }
                }

                if (command == "ta")
                {
                    int current = device.GetTilt();
                    Console.WriteLine("Current Tilt: {0}, Min: {1}, Max: {2}, Step: {3}, Default: {4}", current, device.TiltMin, device.TiltMax, device.TiltStep, device.TiltDefault);
                    string value = Console.ReadLine();
                    int newvalue = int.Parse(value);
                    int result = device.AbsoluteTilt(newvalue);
                    if (result == 0)
                    {
                        Console.WriteLine("Set Tilt Successfully!");
                    }
                    else
                    {
                        Console.WriteLine("Failed! {0}", result);
                    }
                }

                if (command == "pt")
                {
                    int currentPan = device.GetPan();
                    Console.WriteLine("Current Pan: {0}, Min: {1}, Max: {2}, Step: {3}, Default: {4}", currentPan, device.PanMin, device.PanMax, device.PanStep, device.PanDefault);
                    string value = Console.ReadLine();
                    int newpan = int.Parse(value);

                    int current = device.GetTilt();
                    Console.WriteLine("Current Tilt: {0}, Min: {1}, Max: {2}, Step: {3}, Default: {4}", current, device.TiltMin, device.TiltMax, device.TiltStep, device.TiltDefault);
                    string value2 = Console.ReadLine();
                    int newtilt = int.Parse(value2);

                    int result = device.AbsolutePanTilt(newpan, newtilt);
                    if (result == 0)
                    {
                        Console.WriteLine("Set Pan/Tilt Successfully!");
                    }
                    else
                    {
                        Console.WriteLine("Failed! {0}", result);
                    }
                }
            }
        }
    }
}
