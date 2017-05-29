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
                    device.PanMin, device.PanMax, currentPan, 2, 65,
                    device.TiltMin, device.TiltMax, currentTilt, 2.2, 39,
                    w, h, x, y, out pan, out tilt);

                //Console.WriteLine("w: {0}, h: {1}, x: {2}, y: {3}, pan: {4}, tilt: {4}", w, h, x, y, pan, tilt);
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
            int panMin, int panMax, int panCurrent, double panMinDegreesOfView, double panMaxDegreesOfView,
            int tiltMin, int tiltMax, int tiltCurrent, double tiltMinDegreesOfView, double tiltMaxDegreesOfView,
            int w, int h, int x, int y, out int pan, out int tilt) {

            double div;

            // calculate zoom steps
            double zoomStep = (zoomMax - zoomMin) / zoomLevels;

            // calculate current level
            double zoomCurrentLevel = zoomCurrent / zoomStep;

            // calculate current pan view angle based on zoom
            double panDegreesRes = (panMaxDegreesOfView - panMinDegreesOfView) / zoomLevels;
            double panDegreesZoomed = panMaxDegreesOfView - (panDegreesRes * zoomCurrentLevel);
            double panDegreesOfViewOneDirection = panDegreesZoomed / 2;

            // calculate current tilt view angle based on zoom
            double tiltDegreesRes = (tiltMaxDegreesOfView - tiltMinDegreesOfView) / zoomLevels;
            double tiltDegreesZoomed = tiltMaxDegreesOfView - (tiltDegreesRes * zoomCurrentLevel);
            double tiltDegreesOfViewOneDirection = tiltDegreesZoomed / 2;

            // make view bounding box
            double minPanView = -panDegreesOfViewOneDirection;
            double maxPanView = panDegreesOfViewOneDirection;
            double minTiltView = -tiltDegreesOfViewOneDirection;
            double maxTiltView = tiltDegreesOfViewOneDirection;

            // calculate pan pixel to angle ratio
            double xres = (maxPanView - minPanView) / w;
            double xcenter = (double)w / 2;
            double xperpixel = Math.Abs(xcenter - x) * xres;

            // calculate new pan value
            if (x > xcenter) {
                div = Math.Ceiling(panCurrent + xperpixel);
            } else {
                div = Math.Floor(panCurrent - xperpixel);
            }
            pan = (int)div;

            // calculate tilt pixel to angle ratio
            double yres = (maxTiltView - minTiltView) / h;
            double ycenter = (double)h / 2;
            double yperpixel = Math.Abs(ycenter - y) * yres;

            // calculate new tilt value
            if (y > ycenter) {
                div = Math.Floor(tiltCurrent - yperpixel);
            } else {
                div = Math.Ceiling(tiltCurrent + yperpixel);
            }
            tilt = (int)div;
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
            pan = 0;
            tilt = 0;
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
