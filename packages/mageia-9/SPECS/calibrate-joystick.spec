Name:           calibrate-joystick
Version:        0.6.0
Release:        %mkrel 1
Summary:        A joystick calibration program.
Group:          System/Kernel and hardware

License:        GPLv3+
URL:            https://github.com/dkosmari/%{name}
Source0:        https://github.com/dkosmari/%{name}/releases/download/v%{version}/%{name}-%{version}.tar.gz

BuildRequires:  pkgconfig(libevdevxx)
BuildRequires:  pkgconfig(libgudevxx)
BuildRequires:  pkgconfig(gtkmm-3.0)


%description
%{name} is a graphical program to calibrate joysticks. Just select the device, move the
stick around, and apply the calculated minimum and maximum range.


%prep
%autosetup

%build
%configure 
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
