{
  "targets": [
    {
      "target_name": "ptz",
      "sources": [ "lib/ptz.cc" ],
      "libraries": [
        "-luvc"
      ],
      "include_dirs": [
        "/usr/include/",
        "/usr/include/libusb-1.0",
        "/usr/local/include",
        "/usr/local/include/libuvc",
        "/usr/local/Cellar/libuvc/0.0.5/include",
        "/usr/local/Cellar/libuvc/0.0.5/include/libuvc",
        "/usr/local/Cellar/libusb/1.0.21/include",
        "/usr/local/Cellar/libusb/1.0.21/include/libusb-1.0"
      ]
    }
  ]
}
