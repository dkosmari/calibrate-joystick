calibrate-joystick - a tool to calibrate joysticks on Linux systems
===================================================================

This is a graphical program to quickly calibrate range and deadzones of joysticks.


Usage
-----

![calibrate-joystick main window](screenshots/screenshot-1.png)

  - Run the program.

  - On the application window, select the desired input device.

  - Move the sticks to their maximum range. This will be detected as the new minimum and
    maximum ranges.
  
  - If needed, manually adjust the **Flat** parameter; that is the *dead zone*.
  
  - Click **Apply**.

  - Customization of joysticks is not permanent; it's lost when the device is unplugged,
    or when the system reboots.

Note: input devices are enumerated through udev. Only devices with the property
`ID_INPUT_JOYSTICK=1` are processed. If your joystick isn't shown, use this command to
inspect it:

    udevadm info /dev/input/eventN

where `eventN` is the desired device. Udev uses heuristics to guess which devices are
joysticks, but this can be overridden with udev rules.

Note: for security reasons, udev only allows users direct access to devices with the
`uaccess` tag. If your device was not tagged with `uaccess`, either create a custom udev
rule to tag it with `uaccess`, or run calibrate-joystick with sudo/root permissions.

The program can also be run as a daemon:

    calibrate-joystick -d

The main window will stay hidden until an input device is inserted. Closing the window
won't stop the daemon, it must be explicitly closed through the **Quit daemon** button.



Dependencies
------------

This program requires a C++17 compiler. Additionally, it uses the following libraries:

- [gtkmm-3.0](http://gtkmm.org)

- [libevdev](http://www.freedesktop.org/wiki/Software/libevdev)

- [libgudev](http://wiki.gnome.org/Projects/libgudev)

- [libevdevxx](http://github.com/dkosmari/libevdevxx)

- [libgudevxx](http://github.com/dkosmari/libgudevxx)


Build instructions
------------------

If building from the tarball, execute these commands:

    ./configure
    make
    sudo make install

If building from a cloned repository, you need to run the `./bootstrap` script once,
before proceeding with the tarball steps.

This software uses Automake, so the standard Automake build options apply. Check the
[INSTALL](INSTALL) file and the `./configure --help` command for more details.
