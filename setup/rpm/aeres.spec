# Copyright 2018 - aeres.io - All Rights Reserved

%define _topdir      %(echo $RPMDIR)
%define name         %(echo $RPMNAME)
%define version      %(echo $RPMVER)
%define release      %(echo $RPMREL)

%if %(test "$(uname -m)" == "x86_64" && echo 1 || echo 0) 
%define libpath      lib64
%else
%define libpath      lib
%endif

Name:	          %{name}
Version:	      %{version}
Release:	      %{release}
Summary:        AERES client

Group:          Applications/Internet
# TODO: clean up the correct license type		
License:	      MIT
URL:		        https://www.aeres.io
Source0:	      %{_topdir}/SOURCES/%{name}-%{version}.tar.gz
BuildRoot:	    %(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)

%description
Client component of AERES.

%prep
%setup -q


%build


%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/opt/aeres
mkdir -p $RPM_BUILD_ROOT/usr/bin
mkdir -p $RPM_BUILD_ROOT/usr/%{libpath}
cp aeres $RPM_BUILD_ROOT/opt/aeres
cp libquic.so $RPM_BUILD_ROOT/opt/aeres
ln $RPM_BUILD_ROOT/opt/aeres/aeres $RPM_BUILD_ROOT/usr/bin
ln $RPM_BUILD_ROOT/opt/aeres/libquic.so $RPM_BUILD_ROOT/usr/%{libpath}


%post


%postun


%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root,-)
%doc
/opt/aeres/aeres
/opt/aeres/libquic.so
/usr/bin/aeres
/usr/%{libpath}/libquic.so


%changelog

