# https://www.linux.org.ru/news/opensource/13243418?cid=13244508

Prefix: %{_usr}
Name: sviewgl
Version: 2.88
Release: 1%{?dist}
Summary: Simple Viewer GL - simple and tiny image viewer based on OpenGL

Group: LOR/stuff
License: GPLv2
URL: https://bitbucket.org/andreyu/simple-viewer-gl
Source0: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires: gcc, make, cmake
BuildRequires: mesa-libGL-devel, glfw-devel, freetype-devel, libpng-devel, libjpeg-turbo-devel, libtiff-devel, giflib-devel, lcms2-devel, libwebp-devel, libexif-devel, imlib2-devel, zlib-devel

%description
Simple Viewer GL - simple and tiny image viewer based on OpenGL

%prep
%setup -q

%build
%{__make} %{?_smp_mflags} CXXFLAGS="${RPM_OPT_FLAGS}" LFLAGS="${RPM_LD_FLAGS}" release

%install
rm -rf $RPM_BUILD_ROOT
install -d ${RPM_BUILD_ROOT}/usr/bin -m 755
install sviewgl ${RPM_BUILD_ROOT}/usr/bin -m 755

%clean
rm -rf ${RPM_BUILD_ROOT}

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%files
/usr/bin/sviewgl
