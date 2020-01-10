Luppp - OpenAV Productions
===============================

  * [Official web page](http://openavproductions.com/luppp)
  * [Demo videos](https://www.youtube.com/playlist?list=PLPVwzZjovbBxIik8lUisH5XdLzALDeY9j)
  * [User documentation](http://openavproductions.com/doc/luppp.html)

This is the repository of Luppp, the live looping tool.

[![Release](https://img.shields.io/github/release/openAVproductions/openAV-Luppp.svg)](https://github.com/openAVproductions/openAV-Luppp/releases/latest) 
[![Master][openAV-luppp-master-status]][travis-branches] 
[![GitHub license](https://img.shields.io/github/license/openAVproductions/openAV-Luppp)](https://github.com/openAVproductions/openAV-Luppp/blob/master/LICENSE)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/3ab898e60d3f4c8eabd44015e0e347f4)](https://www.codacy.com/manual/Jee-Bee/openAV-Luppp?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=Jee-Bee/openAV-Luppp&amp;utm_campaign=Badge_Grade)

![screenshot](resources/screenshots/1.2.png "Luppp 1.2 Screenshot")

Intro
-----
This version of Luppp is designed from zero to handle realtime
audio, and scale with additional features as needed.

This version depends on the following libraries:
please ensure the -dev versions are installed.

```bash
JACK
CAIRO
LIBLO
LIBSNDFILE
LIBSAMPLERATE
NTK  ( git clone git://git.tuxfamily.org/gitroot/non/fltk.git ntk )
```

Build Status <img src="https://blog.travis-ci.com/images/travis-mascot-200px.png" height="30"/>
------------

| Master | 1.2.1 |
|:------:|:----:|
|[![Master][openAV-luppp-master-status]][travis-branches]|[![1.2.1][openAV-1.2.1-status]][travis-branches]|

[openAV-1.2.1-status]: https://travis-ci.com/openAVproductions/openAV-Luppp.svg?branch=release-1.2.1
[openAV-luppp-master-status]: https://travis-ci.com/openAVproductions/openAV-Luppp.svg?branch=master
[travis-branches]: https://travis-ci.com/openAVproductions/openAV-Luppp/branches
[travis-builds]: https://travis-ci.com/openAVproductions/openAV-Luppp/builds


Install
-------

Run the following commands from the top directory to configure & install Luppp:

```bash
meson build
cd build
ninja
./luppp
```


Issues
------
Please report bugs on [github.com/openAVproductions/openAV-Luppp/issues](http://github.com/openAVproductions/openAV-Luppp/issues)


Contact
-------
If you have a particular question, email me!
```
harryhaaren@gmail.com
```

Cheers, -Harry
