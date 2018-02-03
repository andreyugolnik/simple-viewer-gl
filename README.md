![CI status, master](https://api.shippable.com/projects/5894665c8d80360f008b75d4/badge?branch=master)
![CI coverage, master](https://api.shippable.com/projects/5894665c8d80360f008b75d4/coverageBadge?branch=master)

###Simple Viewer GL - simple and tiny image viewer based on OpenGL.###

The primary goal for writing **Simple Viewer GL** is to create an image viewer, which only has the most basic features required for fast image viewing. It has some vi-like key bindings and works nicely with tiling window managers (such as Ion3 and Notion).

Supported formats: `PNG`, `JPEG`, `PSD`, `AI`, `EPS`, `GIF`, `TIFF`, `TARGA`, `ICO`, `BMP`, `PNM`, `DDS`, `BMP`, `XWD`, `SCR (ZX-Spectrum screen)`, `XPM`, `WebP`, `OpenEXR`.

***
##Screenshot##

![Simple Viewer GL on macOS with EXIF](https://bitbucket.org/andreyu/simple-viewer-gl/downloads/sviewgl-macos_2.png)
![Simple Viewer GL on macOS with Pixel Info](https://bitbucket.org/andreyu/simple-viewer-gl/downloads/sviewgl-macos_3.png)

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
* `<del>`           - toggle deletion mark;
* `<ctrl>+<del>`    - delete marked images from disk;
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
* `<?>`             - hide / show keybindings popup;

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
$ sudo apt-get update
$ sudo apt-get install g++ make build-essential debhelper cmake pkg-config libgl1-mesa-dev
$ sudo apt-get install libxrandr-dev libxcursor-dev libfreetype6-dev libjpeg-dev
$ sudo apt-get install libtiff-dev libgif-dev liblcms2-dev libimlib2-dev libwebp-dev
$ sudo apt-get install libglfw3-dev libexif-dev libilmbase-dev libopenexr-dev
```

Clone and make DEB:
```bash
$ git clone https://bitbucket.org/andreyu/simple-viewer-gl.git
$ cd simple-viewer-gl
$ make deb
```

> You can purge installed packages with `apt-get purge PACKAGE_NAME && apt-get autoremove`

***
##Make RPM package##

Update and install required dependencies:
```bash
$ sudo dnf install gcc-c++ make cmake mesa-libGL-devel glfw-devel freetype-devel
$ sudo dnf install libpng-devel libjpeg-turbo-devel libtiff-devel giflib-devel lcms2-devel
$ sudo dnf install libwebp-devel libexif-devel imlib2-devel zlib-devel ilmbase-devel OpenEXR-devel
```

Clone and make source for RPM:
```bash
$ git clone https://bitbucket.org/andreyu/simple-viewer-gl.git
$ cd simple-viewer-gl
$ sudo make rpm
```

***
##Dependencies##

 Name          | Debian package                     | Description / Notes
---------------|------------------------------------|---------------------
`CMake`        | *cmake*                            | Cross-platform family of tools designed to build, test and package software.
`GLFW3`        | *libglfw3-dev*                     | Multi-platform library for creating windows with OpenGL contexts and receiving input and events.
`JPEG`         | *libjpeg-dev*                      | JPEG format support.
`PNG`          | *libpng12-dev*                     | PNG format support.
`GIF`          | *libgif-dev*                       | GIF format support.
`TIFF`         | *libtiff5-dev*                     | TIFF format support.
`WebP`         | *libwebp-dev*                      | WebP format support.
`ImLib2`       | *libimlib2-dev*                    | (Optional) Common formats support.
`OpenEXR`      | *libilmbase-dev*, *libopenexr-dev* | (Optional) OpenEXR is a high dynamic-range (HDR) image file format developed by Industrial Light & Magic for use in computer imaging applications.
`FreeType2`    | *libfreetype6-dev*                 | TTF/OTF font engine.
`Little CMS 2` | *liblcms2-dev*                     | Color management engine supporting ICC profiles.
`Zlib`         | *zlib1g-dev*                       | Compression support.
`Exif`         | *libexif-dev*                      | Exif support.
`X11`          | *libxrandr-dev*, *libxcursor-dev*  | X11 related libraries (Linux only)

```
Copyright © 2008-2018 Andrey A. Ugolnik. All Rights Reserved.
http://www.ugolnik.info
andrey@ugolnik.info

Icon was created by
Iryna Poliakova (Iryna.poliakova@icloud.com).
```
