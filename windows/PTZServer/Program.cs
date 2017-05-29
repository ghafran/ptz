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
                    device.PanMin, device.PanMax, currentPan, 4, 56,
                    device.TiltMin, device.TiltMax, currentTilt, 4, 37,
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
            else
            {
                return string.Format("<HTML><BODY>And we're up!<br>{0}</BODY></HTML>", DateTime.Now);
            }
        }
        
        static void click(int zoomMin, int zoomMax, int zoomCurrent, int zoomLevels,
            int panMin, int panMax, int panCurrent, int panMinDegreesOfView, int panMaxDegreesOfView,
            int tiltMin, int tiltMax, int tiltCurrent, int tiltMinDegreesOfView, int tiltMaxDegreesOfView,
            int w, int h, int x, int y, out int pan, out int tilt) {

            double div;

            // calculate zoom steps
            div = (double)(zoomMax - zoomMin) / zoomLevels;
            int zoomStep = (int)Math.Floor(div);

            // calculate current level
            div = (double)zoomCurrent / zoomStep;
            int zoomCurrentLevel = (int)Math.Floor(div);

            // calculate current pan view angle based on zoom
            double panDegreesRes = (panMaxDegreesOfView - panMinDegreesOfView) / (double)zoomLevels;
            double panDegreesZoomed = panMaxDegreesOfView - (panDegreesRes * zoomCurrentLevel);
            double panDegreesOfView = zoomCurrent == 0 ? panMaxDegreesOfView : panDegreesZoomed;
            div = (double)panDegreesOfView / 2;
            double panDegreesOfViewOneDirection = Math.Floor(div);

            // calculate current tilt view angle based on zoom
            double tiltDegreesRes = (tiltMaxDegreesOfView - tiltMinDegreesOfView) / (double)zoomLevels;
            double tiltDegreesZoomed = tiltMaxDegreesOfView - (tiltDegreesRes * zoomCurrentLevel);
            double tiltDegreesOfView = zoomCurrent == 0 ? tiltMaxDegreesOfView : tiltDegreesZoomed;
            div = (double)tiltDegreesOfView / 2;
            double tiltDegreesOfViewOneDirection = Math.Floor(tiltDegreesOfView);

            // make view bounding box
            double minPanView = -panDegreesOfViewOneDirection;
            double maxPanView = panDegreesOfViewOneDirection;
            double minTiltView = -tiltDegreesOfViewOneDirection;
            double maxTiltView = tiltDegreesOfViewOneDirection;

            // calculate pan pixel to angle ratio
            double xres = (double)(maxPanView - minPanView) / w;
            div = (double)w / 2;
            int xcenter = (int)Math.Floor(div);
            double xperpixel = Math.Abs(xcenter - x) * xres;

            // calculate new pan value
            if (x > xcenter) {
                div = panCurrent + xperpixel;
            } else {
                div = panCurrent - xperpixel;
            }
            if(div > 0){
                pan = (int)Math.Floor(div);
            } else {
                pan = (int)Math.Ceiling(div);
            }

            // calculate tilt pixel to angle ratio
            double yres = (double)(maxTiltView - minTiltView) / h;
            div = (double)h / 2;
            int ycenter = (int)Math.Floor(div);
            double yperpixel = Math.Abs(ycenter - y) * yres;

            // calculate new tilt value
            if (y > ycenter) {
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
    }
}
