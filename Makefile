VER_MAJOR=3
VER_MINOR=0
VER_RELEASE=0
VERSION=$(VER_MAJOR).$(VER_MINOR)$(VER_RELEASE)
BUILD_DIR_RELEASE=.build_release
BUILD_DIR_DEBUG=.build_debug
BUNDLE_NAME=sviewgl
OUT_NAME=sviewgl

PREFIX?=/usr/local

UNAME=$(shell uname -s)
ifeq ($(UNAME), Darwin)
	BUNDLE_NAME=sviewgl.app
	OUT_NAME="Simple Viewer GL.app"
endif

all:    release

help:
	@echo "Usage:"
	@echo "    make <release | debug>    - make release or debug application"
	@echo "    make install              - install application"
	@echo "    make <cppcheck>           - do static code verification"
	@echo "    make <dist>               - make tar.gz source package"
	@echo "    make <deb>                - make DEB package"
	@echo "    make <rpm>                - make RPM package"
	@echo "    make <clean>              - cleanup directory"

resources:
	$(shell if [ $(UNAME) == "Darwin" ]; then \
		makeicns -in sviewgl.png -out res/macos/Icon.icns; \
		fi )

release: resources
	$(shell if [ ! -d $(BUILD_DIR_RELEASE) ]; then mkdir $(BUILD_DIR_RELEASE); fi)
	cd $(BUILD_DIR_RELEASE) ; cmake -DCMAKE_BUILD_TYPE=Release -DAPP_VERSION_MAJOR:STRING=$(VER_MAJOR) -DAPP_VERSION_MINOR:STRING=$(VER_MINOR) -DAPP_VERSION_RELEASE:STRING=$(VER_RELEASE) .. ; make ; cd ..
	rm -fr $(OUT_NAME) && cp -r $(BUILD_DIR_RELEASE)/$(BUNDLE_NAME) $(OUT_NAME)

debug: resources
	$(shell if [ ! -d $(BUILD_DIR_DEBUG) ]; then mkdir $(BUILD_DIR_DEBUG); fi)
	cd $(BUILD_DIR_DEBUG) ; cmake -DCMAKE_BUILD_TYPE=Debug -DAPP_VERSION_MAJOR:STRING=$(VER_MAJOR) -DAPP_VERSION_MINOR:STRING=$(VER_MINOR) -DAPP_VERSION_RELEASE:STRING=$(VER_RELEASE) .. ; make ; cd ..
	rm -fr $(OUT_NAME) && cp -r $(BUILD_DIR_DEBUG)/$(BUNDLE_NAME) $(OUT_NAME)

cppcheck:
	cppcheck -j 1 --enable=all -f -I src src/ 2> cppcheck-output

clean:
	rm -fr $(BUILD_DIR_RELEASE) $(BUILD_DIR_DEBUG) $(BUNDLE_NAME) cppcheck-output $(BUNDLE_NAME)-$(VERSION)* $(BUNDLE_NAME)_$(VERSION)* *.log *.tasks *.sh *.xz *.list *.deb strace_out cov-int

install:
	install -m 755 -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 $(BUNDLE_NAME) $(DESTDIR)$(PREFIX)/bin

dist:   clean
	install -d $(BUNDLE_NAME)-$(VERSION)
	cp -R cmake src res dist/debian INSTALL README.md CMakeLists.txt Makefile sviewgl.desktop sviewgl.png dist/fedora/* dist/gentoo/* $(BUNDLE_NAME)-$(VERSION)
	mv $(BUNDLE_NAME)-$(VERSION)/simpleviewer-gl-_VERSION_.ebuild $(BUNDLE_NAME)-$(VERSION)/simpleviewer-gl-$(VERSION).ebuild
	sed "s/_VERSION_/$(VERSION)/" -i $(BUNDLE_NAME)-$(VERSION)/$(BUNDLE_NAME).spec
	sed "s/_VERSION_/$(VERSION)/" -i $(BUNDLE_NAME)-$(VERSION)/debian/changelog
	tar -zf $(BUNDLE_NAME)-$(VERSION).tar.gz -c $(BUNDLE_NAME)-$(VERSION)

deb:    clean dist
	mv $(BUNDLE_NAME)-$(VERSION).tar.gz $(BUNDLE_NAME)_$(VERSION).orig.tar.gz
	cd $(BUNDLE_NAME)-$(VERSION) ; PREFIX=/usr dpkg-buildpackage -F -tc

rpm:    clean dist
	rpmbuild -ta $(BUNDLE_NAME)-$(VERSION).tar.gz
