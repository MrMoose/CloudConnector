# CloudConnector Dependencies

#### Experimental Build Scripts

## About

CloudConnector contains its dependencies in binary form.
This may seem unusual but is quite ordinary in the Unreal world.
However, some users may not trust link dependencies coming 
from some Unreal plugin and may want to build their own.
Or need to modify some. Or at least reproduce how those 
binaries are made.

To this end we have started to include this script, which is
the reference way to build all binary dependencies. On Windows
to start with but Linux is slated to follow soon.

Note that building this dependencies is a very brittle and lengthy
process with lots of room for errors. Therefore this is only
recommended for power users.

## Usage

In order to use this script on Windows 10 you need:

* [Python](https://www.python.org/downloads/) (3.9 is the one I used)
* Perl ([StrawberryPerl](https://strawberryperl.com) works well)
* [CMake](https://cmake.org/download/)
* Microsoft Visual Studio 2019, latest patches
* Quite a bit of disc space

CMake, Perl and Python need to be in the PATH for your user.
A virtualenv is not required.

Then open a "x64 Native Tools Command Prompt" shell. 
*Any other shell will not work or produce faulty results*.

Use a command like this

```sh
 > cd C:\your_workdir\CloudConnector\Scripts
 > python .\build_deps.py --cc_dir=C:\your_workdir\CloudConnector
```

The script will build all the dependencies and install them
in the provided CloudConnector tree. Previously installed binary and 
header deps will be implicitly removed.

## Disclaimer

This is highly experimental. Use it at you own risk.
Feel free to provide feedback to CloudConnector but please
do not file bug reports if you use this.
