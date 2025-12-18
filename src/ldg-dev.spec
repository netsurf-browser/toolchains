# LDG-dev specfile

Summary       : LDG development library and header file
Name          : ldg-dev
Version       : 2.34
Release       : 1
Copyright     : LGPL
Group         : Development/Libraries

Packager      : Arnaud Bercegeay <arnaud.bercegeay@free.fr>
Vendor        : Sparemint
URL           : http://ldg.sf.net

Prefix        : %{_prefix}
Docdir        : %{_prefix}/doc
BuildRoot     : %{_tmppath}/%{name}-root

Source: %{name}-%{version}.tar.gz


%description
Contains the LDG library and header file required to build a LDG
library, or an application that use LDG libraries.

%prep
%setup -q -n %{name}-%{version}


%build
cd src/devel
make -f gcc.mak


%install
[ "${RPM_BUILD_ROOT}" != "/" ] && rm -rf ${RPM_BUILD_ROOT}

mkdir -p ${RPM_BUILD_ROOT}%{_prefix}/include
mkdir -p ${RPM_BUILD_ROOT}%{_prefix}/lib
cd lib/gcc
install -m 644 libldg.a ${RPM_BUILD_ROOT}%{_prefix}/lib
cd ../../include
install -m 644 ldg.h ${RPM_BUILD_ROOT}%{_prefix}/include


%clean
[ "${RPM_BUILD_ROOT}" != "/" ] && rm -rf ${RPM_BUILD_ROOT}


%files
%defattr(-,root,root)
#%doc gemlib/ChangeLog*
%{_prefix}/lib/lib*.a
%{_prefix}/include/*.h


%changelog
* Wed Nov 02 2005 Arnaud Bercegeay <arnaud.bercegeay@free.fr>
- Initial version of the ldg-dev package
