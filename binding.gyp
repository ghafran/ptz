{
  "targets": [
    {
      "target_name": "ptz",
      "sources": [ "lib/ptz.cc" ],
      "libraries": [
        "-luvc"
      ],
      'include_dirs': [
        '/usr/local/libuvc',
        '/usr/local/Cellar/libuvc',
        '/usr/local/lib/libuvc',
        '/usr/local/include/libuvc',
      ]
    }
  ]
}
