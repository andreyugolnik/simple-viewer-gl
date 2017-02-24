![CI status, master](https://api.shippable.com/projects/5894665c8d80360f008b75d4/badge?branch=master)
![CI coverage, master](https://api.shippable.com/projects/5894665c8d80360f008b75d4/coverageBadge?branch=master)

###Simple Viewer GL - simple and tiny image viewer based on OpenGL.###

The primary goal for writing **Simple Viewer GL** is to create an image viewer, which only has the most basic features required for fast image viewing. It has some vi-like key bindings and works nicely with tiling window managers (such as Ion3 and Notion).

Supported formats: `PNG`, `JPEG`, `PSD`, `GIF`, `TIFF`, `TARGA`, `ICO`, `BMP`, `PNM`, `DDS`, `BMP`, `XWD`, `SCR (ZX-Spectrum screen)`, `XPM`, `WebP`.

***
##Screenshot##

![Simple Viewer GL](https://bitbucket.org/repo/XgobE8/images/1203610096-simpleviewergl.png)

***
##Features##

* Lightweight and fast: utilize hardware-accelerated video card;
* Support embeded ICC profiles;
* GIF animation support;
* Exif support;
* Very simple interface;
* Suitable for default image viewer of desktop;
* Minimal lib dependency: look library requirements below;
* Desktop independent: doesn't require any specific desktop environment;
* Open source, licensed under GNU GPL;

***
##Usage##

**Simple Viewer GL** has two modes of operation: image viewer and image info mode. The default is image mode, in which only the current image is shown. In image info mode pixel info and rectangular selection available. This mode useful for brief resource description.

***
##Key bindings##

* `<esc>`           - exit;
* `<space>`         - next image;
* `<backspace>`     - previous image;
* `<+> / <->`       - scale image;
* `<1>...<0>`       - set scale from 100% to 1000%;
* `<enter>`         - switch fullscreen / windowed mode;
* `<ctrl>+<del>`    - delete image from disk;
* `<r>`             - rotate clockwise;
* `<shift>+<r>`     - rotate counterclockwise;
* `<pgup> / <bgdn>` - previous /next subimage;
* `<s>`             - fit image to window;
* `<shift>+<s>`     - toggle 'keep scale' on image load;
* `<c>`             - hide / show chequerboard;
* `<i>`             - hide / show on-screen info;
* `<e>`             - hide / show exif;
* `<p>`             - hide / show pixel info;
* `<b>`             - hide / show border around image;

***
##Download and build from sources##

You can browse the source code repository on Bitbucket or get a copy using git with the following command:

```bash
$ git clone https://bitbucket.org/andreyu/simple-viewer-gl.git
$ cd simple-viewer-gl
$ make release
```
> On success **sviewgl** binary produced in current directory. Just symlink it `ln -s /path/to/sviewgl /usr/bin/sviewgl` or copy it `cp /path/to/sviewgl /usr/bin/`.

[Slackbuild by](https://github.com/saahriktu/saahriktu-slackbuilds/tree/master/simple-viewer-gl) [saahriktu](https://www.linux.org.ru/people/saahriktu/profile).
[Gentoo ebuild by](https://gogs.lumi.pw/mike/portage/src/master/media-gfx/simpleviewer-gl) [imul](https://www.linux.org.ru/people/imul/profile)

***
##Make DEB package##

Update and install required dependencies:
```bash
$ apt-get update
$ apt-get install g++ make build-essential debhelper cmake pkg-config libgl1-mesa-dev libxrandr-dev libxcursor-dev libfreetype6-dev libjpeg-dev libtiff-dev libgif-dev liblcms2-dev libimlib2-dev libwebp-dev libglfw3-dev libexif-dev
```

Clone and make DEB:
```bash
$ git clone https://bitbucket.org/andreyu/simple-viewer-gl.git
$ cd simple-viewer-gl
$ ln -sf dist/debian debian
$ make deb
```

> You can purge installed packages with `apt-get purge PACKAGE_NAME && apt-get autoremove`

***
##Make RPM package##

Clone and make RPM:
```bash
$ git clone https://bitbucket.org/andreyu/simple-viewer-gl.git
$ cd simple-viewer-gl
$ make dist
```

***
##Dependencies##

 Name          | Debian package                 | Description / Notes
---------------|--------------------------------|---------------------
`cmake`        | *cmake*                        | Cross-platform family of tools designed to build, test and package software.
`glfw3`        | *libglfw3-dev*                 | Multi-platform library for creating windows with OpenGL contexts and receiving input and events.
`imlib2`       | *libimlib2-dev*                | Common formats support.
`jpeg`         | *libjpeg-dev*                  | JPEG format support.
`png`          | *libpng12-dev*                 | PNG format support.
`gif`          | *libgif-dev*                   | GIF format support.
`tiff`         | *libtiff5-dev*                 | TIFF format support.
`webp`         | *libwebp-dev*                  | WebP format support.
`freetype2`    | *libfreetype6-dev*             | TTF/OTF font engine.
`Little CMS 2` | *liblcms2-dev*                 | Color management engine supporting ICC profiles.
`zlib`         | *zlib1g-dev*                   | Compression support.
`exif`         | *libexif-dev*                  | Exif support.
`X11`          | *libxrandr-dev libxcursor-dev* | X11 related libraries (Linux only)

```
Copyright © 2008-2017 Andrey A. Ugolnik. All Rights Reserved.
http://www.ugolnik.info
andrey@ugolnik.info
```
