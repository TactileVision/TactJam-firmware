# TactJam-firmware

![TactJam setup](https://github.com/TactileVision/TactJam-firmware/wiki/img/TactJam-Teaser.jpg)

TactJam is a collaborative playground for composing spatial tactons. The project is split into the following repositories:

+ Hardware interface: [https://github.com/TactileVision/TactJam-hardware](https://github.com/TactileVision/TactJam-hardware)
+ Firmware driving the hardware: [https://github.com/TactileVision/TactJam-firmware](https://github.com/TactileVision/TactJam-firmware)
+ GUI client to view and edit tactons: [https://github.com/TactileVision/TactJam-client](https://github.com/TactileVision/TactJam-client)
+ Server to store and manage tactons: [https://github.com/TactileVision/TactJam-server](https://github.com/TactileVision/TactJam-server)

If you are looking for the software that drives the device, you have come to the right place. Here you find the firmware and a test suite to run isolated tests for several hardware and software components.


## How to build and develop the firmware

The TactJam hardware is based on the [ESP32 MCU by Espressif](https://www.espressif.com/en/products/socs/esp32/overview) (you might need this [driver](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers) to detect it on Windows). To simplify the embedded development we use [PlatformIO](https://platformio.org/). For the sake of convience the code editor [VSCodium](https://vscodium.com/) offers a package for PlatformIO. Please refer to the [installation guide](https://docs.platformio.org/en/latest/integration/ide/vscode.html#installation) for details. Of course you can also use other editors (see the [list of supported editors](https://docs.platformio.org/en/latest/integration/ide/index.html#desktop-ide)), or use only the [CLI](https://docs.platformio.org/en/latest/core/index.html). The firmware code includes [libvtp](https://github.com/lhinderberger/libvtp) as submoule. To include submodules during clone you can use the command `git clone --recurse-submodules https://github.com/TactileVision/TactJam-firmware.git`.



### Configuration

The build configuration of the PlatformIO project is stored in the _platformio.ini_ (see below). Please change the `upload_port` to the correct port of your ESP and the `monitor_speed` according to your needs.

```
[env:tactjam]
platform = espressif32
board = esp32dev
framework = arduino
upload_port = /dev/cu.SLAB_USBtoUART
monitor_speed = 115200
```


## How to contribute

Anyone with the passion for free software is welcome to contribute to this project by:

+ 👩‍💻 developing software
+ 👾 filing any [issues](https://github.com/TactileVision/TactJam-firmware/issues)  or suggesting new features
+ 🧑‍🏭 sending [pull requests](https://github.com/TactileVision/TactJam-firmware/pulls) for fixed bugs or new features

Before you start, please take a look at the project [board](https://github.com/orgs/TactileVision/projects/1) and the [issues](https://github.com/TactileVision/TactJam-firmware/issues). Maybe there is already a similar bug or feature request that is already under construction and may need your expertise and support.


### Branching model

In 2010 Vincent Driessen wrote a nice blog post where he introduced a branching model called [git-flow](https://nvie.com/posts/a-successful-git-branching-model/). Over the years it became very popular and was used in many projects. Meanwhile GitHub introduced a much simpler workflow called [GitHub flow](https://guides.github.com/introduction/flow/). Both approaches have their pros and cons. That’s why we use a combination – git-flow as the branching model and the pull request workflow suggested in GitHub flow.


### How to commit

To make contribution easier for everyone we like use a common structure of git commit messages: 

```
<type>[optional scope]: <description>
[optional body]
[optional footer(s)]
```

Please refer to [www.conventionalcommits.org](https://www.conventionalcommits.org/en/v1.0.0/) for more information.


### Code style

This might not be the world’s largest code base. However, a consistent code style makes it easier to read and maintain. The people at Google are very experienced in this and have published their [guidelines](https://google.github.io/styleguide/) for different languages. For this project we want to orientate ourselves by this (e.g. [C++ style guide](https://google.github.io/styleguide/cppguide.html) for firmware code).


## Copyright

TactJam-hardware is (C) 2020 Tactile Vision

It is licensed under the MIT license. For details, please refer to the [LICENSE](LICENSE) file.
