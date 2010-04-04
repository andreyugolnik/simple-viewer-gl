/*
 * Simple Viewer GL edition
 *
 */

#include "main.h"
#include "window.h"

#include <locale.h>
#include <string.h>


std::auto_ptr<CWindow> g_window;

void ShowHelp(const char* pchArgv0);


// Program entry point
int main(int argc, char *argv[]) {
	setlocale(LC_ALL, "");

	printf("%s v%s\n\n", TITLE, FULLVERSION_STRING);
	printf("Copyright © 2008-2010 Andrey A. Ugolnik. All Rights Reserved.\n");
	printf("http://www.wegroup.org\n");
	printf("http://www.ugolnik.info\n");
	printf("andrey@ugolnik.info\n\n");

	if(argc < 2) {
		ShowHelp(argv[0]);
		return 0;
	}

	g_window.reset(new CWindow());
	const char* path	= 0;

	for(int i = 1; i < argc; i++) {
		if(strncmp(argv[i], "-h", 2) == 0 || strncmp(argv[i], "--help", 6) == 0) {
			ShowHelp(argv[0]);
			return 0;
		}
//		if(!strncmp(argv[i], "-i", 2))
//			g_infoBar->Show(false);
//		else if(!strncmp(argv[i], "-c", 2))
//			g_bShowBg	= false;
//		else if(!strncmp(argv[i], "-s", 2))
//			m_fitImage	= true;
//		else if(!strncmp(argv[i], "-f", 2))
//			g_bWindowed	= false;
//		else if(!strncmp(argv[i], "-C", 2)) {
//			if(3 == sscanf(argv[i+1], "%2cx%2cx%2cx", &g_byR, &g_byG, &g_byB)) {
//				g_bShowBg	= false;
//				i++;
//			}
//		}
		else {
			path	= argv[i];
		}
	}

	if(g_window->Init(argc, argv, path) == true) {
		glutMainLoop();
	}

    return 0;
}

void ShowHelp(const char* pchArgv0) {
	char* p	= strrchr(pchArgv0, '/');

	printf("\nUsage:\n");
	printf("  %s [OPTION]... FILE\n", (p ? p + 1 : pchArgv0));
	printf("  -h, --help    show this help;\n");
//	printf("  -i            disable on screen info;\n");
//	printf("  -s            enable scale to window;\n");
//	printf("  -f            start in fullscreen mode;\n");
//	printf("  -c            disable chequerboard;\n");
//	printf("  -C RRGGBB     background color in hex format;\n");

	printf("\nAvailable keys:\n");
	printf("  <esc>         exit;\n");
	printf("  <space>       next image;\n");
	printf("  <backspace>   previous image;\n");
	printf("  <+> / <->     scale image;\n");
	printf("  <0>           set scale to 100%%;\n");
//	printf("  <pgdn>        next image in multi-page image;\n");
//	printf("  <pgup>        previous image in multi-page image;\n");
	printf("  <enter>       switch fullscreen / windowed mode;\n");
//	printf("  <ctrl>+<b>    set as wallpapper;\n");
	printf("  <ctrl>+<del>  delete image from disk;\n");
	printf("  <s>           fit image to window (quick algorithm);\n");
//	printf("  <v>           flip vertical;\n");
//	printf("  <h>           flip horizontal;\n");
//	printf("  <r>           rotate clockwice;\n");
//	printf("  <l>           rotate counter clockwise;\n");
	printf("  <c>           hide / show chequerboard;\n");
	printf("  <i>           hide / show on screen info.\n");
	printf("\n");
}
