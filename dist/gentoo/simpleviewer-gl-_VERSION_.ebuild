# Copyright 1999-2017 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI=6

inherit cmake-utils git-r3

DESCRIPTION="Small and simple image viewer for Linux."
HOMEPAGE="http://www.ugolnik.info/?p=1302"
EGIT_REPO_URI="https://bitbucket.org/andreyu/simple-viewer-gl"
EGIT_TAG="${PV}"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~x86 ~amd64"
IUSE=""

DEPEND="
	media-libs/lcms
	media-libs/glfw
	media-libs/freetype:2
	media-libs/giflib
	media-libs/imlib2[X,jpeg,gif,png,tiff]
	virtual/jpeg
	media-libs/libpng
	media-libs/tiff
	media-libs/openexr
	media-libs/ilmbase
	virtual/opengl
"
RDEPEND="${DEPEND}"

src_install() {
	dobin "${BUILD_DIR}"/sviewgl
	dodoc config.example
}
