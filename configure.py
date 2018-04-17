#! /usr/bin/env python

# Simple scipt for configure cmake build

import os
import platform
import shutil
import subprocess as sp

from optparse import OptionParser, OptionGroup
from os.path import join

from logging import (
    info, error,
    Formatter, StreamHandler, getLogger,
    INFO, WARN, ERROR, DEBUG)

CHOICE_LOG_LEVELS = ('INFO', 'WARN', 'ERROR', 'DEBUG')
DEFAULT_LOG_LEVEL = CHOICE_LOG_LEVELS[0]
LOG_LEVELS = dict(zip(CHOICE_LOG_LEVELS, (INFO, WARN, ERROR, DEBUG)))

CHOICE_FLAVOR = ('Release', 'Debug')
DEFAULT_FLAVOR = CHOICE_FLAVOR[0]


def normpath(path):
    """Return a normalized absolutized version of the pathname path."""

    return os.path.normpath(
        os.path.join(os.getcwd(), os.path.expanduser(path)))


def guessed_platform():
    """Guess platform"""

    sys_name = platform.system().lower()
    if (sys_name.find('win') >= 0):
        sys_name = 'win'
    archi = platform.architecture()
    if archi[0].find('64') >= 0:
        archi = '64'
    else:
        archi = '32'
    return "".join((sys_name, archi)).lower()


#
# Some staf for prety log
#
class ColoredFormatter(Formatter):
    """A logger formatter coloring the outputs."""

    def format(self, record):

        record.levelcolor = {INFO: 32, WARN: 33,
                             ERROR: 31, DEBUG: 30}[record.levelno]
        return super(ColoredFormatter, self).format(record)


def set_logger(log_level):
    """Set the script logger."""

    platform = guessed_platform()

    level = LOG_LEVELS.get(log_level, DEFAULT_LOG_LEVEL)
    formatting = "[%(levelname)s] %(message)s"
    if platform.startswith('linux'):
        formatting = "\033[0;%(levelcolor)sm" + formatting + "\033[0m"
    formatter = ColoredFormatter(formatting)
    handler = StreamHandler(None)
    handler.setLevel(level)
    handler.setFormatter(formatter)
    logger = getLogger()
    logger.setLevel(level)
    logger.addHandler(handler)


def cmake_onoff(boolean):
    """convert bool to ON/OFF"""

    if (boolean):
        return "ON"
    else:
        return "OFF"


def get_default_cmake_generator():
    "Return the default CMake gemerator or None"

    if "win" in guessed_platform():
        return "NMake Makefiles"
    return None


usage = """usage: %prog [options] arg1 arg2 [cmake options]

This script provide a easy way for build Amelet-HDF C library. The main
behaviour is to run cmake with right command line options.

This scipt can be also used with QtCreator developpement
environement. QtCreator can use cmake configure file for build a
project. However, it's hard to remember the command line to build QtCreator
project. This scipt can help you with the dry mode (see "--drymode").

# %prog [options] --drymode

Copy/colle the cmake command line into QtCreator for build the project.
"""


def main():
    platform = guessed_platform()
    parser = OptionParser(usage=usage)

    group = OptionGroup(parser, "General Configure Options", "")
    group.add_option("--build", dest="build", default=None,
                     help="Where is build the application")
    group.add_option("--prefix", dest="prefix", default=None,
                     help="Where is installed the application")
    group.add_option("--flavor", dest="flavor", default=None,
                     choices=CHOICE_FLAVOR,
                     help="build flavor id (default: %(default)s)")
    parser.add_option_group(group)

    group = OptionGroup(parser, "Manage dependencies", "")
    group.add_option("--hdf5-dir", dest="hdf5",
                     help="path to the HDF5 cmake directory "
                     "'${SOMEWHERE}/share/cmake/hdf5'",
                     metavar="PATH", default=None)
    parser.add_option_group(group)

    group = OptionGroup(parser, "Manage features", "")
    group.add_option("--build-docs", dest="docs",
                     help="Build library documentation",
                     action="store_true", default=False)
    parser.add_option_group(group)

    group = OptionGroup(parser, "CMake", "")
    group.add_option("--cmake-generator",
                     dest="cmake_generator",
                     default=get_default_cmake_generator(),
                     help="Define the generator used by CMake")
    group.add_option("--cmake-args",
                     dest="cmake_args", default="",
                     help="The value of this option are directly provide to camke")
    parser.add_option_group(group)

    group = OptionGroup(parser, "Control", "")
    group.add_option("--clean",
                     action="store_true", dest="clean", default=False,
                     help="Clean build directory before do something")
    group.add_option("--drymode",
                     action="store_true", dest="drymode", default=False,
                     help="Build environement en display info for outside "
                     "configuration")
    group.add_option('--log-level', dest='log_level',
                     default=DEFAULT_LOG_LEVEL,
                     choices=CHOICE_LOG_LEVELS,
                     help="the logging level (default: %(default)s)")
    parser.add_option_group(group)

    (options, user_args) = parser.parse_args()
    set_logger(options.log_level)

    # Build cmake options into a dict.
    cmake_options = {}

    # Configure cmake with adk
    if options.hdf5 is not None:
        hdf5_path = normpath(options.hdf5)
        cmake_options.setdefault("HDF5_DIR:PATH", hdf5_path)

    if options.prefix is not None:
        cmake_options.setdefault("CMAKE_INSTALL_PREFIX:PATH",
                                 normpath(options.prefix))

    if options.docs is not None:
        cmake_options.setdefault(
            "AMELETHDF_BUILD_DOCS:BOOL", cmake_onoff(options.docs))

    if options.flavor is not None:
        cmake_options.setdefault("CMAKE_BUILD_TYPE", options.flavor)
        cmake_options.setdefault(
            "AMELETHDF_ENABLE_COVERAGE:BOOL",
            cmake_onoff(options.flavor == "Debug"))

    args = ["-D%s=%s" % (name, val) for name, val in cmake_options.items()]
    args += user_args

    # configure cmake
    if options.cmake_args:
        args += options.cmake_args.split(" ")
    if options.cmake_generator:
        args.extend(("-G", options.cmake_generator))

    # deal with build dir.
    srcdir = None
    if options.build is not None:
        srcdir = os.getcwd()
        builddir = normpath(options.build)
        if not os.path.exists(builddir):
            os.makedirs(builddir)
        elif options.clean:
            for x in os.listdir(builddir):
                content = os.path.join(builddir, x)
                if os.path.isfile(content):
                    os.remove(content)
                else:
                    shutil.rmtree(content)
        os.chdir(builddir)
        args.insert(0, srcdir)
        info("Mustard should be build here '%s'." % options.build)

    args.insert(0, "cmake")
    info("# %s" % (" ".join(args)))
    if (not options.drymode):
        info("Configure Amelet-HDF C library")
        info("Cmake stdout:")
        if (sp.Popen(args, stdout=sp.PIPE).wait() != 0):
            error("Configure finiche with an error!")
        info("Cmake end")
        info("For build teta run the folowin command.")
        if srcdir is not None:
            os.chdir(srcdir)
            info("# cd %s" % options.build)
            info("# make")
            info("# make test")
            info("# make docs")
            info("# make coverage")

        info("For adjust configuration or reconfigure used in build dir:")
        info("# cmake-gui .")
        info("# cmake .")


if __name__ == "__main__":
    main()
