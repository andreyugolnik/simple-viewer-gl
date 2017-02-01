##Simple Viewer GL - simple and tiny image viewer based on OpenGL.##

The primary goal for writing **sviewgl** is to create an image viewer, which only has the most basic features required for fast image viewing. It has some vi-like key bindings and works nicely with tiling window managers (such as Ion3 and Notion).
Supported formats: PNG, JPEG, BMP, TIFF, GIF, PPM, DDS, BMP, TARGA, PSD, ICO, XWD, SCR (ZX-Spectrum screen), XPM, WebP.

##Screenshot##

![simpleviewergl.png](https://bitbucket.org/repo/XgobE8/images/1203610096-simpleviewergl.png)

##Features##

* Lightweight and fast: utilize hardware-accelerated video card;
* Support embeded ICC profiles.
* Very simple interface;
* Suitable for default image viewer of desktop;
* Minimal lib dependency: look library requirements below;
* Desktop independent: doesn't require any specific desktop environment;
* Open source, licensed under GNU GPL.

##Usage##

**sviewgl** has two modes of operation: image viewer and image info mode. The default is image mode, in which only the current image is shown. In image info mode pixel info and rectangular selection available. This mode useful for brief resource description.

##Key bindings##

Key             | Description
----------------| --------------------------------------
<esc>           | exit
<space>         | next image
<backspace>     | previous image
<+> / <->       | scale image
<1>...<0>       | set scale from 100% to 1000%
<enter>         | switch fullscreen / windowed mode
<ctrl>+<del>    | delete image from disk
<r>             | rotate clockwise
<shift>+<r>     | rotate counterclockwise
<pgup> / <bgdn> | previous /next subimage
<s>             | fit image to window (quick algorithm)
<shift>+<s>     | toggle 'keep scale' on image load
<c>             | hide / show chequerboard
<i>             | hide / show on screen info
<p>             | hide / show pixel info
<b>             | hide / show border around image

##Download and build##

You can browse the source code repository on GitHub or get a copy using git with the following command:
```
$ git clone https://bitbucket.org/andreyu/simple-viewer-gl.git
$ cd simple-viewer-gl
$ make release
```

##Dependencies##

* GLFW3 - multi-platform library for creating windows with OpenGL contexts and receiving input and events.
* OpenGL
* Imlib2 - common formats support.
* libjpeg - jpeg format support.
* libpng - png format support.
* giflib - gif format support.
* tiff - tiff format support.
* webp - WebP format support.
* freetype2 - FreeType 2 font engine, shared library files.
* zlib - compression support.
* Little CMS 2 - Color management engine supporting ICC profiles.

***
```
Copyright © 2008-2017 Andrey A. Ugolnik. All Rights Reserved.
http://www.ugolnik.info
andrey@ugolnik.info
```
