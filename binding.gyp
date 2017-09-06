{
  "targets": [
    {
      "target_name": "ptz",
      "sources": [ "lib/ptz.cc" ],
      "libraries": [
        "-luvc",
        "-L/usr/local/Cellar/libuvc/0.0.5/include/libuvc"
      ],
      "include_dirs": [
        "/usr/local/Cellar/libuvc/0.0.5/include/libuvc",
      ]
    }
  ]
}
