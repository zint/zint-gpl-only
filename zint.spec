Name: zint
Version: 2.3
Release: 1
Summary: A barcode encoding library.
Summary(ru_RU.UTF8): A barcode encoding library.
License: GPL
Url:  http://www.zint.org.uk
Vendor: Robin Stuart <robin@zint.org.uk>
Packager: Radist Tagan <radist@list.ru>
Group: Development/Libraries/C and C++

BuildRoot: %{_tmppath}/%{name}-%{version}-build
Source:	%name-%version.tar.bz2
Patch:  error_svg_plot_fix.patch

##Requires: Qt >= 4.4
BuildRequires: cmake
BuildRequires: libqt4-devel
BuildRequires: libpng-devel
BuildRequires: zlib-devel
BuildRequires: qrencode-devel

%description
A library for encoding data in barcode symbols. Includes support for over 50 symbologies including
all ISO/IEC standards.

%package -n zint-devel
Summary:    Library and header files for zint.
Group:      Development/C  
Provides:   %{name}-devel = %{version}  
Requires:   zint = %{version}

%description -n zint-devel 
A library for encoding data in barcode symbols. Includes support for over 50 symbologies including
all ISO/IEC standards.

%debug_package

%prep
%setup
%patch -p1

%build
##mkdir 'build'
##cd build
cmake CMakeLists.txt
make -j2

%install
rm -rf $RPM_BUILD_ROOT
%makeinstall
cd %buildroot/usr/ && mv ./local/* ./
##make install DESTDIR=$RPM_BUILD_ROOT
%post
/sbin/ldconfig
%postun
ldconfig
%clean

%files
%{_libdir}/*
%{_datadir}/cmake/Modules/FindZint.cmake

%files -n zint-devel
%defattr(-,root,root,-)
%{_includedir}/*.h

%changelog
* Thu Mar 17 2009 radist <radist@list.ru>
  + Make spec