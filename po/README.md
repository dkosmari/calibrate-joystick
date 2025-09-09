# Basics

[`POTFILES.in`](POTFILES.in) contains a list of files that are scanned for translation.

The "`*.pot`" file is the template file with the original messages. It's generated
automatically from the files listed in `POTFILES.in`.

The "`*.po`" files contain the translations. They're text files, and can be edited with
any text editor; a specialized translations editor like [Poedit](https://poedit.net/) is
recommended.

The "`*.po`" files must be compiled to a binary format, as "`*.mo`" files. GNU Gettext
sometimes uses the "`*.gmo`" extension instead. The binary files must be installed for the
program to locate them, not the "`*.po`" text files.


## Updating translations:

Inside the "`po`" directory, run:

    make update-po

This will update the "`*.pot`" file, merge any changes with all "`*.po`" files, then
compile them all. If the a compiled translation is not present, it will not be installed.


## Adding new translations:

Edit the "LINGUAS" file to add the new language. See the GNU Gettext manual for a list of
[language codes](https://getdocs.org/Gettext/Language-Codes#Language-Codes).

Then run the command:

    msginit -l CODE

where CODE is the language code you added. This will create an empty "CODE.po" translation
file.

> Note: if you use a special translation editor like Poedit, it can also create a new
> translation file from the "`*.pot`" template.


## Testing translations:

Gettext translation files must be organized inside specific directories. Therefore, **you
need to install** the program before the translations can be used.

To avoid doing a system install every time you want to test the translation, it's useful
to configure a "local install" folder like this:

    ./configure --prefix=$PWD/test-install
    make
    make install
    cd test-install
    export GSETTINGS_SCHEMA_DIR=$PWD/share/glib-2.0/schemas

When the program is run, it will correctly load the translation files as specified in the
"LANGUAGE" environment variable:

    LANGUAGE=fr bin/calibrate-joystick
