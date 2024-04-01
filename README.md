# XBLC Dumper.
Dump the firmware off an Original Xbox Live Communicator straight from the console. The communicator contains a RISC-based micro-controller, which appears to use the V8-uRISC architecture. Public datasheets for the chipset indicate it has a 12KB programmable ROM. It appears a custom ROM was developed for the XBLC as it does not follow the standard USB Audio Class Specification.

* See https://xboxdevwiki.net/Xbox_Live_Communicator
* See https://web.archive.org/web/20200521011406/http://www.kako.com/neta/2005-009/uac3556b.pdf
* SHA1 implementation from https://github.com/CTrabant/teeny-sha1. (MIT license)

## Compile
Setup and install [nxdk](https://github.com/XboxDev/nxdk) then:
```
git clone https://github.com/Ryzee119/Dongle_Dumper.git
make NXDK_DIR=/path/to/nxdk
```

## Usage
* Copy the `default.xbe` to a folder on your xbox and launch it. See [releases](https://github.com/Ryzee119/Xblc_Dumper/releases).
* Connect a Xbox Live Communicator.
* The firmware file will be written to the same directory as the xbe.
