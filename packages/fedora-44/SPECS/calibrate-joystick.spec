Name:           calibrate-joystick
Version:        0.6.1
Release:        1%{?dist}
Summary:        A joystick calibration program.
Group:          System/Kernel and hardware

License:        GPLv3+
URL:            https://github.com/dkosmari/%{name}
Source0:        https://github.com/dkosmari/%{name}/releases/download/v%{version}/%{name}-%{version}.tar.gz

BuildRequires:  pkgconfig(libevdev)
BuildRequires:  pkgconfig(gudev-1.0)
BuildRequires:  pkgconfig(gtkmm-3.0)


%description
%{name} is a graphical program to calibrate joysticks. Just select the device, move the
stick around, and apply the calculated minimum and maximum range.


%prep
%autosetup

%build
%configure --disable-system-libevdevxx --disable-system-libgudevxx
%make_build

%install
%make_install

%find_lang %{name}


%files -f %{name}.lang
%license COPYING
%doc README.md
%{_bindir}/calibrate-joystick
%{_datadir}/calibrate-joystick/*.gresource
%{_datadir}/applications/*.desktop
%{_datadir}/glib-2.0/schemas/*.gschema.xml
