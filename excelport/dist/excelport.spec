%define name			excelport
%define version_major	0
%define version_minor	1
%define version_patch	0
%define	version_build	1

%define debug_package %{nil}

%define install_dir	/opt/excelport

Summary:	excelport daemon

Name:			%{name}
Version:		%{version_major}.%{version_minor}.%{version_patch}
Release:		%{version_build}%{?dist}
License:		Commercial
URL:			http://forsys.ru
Group:			Applications/Multimedia
Source:			%{name}-%{version}.tgz
BuildRoot:		%_tmppath/%name-%version-%release-root
Requires:		libfsqt >= 1.23.1
BuildRequires:	cmake >= 2.8.0, libfsqt-devel
Provides:		%{name} = %{version}

%description
%{name} is excelport srvice

%prep
%setup -q -n %{name}-%{version}

%build
(cd src

mkdir -p build
cd build

# run cmake
cmake -D CMAKE_BUILD_TYPE=Release \
	-D excelport_srv_VERSION_MAJOR=%{version_major} \
	-D excelport_srv_VERSION_MINOR=%{version_minor} \
	-D excelport_srv_VERSION_PATCH=%{version_patch} \
	-D excelport_srv_VERSION_BUILD=%{version_build} \
	../

make %{?_smp_mflags}
)

%install
pwd
echo "RPM_BUILD_DIR=$RPM_BUILD_DIR"
echo "buildroot=%buildroot"
%__rm -rf %buildroot
%__mkdir_p %buildroot%{install_dir}

cd src

# service

%__cp -f build/excelport_srv %buildroot%{install_dir}/

# all configs
%__cp -f build/excelport_srv.xml %buildroot%{install_dir}/

%pre

%post

%preun

%clean
rm -rf %buildroot

%files
%defattr(0755,root,root,-)
%dir %attr(0755,root,root) %{install_dir}
%defattr(0755,root,root,-)
%{install_dir}/excelport_srv
%config %{install_dir}/excelport_srv.xml

%changelog

* Sun Dec 23 2018 Denis Podchernyaev <dpodch@gmail.ru> <0.1.0>
- init version