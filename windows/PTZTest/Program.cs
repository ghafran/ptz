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
            var p = PTZDevice.GetDevice(ConfigurationManager.AppSettings["DeviceName"], PTZType.Relative);

            while (true)
            {
                Console.WriteLine("Enter Command: (q, za)");
                string command = Console.ReadLine();
                if (command == "q") {
                    break;
                }

                if (command == "za")
                {
                    
                }

                //if (info.Key == ConsoleKey.LeftArrow)
                //{
                //    p.Move(-1, 0);
                //}
                //else if (info.Key == ConsoleKey.RightArrow)
                //{
                //    p.Move(1, 0);
                //}
                //else if (info.Key == ConsoleKey.UpArrow)
                //{
                //    p.Move(0, 1);
                //}
                //else if (info.Key == ConsoleKey.DownArrow)
                //{
                //    p.Move(0, -1);
                //}
                //else if (info.Key == ConsoleKey.Home)
                //{
                //    p.Zoom(1);
                //}
                //else if (info.Key == ConsoleKey.End)
                //{
                //    p.Zoom(-1);
                //}
            }
        }
    }
}
