APP_VERSION_MAJOR=2
APP_VERSION_MINOR=8
APP_VERSION_RELEASE=9
VERSION=$(APP_VERSION_MAJOR).$(APP_VERSION_MINOR)$(APP_VERSION_RELEASE)
BUILD_DIR_RELEASE=.build_release
BUILD_DIR_DEBUG=.build_debug
DESTDIR=/usr/local
BUNDLE_NAME=sviewgl

UNAME=$(shell uname -s)
ifeq ($(UNAME), Darwin)
	BUNDLE_NAME=sviewgl.app
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

release:
	$(shell if [ ! -d $(BUILD_DIR_RELEASE) ]; then mkdir $(BUILD_DIR_RELEASE); fi)
	cd $(BUILD_DIR_RELEASE) ; cmake -DCMAKE_BUILD_TYPE=Release .. ; make ; cd ..
	cp -r $(BUILD_DIR_RELEASE)/$(BUNDLE_NAME) .

debug:
	$(shell if [ ! -d $(BUILD_DIR_DEBUG) ]; then mkdir $(BUILD_DIR_DEBUG); fi)
	cd $(BUILD_DIR_DEBUG) ; cmake -DCMAKE_BUILD_TYPE=Debug .. ; make ; cd ..
	cp -r $(BUILD_DIR_DEBUG)/$(BUNDLE_NAME) .

cppcheck:
	cppcheck -j 1 --enable=all -f -I src src/ 2> cppcheck-output

clean:
	rm -fr $(BUILD_DIR_RELEASE) $(BUILD_DIR_DEBUG) $(BUNDLE_NAME) cppcheck-output $(BUNDLE_NAME)-$(VERSION)* $(BUNDLE_NAME)_$(VERSION)* *.{log,tasks,sh,xz,list} strace_out cov-int

install:
	install -m 755 -d $(DESTDIR)/usr/bin
	install -m 755 $(BUNDLE_NAME) $(DESTDIR)/usr/bin

dist:   clean
	install -d $(BUNDLE_NAME)-$(VERSION)
	cp -R cmake src res dist/debian INSTALL README.md CMakeLists.txt Makefile sviewgl.desktop sviewgl.png dist/fedora/* dist/gentoo/* $(BUNDLE_NAME)-$(VERSION)
	mv $(BUNDLE_NAME)-$(VERSION)/simpleviewer-gl-_VERSION_.ebuild $(BUNDLE_NAME)-$(VERSION)/simpleviewer-gl-$(VERSION).ebuild
	sed "s/_VERSION_/$(VERSION)/" -i $(BUNDLE_NAME)-$(VERSION)/$(BUNDLE_NAME).spec
	sed "s/_VERSION_/$(VERSION)/" -i $(BUNDLE_NAME)-$(VERSION)/debian/changelog
	tar -zf $(BUNDLE_NAME)-$(VERSION).tar.gz -c $(BUNDLE_NAME)-$(VERSION)

deb:    clean dist
	mv $(BUNDLE_NAME)-$(VERSION).tar.gz $(BUNDLE_NAME)_$(VERSION).orig.tar.gz
	cd $(BUNDLE_NAME)-$(VERSION) ; dpkg-buildpackage -F -tc

rpm:    clean dist
	rpmbuild -ta $(BUNDLE_NAME)-$(VERSION).tar.gz
