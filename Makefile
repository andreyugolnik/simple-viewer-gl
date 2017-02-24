VERSION=2.89
BUILD_DIR_RELEASE=.build_release
BUILD_DIR_DEBUG=.build_debug

BUNDLE_NAME=sviewgl

UNAME=$(shell uname -s)
ifeq ($(UNAME), Darwin)
	BUNDLE_NAME=sviewgl.app
endif

all:    release

help:
	@echo "Usage:"
	@echo "    make <release | debug>    - make release or debug application"
	@echo "    make <cppcheck>           - do static code verification"
	@echo "    make <dist>               - make RPM package"
	@echo "    make <deb>                - make DEB package"
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
	rm -fr $(BUILD_DIR_RELEASE) $(BUILD_DIR_DEBUG) sviewgl cppcheck-output

install:
	install -m 755 -d $(DESTDIR)/usr/bin
	install -m 755 sviewgl $(DESTDIR)/usr/bin

dist:   clean
	install -d sviewgl-$(VERSION)
	cp -R cmake src INSTALL README.md CMakeLists.txt Makefile dist/fedora/sviewgl.spec sviewgl-$(VERSION)
	tar -f sviewgl-$(VERSION).tar -c sviewgl-$(VERSION)
	gzip -f sviewgl-$(VERSION).tar

deb:    clean
	ln -sf dist/debian debian
	install -d sviewgl-$(VERSION)
	cp -R cmake src INSTALL README.md CMakeLists.txt Makefile sviewgl-$(VERSION)
	tar -f ../sviewgl_$(VERSION).orig.tar -c sviewgl-$(VERSION)
	gzip -f ../sviewgl_$(VERSION).orig.tar
	rm -rf sviewgl-$(VERSION) Copying.txt bitbucket-pipelines.yml config.example shippable.yml
	dpkg-buildpackage -F -tc

