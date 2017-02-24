VERSION=2.88
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
	@echo "    make <release | debug>"
	@echo "    make <cppcheck>"
	@echo "    make <clean>"

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
	cp -R cmake src INSTALL README.md CMakeLists.txt Makefile sviewgl.spec sviewgl-$(VERSION)
	tar -f sviewgl-$(VERSION).tar -c sviewgl-$(VERSION)
	gzip -f sviewgl-$(VERSION).tar

deb:    clean
	install -d sviewgl-$(VERSION)
	cp -R cmake src INSTALL README.md CMakeLists.txt Makefile sviewgl-$(VERSION)
	tar -f ../sviewgl_$(VERSION).orig.tar -c sviewgl-$(VERSION)
	gzip -f ../sviewgl_$(VERSION).orig.tar
	rm -rf sviewgl-$(VERSION) Copying.txt bitbucket-pipelines.yml config.example shippable.yml sviewgl.spec *.ebuild
	dpkg-buildpackage -F -tc
	# apt install g++ make build-essential debhelper cmake pkg-config libgl1-mesa-dev libxrandr-dev libxcursor-dev libfreetype6-dev libjpeg-dev libtiff-dev libgif-dev liblcms2-dev libimlib2-dev libwebp-dev libglfw3-dev libexif-dev
