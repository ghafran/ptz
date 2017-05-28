using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Web;
using PTZ;
using System.Collections.Specialized;
using System.Configuration;

namespace PTZServer
{
    class Program
    {
        static void Main(string[] args)
        {
            WebServer ws = new WebServer(SendResponse, "http://localhost:8443/");
            ws.Run();
            Console.WriteLine("A simple webserver. Press a key to quit.");
            Console.ReadKey();
            ws.Stop();
        }

        public static string SendResponse(HttpListenerRequest request)
        {
            Console.WriteLine(request.Url.OriginalString);
            NameValueCollection qs = HttpUtility.ParseQueryString(request.Url.Query);
            var name = ConfigurationManager.AppSettings["DeviceName"];
            if (request.Url.LocalPath == "/cmd/zoom")
            {
                int level = int.Parse(qs.Get("level"));
                var device = PTZ.Device.GetDevice(name);
                int res = device.AbsoluteZoom(level);
                if (res == 0)
                {
                    return string.Format("<HTML><BODY>Zoom Successful! {0}</BODY></HTML>", DateTime.Now);
                }
                else
                {
                    return string.Format("<HTML><BODY>Zoom Failed! {0} - {1}</BODY></HTML>", DateTime.Now, res);
                }
            }
            else if (request.Url.LocalPath == "/cmd/pantilt")
            {
                int pan = int.Parse(qs.Get("pan"));
                int tilt = int.Parse(qs.Get("tilt"));
                var device = PTZ.Device.GetDevice(name);
                int res = device.AbsolutePanTilt(pan, tilt);
                if (res == 0)
                {
                    return string.Format("<HTML><BODY>Pan/Tilt Successful! {0}</BODY></HTML>", DateTime.Now);
                }
                else
                {
                    return string.Format("<HTML><BODY>Pan/Tilt Failed! {0} - {1}</BODY></HTML>", DateTime.Now, res);
                }
            }
            else if (request.Url.LocalPath == "/cmd/click")
            {
                int x = int.Parse(qs.Get("x"));
                int y = int.Parse(qs.Get("y"));
                int w = int.Parse(qs.Get("w"));
                int h = int.Parse(qs.Get("h"));
                var device = PTZ.Device.GetDevice(name);

                var tweak = 1.2;
                var currentzoom = device.GetZoom();
                int currentPan = device.GetPan();
                int currentTilt = device.GetTilt();

                int zoomrange = (device.ZoomMax - device.ZoomMin) / 10;
                int level = currentzoom / zoomrange;
                double panOffset = currentzoom == 0 ? 56 : (1 / level) * 56 * tweak; //56 is degrees viewable from center
                double tiltOffset = currentzoom == 0 ? 37 : (1 / level) * 37 * tweak; //37 is degrees viewable from center
                int minPanView = (int)-panOffset;
                int maxPanView = (int)panOffset;
                int minTiltView = (int)-tiltOffset;
                int maxTiltView = (int)tiltOffset;
                int pan;
                int xres = (maxPanView - minPanView) / (w); // pan value per pixel
                int xcenter = w / 2;
                int xperpixel = Math.Abs(xcenter - x) * (xres / 2);
                if (x > xcenter)
                {
                    pan = currentPan + xperpixel;
                }
                else
                {
                    pan = currentPan - xperpixel;
                }

                int tilt;
                int yres = (maxTiltView - minTiltView) / (h); // tilt value per pixel
                int ycenter = h / 2;
                int yperpixel = Math.Abs(ycenter - y) * (yres / 2);
                if (y > ycenter)
                {
                    tilt = currentTilt - yperpixel;
                }
                else
                {
                    tilt = currentTilt + yperpixel;
                }

                int res = device.AbsolutePanTilt(pan, tilt);
                if (res == 0)
                {
                    return string.Format("<HTML><BODY>Click Successful! {0}</BODY></HTML>", DateTime.Now);
                }
                else
                {
                    return string.Format("<HTML><BODY>Click Failed! {0} - {1}</BODY></HTML>", DateTime.Now, res);
                }
            }
            else
            {
                return string.Format("<HTML><BODY>And we're up!<br>{0}</BODY></HTML>", DateTime.Now);
            }
        }
    }
}
