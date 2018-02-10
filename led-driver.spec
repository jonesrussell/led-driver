# LED Driver RPM spec
Name: led-driver
Version: 1.1
URL: http://sokoloski.ca/
Release: 3%{dist}
Vendor: Darryl Sokoloski <darryl@sokoloski.ca>
License: GPL
Group: Applications/Multimedia
Packager: Darryl Sokoloski <darryl@sokoloski.ca>
Source: %{name}-%{version}.tar.gz
BuildRoot: /var/tmp/%{name}-%{version}
Summary: Generic LED Driver with Lightpack support.
Requires: libusbx
%{?systemd_requires}
BuildRequires: libusbx-devel glibc-headers

%description
Generic LED Driver with Lightpack support.
For more information, visit: http://sokoloski.ca/

# Build
%prep
%setup -q
./autogen.sh
%{configure}

%build
make %{?_smp_mflags}

# Install
%install
make install DESTDIR=%{buildroot}
install -d -m 0755 %{buildroot}/run/%{name}
install -D -m 0644 deploy/%{name}.tmpf %{buildroot}/%{_tmpfilesdir}/%{name}.conf

# Clean-up
%clean
[ "%{buildroot}" != "/" ] && rm -rf %{buildroot}

%post
%systemd_post %{name}.service

%preun
%systemd_preun %{name}.service

%postun
%systemd_postun_with_restart %{name}.service

# Files
%files
%defattr(-,root,root)
%dir /run/%{name}
%{_bindir}/led-ctrl
%{_sbindir}/led-server
%{_libdir}/lib%{name}.*
%config %attr(0644,root,root) %{_tmpfilesdir}/%{name}.conf
%config %attr(0644,root,root) %{_unitdir}/%{name}.service

# Change log
%changelog

# vi: expandtab shiftwidth=4 softtabstop=4 tabstop=4
