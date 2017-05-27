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

            while (true)
            {
                Console.WriteLine("Enter Command: (q, za)");
                string command = Console.ReadLine();
                if (command == "q") {
                    break;
                }

                if (command == "za")
                {
                    int currentZoom = device.GetZoom();
                    Console.WriteLine("Current Zoom: %d, Min: %d, Max: %d, Step: %d, Default: %d", currentZoom, device.ZoomMin, device.ZoomMax, device.ZoomStep, device.ZoomDefault);
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
            }
        }
    }
}
