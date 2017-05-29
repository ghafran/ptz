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
                var newZoom = zoomLevel(device.ZoomMin, device.ZoomMax, 10, level);
                int res = device.AbsoluteZoom(newZoom);
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
                int pan;
                int tilt;
 
                var currentzoom = device.GetZoom();
                int currentPan = device.GetPan();
                int currentTilt = device.GetTilt();

                click(device.ZoomMin, device.ZoomMax, currentzoom, 10,
                    device.PanMin, device.PanMax, currentPan, 56,
                    device.TiltMin, device.TiltMax, currentTilt, 37,
                    w, h, x, y, out pan, out tilt);

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
			else if (request.Url.LocalPath == "/cmd/pantilt_command")
			{
                string command = qs.Get("command");
				var device = PTZ.Device.GetDevice(name);
				int pan;
				int tilt;

				int currentPan = device.GetPan();
				int currentTilt = device.GetTilt();

				panTiltCommand(command, device.PanMin, device.PanMax, currentPan, device.PanDefault,
					device.TiltMin, device.TiltMax, currentTilt, device.TiltDefault,
					out pan, out tilt);

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
        
        static void click(int zoomMin, int zoomMax, int zoomCurrent, int zoomLevels,
            int panMin, int panMax, int panCurrent, int panDegreesOfView,
            int tiltMin, int tiltMax, int tiltCurrent, int tiltDegreesOfView,
            int w, int h, int x, int y, out int pan, out int tilt) {

            double div;

            int zoomTweak = 1;
            div = (double)(zoomMax - zoomMin) / zoomLevels;
            int zoomStep = (int)Math.Floor(div);
            div = (double)zoomCurrent / zoomStep;
            int zoomCurrentLevel = (int)Math.Floor(div);

            div = (double)panDegreesOfView / 2;
            int panDegreesOfViewOneDirection = (int)Math.Floor(div);
            div = (double)tiltDegreesOfView / 2;
            int tiltDegreesOfViewOneDirection = (int)Math.Floor(div);
            double panOffset = zoomCurrent == 0 ? panDegreesOfViewOneDirection : (1 / (double)zoomCurrentLevel) * panDegreesOfViewOneDirection * zoomTweak;
            double tiltOffset = zoomCurrent == 0 ? tiltDegreesOfViewOneDirection : (1 / (double)zoomCurrentLevel) * tiltDegreesOfViewOneDirection * zoomTweak;

            double minPanView = -panOffset;
            double maxPanView = panOffset;
            double minTiltView = -tiltOffset;
            double maxTiltView = tiltOffset;

            double xres = (double)(maxPanView - minPanView) / w; // pan value per pixel
            div = (double)w / 2;
            int xcenter = (int)Math.Floor(div);
            double xperpixel = Math.Abs(xcenter - x) * xres;

            if(x > xcenter) {
                div = panCurrent + xperpixel;
            } else {
                div = panCurrent - xperpixel;
            }
            if(div > 0){
                pan = (int)Math.Floor(div);
            } else {
                pan = (int)Math.Ceiling(div);
            }

            double yres = (double)(maxTiltView - minTiltView) / h; // tilt value per pixel
            div = (double)h / 2;
            int ycenter = (int)Math.Floor(div);
            double yperpixel = Math.Abs(ycenter - y) * yres;

            if(y > ycenter) {
                div = tiltCurrent - yperpixel;
            } else {
                div = tiltCurrent + yperpixel;
            }
            if(div > 0){
                tilt = (int)Math.Floor(div);
            } else {
                tilt = (int)Math.Ceiling(div);
            }
        }

        static int zoomLevel(int zoomMin, int zoomMax, int zoomLevels, int level) {
            double div = (zoomMax - zoomMin) / zoomLevels;
            int stepsPerLevel = (int)Math.Floor(div);
            var newZoom = stepsPerLevel * level;
            return newZoom;
        }

        static void panTiltCommand(string command, 
                            int panMin, int panMax, int panCurrent, int panDefault, 
                            int tiltMin, int tiltMax, int tiltCurrent, int defaultTilt, 
                            out int pan, out int tilt)
        {
			var singlePanDegreeValue = (panMax - panMin) > 360 ? 3600 : 1;
			var singleTiltDegreeValue = (tiltMax - tiltMin) > 360 ? 3600 : 1;
			if (command == "home")
			{
				pan = panDefault;
				tilt = defaultTilt;
			}
			else if (command == "left")
			{
				pan = panCurrent - singlePanDegreeValue;
				tilt = tiltCurrent;
			}
			else if (command == "right")
			{
				pan = panCurrent + singlePanDegreeValue;
				tilt = tiltCurrent;
			}
			else if (command == "up")
			{
				pan = panCurrent;
				tilt = tiltCurrent + singleTiltDegreeValue;
			}
			else if (command == "down")
			{
				pan = panCurrent;
				tilt = tiltCurrent - singleTiltDegreeValue;
			}
        }
	}
}
