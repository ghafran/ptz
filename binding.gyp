{
  "targets": [
    {
      "target_name": "ptz",
      "sources": [ "lib/ptz.cc" ],
      "libraries": [
        "-luvc"
      ],
      "include_dirs": [
        "/usr/local/Cellar/libuvc/0.0.5/include",
        "/usr/local/Cellar/libusb/1.0.21/include"
      ]
    }
  ]
}
