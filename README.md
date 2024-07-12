# csas
csas is a console file manager written in C. It is heavily inspired by [ranger](https://github.com/ranger/ranger) and [lf](https://github.com/gokcehan/lf).

![multicol-screenshot](https://i.imgur.com/bswcTiS.png)
![singlecol-screenshot](https://i.imgur.com/OiZy6ft.png)

## Design Goals

 - fast and low memory consumption file loading
 - expandability

## Features

 - Privacy-aware (no unconfirmed user data collection)
 - Multi-column display
 - Preview of the selected file/directory
 - Customizable keybindings (vi and readline defaults)
 - Tiny binary (typically around 130KB)
 - No FPU usage
 - UTF-8 Support
 - Hackable

## Installation
    make install

You can also install example configuration by running:

    cp csasrc ~/.config/csasrc
