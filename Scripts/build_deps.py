#!/usr/bin/env python3

# (c) 2022 by Stephan Menzel
# Licensed under the Apache License, Version 2.0.
# See attached file LICENSE for full details
#

import argparse
import glob
import os
import subprocess
import datetime
import contextlib
import logging as log
import multiprocessing
import math
import shutil
import typing
import xml.etree.ElementTree as ET

# Versions to fetch and build
# Note that they _have_ to match together and make sense in their context
zlib_tag = "v1.2.13"
openssl_tag = "1_1_1s"
cares_tag = "1_18_1"
curl_tag = "7_86_0"
nlohmann_tag = "v3.11.2"
protobuf_tag = "v3.21.11"
abseil_tag = "20220623.1"
re2_tag = "2022-12-01"
crc32c_tag = "1.1.2"
grpc_tag = "v1.49.2"
gcloud_sdk_tag = "v2.5.0"
opentelemetry_tag = "v1.8.1"
aws_sdk_tag = "1.10.31"
cmake_config_flag = ""
cmake_toolset = ""

if os.name == "posix":
    zlib_static_lib_name = "libz.a"
    cmake_config_flag = "CMAKE_BUILD_TYPE=Release"
    cmake_toolset = ""
    lib_wildcard = "*.a"
    dll_wildcard = "*.so"
elif os.name == "nt":  # Windows
    zlib_static_lib_name = "zlibstatic.lib"
    cmake_config_flag = "CMAKE_CONFIGURATION_TYPES:STRING=Release"
    cmake_toolset = "-T v142"
    lib_wildcard = "*.lib"
    dll_wildcard = "*.dll"
else:
    print("Unsupported OS")
    exit(1)

cmake_global_flags = [
    ("BUILD_SHARED_LIBS:BOOL", "OFF"),
    ("BUILD_TESTING:BOOL", "OFF"),
    ("CMAKE_CXX_STANDARD:STRING", "17"),
    ("CMAKE_CXX_STANDARD_REQUIRED:BOOL", "ON"),
    ("CMAKE_MSVC_RUNTIME_LIBRARY:STRING", "MultiThreadedDLL"),
    ("CMAKE_POSITION_INDEPENDENT_CODE:BOOL", "ON"),

    ("PROTOBUF_USE_DLLS:BOOL", "OFF")
]

script_dir = os.path.dirname(os.path.realpath(os.path.abspath(__file__)))
parser = argparse.ArgumentParser()
parser.add_argument("--rebuild", action="store_true", help="always delete build dir")
parser.add_argument("--install_dir", type=str, nargs='?',
                    default=os.path.join(script_dir, 'install'))
parser.add_argument("--cc_dir", type=str, nargs='?', required=True)

args = parser.parse_args()
if os.path.isabs(args.install_dir):
    global_prefix = args.install_dir
else:
    global_prefix = os.path.join(script_dir, args.install_dir)

if not os.path.exists(args.cc_dir):
    print(f"CloudConnector dir {args.cc_dir} does not exist. Exiting")
    exit(1)

cc_dir = args.cc_dir

logfile = os.path.join(script_dir, 'build.log')
number_of_jobs = max(1, math.floor(multiprocessing.cpu_count()))


def file_and_console_log(*args, **kwargs):
    print(*args, **kwargs)
    with open(logfile, 'a') as f:
        f.write(*args)
        f.write('\n')


def print_banner(text: str):
    file_and_console_log("#########################################################")
    file_and_console_log("  " + text)
    file_and_console_log("#########################################################")


def run_in_shell(*args, **kwargs):
    cmd = ' '.join(args)
    print("> " + cmd)
    with open(logfile, 'a') as f:
        f.write('[cwd] ' + os.getcwd() + '\n')
        f.write(cmd + '\n')

    try:
        start = datetime.datetime.now()
        subprocess.run(*args, **kwargs, shell=True, check=True)
        with open(logfile, 'a') as f:
            f.write(f'  took {(datetime.datetime.now() - start).total_seconds()} seconds.\n\n')
    except Exception as e:
        file_and_console_log("Error: " + str(e))
        raise e


@contextlib.contextmanager
def pushd(new_dir):
    """temporarily go into subdir
        https://stackoverflow.com/questions/6194499/pushd-through-os-system"""

    previous_dir = os.getcwd()
    os.chdir(new_dir)
    try:
        yield
    finally:
        os.chdir(previous_dir)


def get_local_prefix(prefix):
    p = os.path.join(prefix, os.path.basename(os.getcwd()))
    if os.name == 'posix':
        if not "PKG_CONFIG_PATH" in os.environ:
            os.environ["PKG_CONFIG_PATH"] = ''
        os.environ["PKG_CONFIG_PATH"] += os.pathsep + os.path.join(p, 'lib', 'pkg_config')
        file_and_console_log("PKG_CONFIG_PATH = " + os.environ["PKG_CONFIG_PATH"])
    return p


def openssl_build_install(local_directory,
                          zlib_include_path: typing.Union[str, bytes, os.PathLike],
                          zlib_library_path: typing.Union[str, bytes, os.PathLike],
                          prefix=global_prefix, job_count=number_of_jobs):
    """OpenSSL is the only dependency with its own build type. It doesn't support
        CMake but requires Perl.
    """

    with pushd(local_directory):
        install_prefix = get_local_prefix(prefix)

        if not args.rebuild and os.path.exists("built_and_installed.txt"):
            file_and_console_log("already built, exiting")
            return install_prefix

        # This requires not only perl but also assumes we are in a "Developer Shell" on Windows,
        # meaning we have nmake in the path

        if os.name == "nt":
            # run_in_shell("C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\Common7\\Tools\\Launch-VsDevShell.ps1")

            run_in_shell("perl .\\Configure no-shared zlib no-zlib-dynamic threads no-unit-test "
                         f"--with-zlib-include={zlib_include_path} "
                         f"--with-zlib-lib={zlib_library_path} "
                         f"--prefix={install_prefix} --openssldir={install_prefix} "
                         "VC-WIN64A")

            # OpenSSL always assumes /MT when building statically. Looks like this cannot be overridden
            # So we change the makefile by replacing the occurrences
            with open('makefile', 'r') as file:
                filedata = file.read()

            # Replace the MT flags
            filedata = filedata.replace("/MT", "/MD")

            # Write the file out again
            with open('makefile', 'w') as lockfile:
                lockfile.write(filedata)

            # If you are here debugging why this doesn't work, you are probably not starting
            # this in a x64 native tools command prompt shell
            run_in_shell("nmake install")

            # I know, dirty, but building openssl takes for evah
            with open("built_and_installed.txt", "w") as lockfile:
                lockfile.write("built")

        elif os.name == "posix":
            run_in_shell("perl ./Configure no-shared zlib no-zlib-dynamic threads no-unit-test "
                         f"--with-zlib-include={zlib_include_path} "
                         f"--with-zlib-lib={zlib_library_path} "
                         f"--prefix={install_prefix} --openssldir={install_prefix} "
                         "")
            run_in_shell(f"make -j{job_count} install")

            with open("built_and_installed.txt", "w") as lockfile:
                lockfile.write("built")

    return install_prefix


def hack_opentelemetry_build(prefix: typing.Union[str, bytes, os.PathLike],
                             abseil_prefix: typing.Union[str, bytes, os.PathLike],
                             grpc_prefix: typing.Union[str, bytes, os.PathLike]):
    """Try to fix the broken OpenTelemetry build system. Missing include dependencies
        to absl and grpc were added during CMake but the resulting solution still contains
        one broken project. This modifies directly the offending "opentelemetry_proto"
        project by adding the dependencies there.
        Note that this is very ugly and prone to break at any moment.
        If you find yourself reading this, try to exclude this and see if the problem
        was fixed by upstream.
    """

    if os.name != "nt":
        return

    if "opentelemetry" not in prefix:
        return

    ET.register_namespace(prefix="", uri="http://schemas.microsoft.com/developer/msbuild/2003")
    tree = ET.parse("opentelemetry_proto.vcxproj")
    root = tree.getroot()
    for child in root:
        if "ItemDefinitionGroup" in child.tag:
            for idchild in child:
                if "ClCompile" in idchild.tag:
                    for clchild in idchild:
                        if "AdditionalIncludeDirectories" in clchild.tag:
                            new_text = ""
                            tokens = clchild.text.split(sep=";")
                            tokens.append(os.path.join(abseil_prefix, "include"))
                            tokens.append(os.path.join(grpc_prefix, "include"))

                            for t in tokens:
                                new_text += t + ";"
                            new_text = new_text[:-1]  # slice away last ";"
                            clchild.text = new_text

    tree.write("opentelemetry_proto.vcxproj")


def cmake_build_install(local_directory, prefix=global_prefix, cmake_args: list[tuple[str, str]] = [],
                        job_count=number_of_jobs,
                        # those are hacked in super ugly cause I want them to disappear as
                        # soon as OpenTelemetry fixes their build system
                        abseil_prefix: typing.Union[str, bytes, os.PathLike] = "",
                        grpc_prefix: typing.Union[str, bytes, os.PathLike] = ""):
    argstr = ""

    # Create flags string out of args tuples
    for key, value in cmake_global_flags:
        slash_value = value.replace("\\", "/")
        argstr += f"-D{key}={slash_value} "

    for key, value in cmake_args:
        slash_value = value.replace("\\", "/")
        argstr += f"-D{key}={slash_value} "

    build_folder = '_build'
    with pushd(local_directory):
        install_prefix = get_local_prefix(prefix)

        if not os.path.isdir(build_folder):
            os.makedirs(build_folder)
        with pushd(build_folder):
            if not args.rebuild and os.path.exists("built_and_installed.txt"):
                file_and_console_log("already built, exiting")
                return install_prefix
            run_in_shell(
                f"cmake .. {cmake_toolset} -D{cmake_config_flag} {argstr} -DCMAKE_INSTALL_PREFIX={install_prefix}")

            hack_opentelemetry_build(install_prefix, abseil_prefix, grpc_prefix)

            run_in_shell(f'cmake --build . --config Release --parallel {job_count}')
            run_in_shell(f'cmake --install .')

            with open("built_and_installed.txt", "w") as lockfile:
                lockfile.write("built")

    return install_prefix


def strip_unwanted_prefixes(git_tag: str):
    if git_tag.startswith('OpenSSL_'):
        return git_tag[8:]

    if git_tag.startswith('cares-'):
        return git_tag[6:]

    if git_tag.startswith('curl-'):
        return git_tag[5:]

    if git_tag.startswith('v'):
        return git_tag[1:]

    return git_tag


def clone_git_tag(git_repository, git_tag: str, backup_existing=False, delete_existing=False, recursive=False):
    git_repo_name = os.path.splitext(os.path.basename(git_repository))[0]

    os.makedirs('raw', exist_ok=True)
    local_dir = os.path.join('raw', git_repo_name.lower() + '-' + strip_unwanted_prefixes(git_tag).replace('_', '.'))

    exists = os.path.exists(local_dir)

    recursive_flag = "--recursive " if recursive else ""

    if exists:
        if delete_existing:
            shutil.rmtree(local_dir)
        elif args.rebuild:
            shutil.rmtree(os.path.join(local_dir, "_build"))
    elif exists and backup_existing:
        date_str = datetime.datetime.now().strftime("%Y%m%d")
        os.rename(local_dir, date_str + '_' + local_dir)
    else:
        log.info(f"Cloning {git_repository} ...")
        run_in_shell(
            f"git clone -c advice.detachedHead=false {recursive_flag} --depth 1 -b {git_tag} {git_repository} {local_dir}")

    return local_dir if os.path.isdir(local_dir) else None


def copy_libs(install_path: typing.Union[str, bytes, os.PathLike], destination: typing.Union[str, bytes, os.PathLike]):
    for libfile in glob.glob(os.path.join(install_path, "lib", lib_wildcard)):
        shutil.copy(libfile, destination)


if __name__ == '__main__':
    with open(logfile, 'w') as f:
        f.write(str(datetime.datetime.now()) + f' - started {__file__}\n')

    file_and_console_log("Installing to: " + global_prefix)

    # if os.name == "nt":
    # run_in_shell(
    #    "'C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\Common7\\Tools\\Launch-VsDevShell.ps1' -Arch amd64 -HostArch amd64")
    # run_in_shell(
    #     "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Auxiliary\\Build\\vcvars64.bat")

    print_banner("Building ZLIB")
    zlib_dir = clone_git_tag("https://github.com/madler/zlib.git", git_tag=zlib_tag)
    zlib_install_path = cmake_build_install(zlib_dir)

    print_banner("Building OpenSSL")
    openssl_dir = clone_git_tag("https://github.com/openssl/openssl.git",
                                git_tag=f"OpenSSL_{openssl_tag}", recursive=False)
    openssl_install_path = openssl_build_install(openssl_dir,
                                                 zlib_include_path=os.path.join(zlib_install_path, "include"),
                                                 zlib_library_path=os.path.join(zlib_install_path, "lib",
                                                                                zlib_static_lib_name))

    print_banner("Building CARES")
    cares_cmake_args = [
        ("CARES_SHARED:BOOL", "OFF"),
        ("CARES_STATIC:BOOL", "ON"),
        ("CARES_STATIC_PIC:BOOL", "ON"),
        ("CARES_MSVC_STATIC_RUNTIME:BOOL", "OFF")
    ]
    cares_dir = clone_git_tag("https://github.com/c-ares/c-ares.git", git_tag=f"cares-{cares_tag}")
    cares_install_path = cmake_build_install(cares_dir, cmake_args=cares_cmake_args)

    print_banner("Building CURL")
    curl_cmake_args = [
        ("BUILD_CURL_EXE", "OFF"),
        ("USE_LIBIDN2", "OFF"),
        ("CURL_USE_LIBPSL", "OFF"),
        ("CURL_USE_LIBSSH", "OFF"),
        ("CURL_USE_LIBSSH2", "OFF"),
        ("CURL_USE_OPENSSL", "OFF"),
        ("CURL_USE_SCHANNEL", "ON"),   # This may cause trouble on linux
        ("OPENSSL_ROOT_DIR:PATH", openssl_install_path),
        ("OPENSSL_USE_STATIC_LIBS:BOOL", "ON"),
        ("CURL_STATIC_CRT", "OFF"),
        ("ENABLE_ARES", "ON"),
        # ("c-ares_DIR", os.path.join(cares_install_path, "lib", "cmake", "c-ares")),
        ("CARES_INCLUDE_DIR", os.path.join(cares_install_path, "include")),
        ("CARES_LIBRARY", os.path.join(cares_install_path, "lib", "cares.lib")),
        ("ZLIB_ROOT", zlib_install_path),
        ("ZLIB_USE_STATIC_LIBS", "ON"),  # doesn't appear to do its job
        ("ZLIB_LIBRARY_RELEASE", os.path.join(zlib_install_path, "lib", zlib_static_lib_name)),
        ("ZLIB_LIBRARY_DEBUG", os.path.join(zlib_install_path, "lib", zlib_static_lib_name))  # We are on fake debug
    ]
    curl_dir = clone_git_tag('https://github.com/curl/curl.git', git_tag=f"curl-{curl_tag}", recursive=False)
    curl_install_path = cmake_build_install(curl_dir, cmake_args=curl_cmake_args)

    print_banner("Building NLohmann")
    nlohmann_cmake_args = [
        ("JSON_Install", "ON"),
        ("JSON_BuildTests", "OFF")
    ]
    nlohmann_dir = clone_git_tag('https://github.com/nlohmann/json.git', git_tag=nlohmann_tag)
    nlohmann_install_path = cmake_build_install(nlohmann_dir, cmake_args=nlohmann_cmake_args)

    # Now for the rough part. Google stuff. Buckle up!
    # Note that Google build systems are highly unstable and subject to
    # rapid unannounced changes, particularly on Windows. So if you have changed version tags
    # make sure the new generated versions have the same variables as before.
    print_banner("Building Protobuf")
    protobuf_cmake_args = [
        ("protobuf_BUILD_EXAMPLES", "OFF"),
        ("protobuf_BUILD_SHARED_LIBS", "OFF"),
        ("protobuf_BUILD_TESTS", "OFF"),
        ("protobuf_MODULE_COMPATIBLE", "ON"),
        ("protobuf_MSVC_STATIC_RUNTIME", "OFF"),
        ("protobuf_DISABLE_RTTI:BOOL", "ON"),
        ("ZLIB_ROOT:PATH", zlib_install_path),
        ("ZLIB_USE_STATIC_LIBS:BOOL", "ON"),  # doesn't appear to do its job
        ("ZLIB_LIBRARY_RELEASE:FILEPATH", os.path.join(zlib_install_path, "lib", zlib_static_lib_name)),
        ("ZLIB_LIBRARY_DEBUG:FILEPATH", os.path.join(zlib_install_path, "lib", zlib_static_lib_name))
        # We are on fake debug
    ]
    protobuf_dir = clone_git_tag("https://github.com/protocolbuffers/protobuf.git", git_tag=protobuf_tag,
                                 recursive=False)
    protobuf_install_path = cmake_build_install(protobuf_dir, cmake_args=protobuf_cmake_args)

    print_banner("Building Abseil")
    abseil_cmake_args = [
        ("ABSL_BUILD_TESTING", "OFF"),
        ("ABSL_ENABLE_INSTALL", "ON"),
        ("ABSL_PROPAGATE_CXX_STD", "ON"),
        ("ABSL_USE_EXTERNAL_GOOGLETEST", "OFF")
    ]
    abseil_dir = clone_git_tag("https://github.com/abseil/abseil-cpp.git", git_tag=abseil_tag, recursive=False)
    abseil_install_path = cmake_build_install(abseil_dir, cmake_args=abseil_cmake_args)

    print_banner("Building Re2 regular expression lib")
    re2_cmake_args = [
        ("RE2_BUILD_TESTING", "OFF")
    ]
    re2_dir = clone_git_tag("https://github.com/google/re2.git", git_tag=re2_tag, recursive=False)
    re2_install_path = cmake_build_install(re2_dir, cmake_args=re2_cmake_args)

    print_banner("Building Crc32c")
    crc32c_cmake_args = [
        ("CRC32C_BUILD_BENCHMARKS:BOOL", "OFF"),
        ("CRC32C_BUILD_TESTS:BOOL", "OFF"),
        ("CRC32C_USE_GLOG:BOOL", "OFF"),
        ("BENCHMARK_ENABLE_EXCEPTIONS:BOOL", "OFF"),
        ("BENCHMARK_ENABLE_TESTING:BOOL", "OFF"),
    ]
    crc32c_dir = clone_git_tag("https://github.com/google/crc32c.git", git_tag=crc32c_tag, recursive=False)
    crc32c_install_path = cmake_build_install(crc32c_dir, cmake_args=crc32c_cmake_args)

    print_banner("Building gRPC")
    grpc_cmake_args = [
        ("gRPC_MSVC_STATIC_RUNTIME:BOOL", "OFF"),
        ("gRPC_BUILD_CSHARP_EXT:BOOL", "OFF"),
        ("gRPC_BUILD_GRPC_CSHARP_PLUGIN:BOOL", "OFF"),
        ("gRPC_BUILD_GRPC_CPP_PLUGIN:BOOL", "ON"),
        ("gRPC_BUILD_GRPC_NODE_PLUGIN:BOOL", "OFF"),
        ("gRPC_BUILD_GRPC_OBJECTIVE_C_PLUGIN:BOOL", "OFF"),
        ("gRPC_BUILD_GRPC_PHP_PLUGIN:BOOL", "OFF"),
        ("gRPC_BUILD_GRPC_PYTHON_PLUGIN:BOOL", "OFF"),
        ("gRPC_BUILD_GRPC_RUBY_PLUGIN:BOOL", "OFF"),

        ("absl_DIR:PATH", os.path.join(abseil_install_path, "lib", "cmake", "absl")),
        ("gRPC_ABSL_PROVIDER", "package"),

        ("re2_DIR:PATH", os.path.join(re2_install_path, "lib", "cmake", "re2")),
        ("gRPC_RE2_PROVIDER", "package"),

        ("OPENSSL_ROOT_DIR:PATH", openssl_install_path),
        ("OPENSSL_USE_STATIC_LIBS:BOOL", "ON"),
        ("gRPC_SSL_PROVIDER", "package"),

        ("Protobuf_DIR", os.path.join(protobuf_install_path, "cmake")),
        ("Protobuf_CONFIG", os.path.join(protobuf_install_path, "cmake", "protobuf-config.cmake")),
        ("gRPC_PROTOBUF_PROVIDER", "package"),
        ("gRPC_PROTOBUF_PACKAGE_TYPE", "CONFIG"),

        ("c-ares_DIR:PATH", os.path.join(cares_install_path, "lib", "cmake", "c-ares")),
        ("gRPC_CARES_PROVIDER", "package"),
        ("CARES_STATIC:BOOL", "ON"),
        ("CARES_STATIC_PIC", "ON"),
        ("CARES_MSVC_STATIC_RUNTIME", "OFF"),

        ("Protobuf_USE_STATIC_LIBS", "ON"),
        ("protobuf_MSVC_STATIC_RUNTIME", "OFF"),

        ("gRPC_ZLIB_PROVIDER", "package"),
        ("ZLIB_ROOT", zlib_install_path),
        ("ZLIB_USE_STATIC_LIBS", "True"),
        ("ZLIB_LIBRARY_RELEASE", os.path.join(zlib_install_path, "lib", zlib_static_lib_name)),
        ("ZLIB_LIBRARY_DEBUG", os.path.join(zlib_install_path, "lib", zlib_static_lib_name))  # We are on fake debug
    ]

    # gRPC (or rather one of the dependencies, "upb") throws tons of warnings in MSVC.
    # I used to fix the places that caused them but now with that automated build I'd rather disable them.
    # This doesn't work however, even though the warnings show up as disabled in the VS solution.
    # If you know why, plz fix this.
    if os.name == "nt":
        grpc_cmake_args.append(("CMAKE_CXX_FLAGS", "\"/D_HAS_EXCEPTIONS=0 /wd4090 /wd4146 /wd4334 /wd5105\""))

    grpc_dir = clone_git_tag("https://github.com/grpc/grpc.git", git_tag=grpc_tag, recursive=False)
    grpc_install_path = cmake_build_install(grpc_dir, cmake_args=grpc_cmake_args)

    print_banner("Building Google Cloud SDK")
    gcloud_sdk_cmake_args = [
        ("absl_DIR:PATH", os.path.join(abseil_install_path, "lib", "cmake", "absl")),
        ("c-ares_DIR:PATH", os.path.join(cares_install_path, "lib", "cmake", "c-ares")),
        ("re2_DIR:PATH", os.path.join(re2_install_path, "lib", "cmake", "re2")),
        ("gRPC_DIR:PATH", os.path.join(grpc_install_path, "lib", "cmake", "grpc")),
        ("nlohmann_json_DIR:PATH", os.path.join(nlohmann_install_path, "share", "cmake", "nlohmann_json")),
        ("CURL_DIR:PATH", os.path.join(curl_install_path, "lib", "cmake", "CURL")),
        ("Crc32c_DIR:PATH", os.path.join(crc32c_install_path, "lib", "cmake", "Crc32c")),

        ("OPENSSL_ROOT_DIR:PATH", openssl_install_path),
        ("OPENSSL_USE_STATIC_LIBS:BOOL", "ON"),

        # The Google Cloud SDK build system is unable to find the installed Protobuf
        # package, for unknown reasons. None of the below seemed to do the trick.
        # I believe it's due to a mismatch in spelling. Protobuf installs its cmake
        # config using "protobufConfig" whereas the SDK looks for "ProtobufConfig"
        ("GOOGLE_CLOUD_CPP_DEPENDENCY_PROVIDER:STRING", "package"),
        ("GOOGLE_CLOUD_CPP_PROTOBUF_PROVIDER:STRING", "package"),
        ("Protobuf_DIR:PATH", os.path.join(protobuf_install_path, "cmake")),
        ("protobuf_DIR:PATH", os.path.join(protobuf_install_path, "cmake")),
        ("Protobuf_CONFIG:FILEPATH", os.path.join(protobuf_install_path, "cmake", "protobuf-config.cmake")),

        # I try to fix the situation by overriding everything
        ("Protobuf_INCLUDE_DIR:PATH", os.path.join(protobuf_install_path, "include")),
        ("Protobuf_LIBRARY_DEBUG:FILEPATH", os.path.join(protobuf_install_path, "lib", "libprotobuf.lib")),
        ("Protobuf_LIBRARY_RELEASE:FILEPATH", os.path.join(protobuf_install_path, "lib", "libprotobuf.lib")),
        ("Protobuf_LITE_LIBRARY_DEBUG:FILEPATH", os.path.join(protobuf_install_path, "lib", "libprotobuf-lite.lib")),
        ("Protobuf_LITE_LIBRARY_RELEASE:FILEPATH", os.path.join(protobuf_install_path, "lib", "libprotobuf-lite.lib")),
        ("Protobuf_PROTOC_LIBRARY_DEBUG:FILEPATH", os.path.join(protobuf_install_path, "lib", "libprotoc.lib")),
        ("Protobuf_PROTOC_LIBRARY_RELEASE:FILEPATH", os.path.join(protobuf_install_path, "lib", "libprotoc.lib")),
        ("Protobuf_PROTOC_EXECUTABLE:FILEPATH", os.path.join(protobuf_install_path, "bin", "protoc.exe")),

        # Some proto files appear to include stuff from protobuf itself.
        # This is what they seem to be looking for
        ("PROTO_INCLUDE_DIR:FILEPATH", os.path.join(protobuf_install_path, "include", "google", "protobuf")),

        ("protobuf_BUILD_SHARED_LIBS", "OFF"),
        ("protobuf_BUILD_TESTS", "OFF"),
        ("Protobuf_USE_STATIC_LIBS", "ON"),
        ("protobuf_MSVC_STATIC_RUNTIME", "OFF"),

        ("GOOGLE_CLOUD_CPP_ENABLE:STRING", "iam;logging;pubsub;storage"),
        ("GOOGLE_CLOUD_CPP_ENABLE_BIGQUERY:BOOL", "OFF"),
        ("GOOGLE_CLOUD_CPP_ENABLE_BIGTABLE:BOOL", "OFF"),
        ("GOOGLE_CLOUD_CPP_ENABLE_CCACHE:BOOL", "OFF"),
        ("GOOGLE_CLOUD_CPP_ENABLE_IAM:BOOL", "ON"),
        ("GOOGLE_CLOUD_CPP_ENABLE_LOGGING:BOOL", "ON"),
        ("GOOGLE_CLOUD_CPP_ENABLE_PUBSUB:BOOL", "ON"),
        ("GOOGLE_CLOUD_CPP_ENABLE_SPANNER:BOOL", "OFF"),
        ("GOOGLE_CLOUD_CPP_ENABLE_CXX_EXCEPTIONS:BOOL", "OFF"),

        ("gRPC_CPP_PLUGIN_EXECUTABLE:FILEPATH", os.path.join(grpc_install_path, "bin", "grpc_cpp_plugin.exe")),
        ("gRPC_ZLIB_PROVIDER", "package"),
        ("ZLIB_ROOT", zlib_install_path),
        ("ZLIB_USE_STATIC_LIBS", "True"),
        ("ZLIB_LIBRARY_RELEASE", os.path.join(zlib_install_path, "lib", zlib_static_lib_name)),
        ("ZLIB_LIBRARY_DEBUG", os.path.join(zlib_install_path, "lib", zlib_static_lib_name))  # We are on fake debug
    ]

    if os.name == "nt":
        gcloud_sdk_cmake_args.append(("CMAKE_CXX_FLAGS", "\"/D_HAS_EXCEPTIONS=0 \""))

    gcloud_sdk_dir = clone_git_tag("https://github.com/googleapis/google-cloud-cpp.git", git_tag=gcloud_sdk_tag,
                                   recursive=False)
    gcloud_sdk_install_path = cmake_build_install(gcloud_sdk_dir, cmake_args=gcloud_sdk_cmake_args)

    print_banner("Building OpenTelemetry C++ lib")
    opentelemetry_cmake_args = [
        ("WITH_EXAMPLES:BOOL", "OFF"),
        ("WITH_OTLP:BOOL", "ON"),
        ("WITH_OTLP_GRPC:BOOL", "ON"),
        ("WITH_OTLP_HTTP:BOOL", "ON"),
        ("WITH_STL:BOOL", "ON"),

        ("c-ares_DIR:PATH", os.path.join(cares_install_path, "lib", "cmake", "c-ares")),
        ("re2_DIR:PATH", os.path.join(re2_install_path, "lib", "cmake", "re2")),
        ("gRPC_DIR:PATH", os.path.join(grpc_install_path, "lib", "cmake", "grpc")),
        ("absl_DIR:PATH", os.path.join(abseil_install_path, "lib", "cmake", "absl")),
        ("WITH_ABSEIL:BOOL", "ON"),
        ("nlohmann_json_DIR:PATH", os.path.join(nlohmann_install_path, "share", "cmake", "nlohmann_json")),
        ("CURL_DIR:PATH", os.path.join(curl_install_path, "lib", "cmake", "CURL")),

        ("OPENSSL_ROOT_DIR:PATH", openssl_install_path),
        ("OPENSSL_USE_STATIC_LIBS:BOOL", "ON"),

        ("Protobuf_DIR:PATH", os.path.join(protobuf_install_path, "cmake")),
        ("Protobuf_PROTOC_EXECUTABLE:FILEPATH", os.path.join(protobuf_install_path, "bin", "protoc.exe")),

        # Some proto files appear to include stuff from protobuf itself.
        # This is what they seem to be looking for
        ("PROTO_INCLUDE_DIR:FILEPATH", os.path.join(protobuf_install_path, "include", "google", "protobuf")),

        # OpenTelemetry pulls in various includes from within grpc's include dir but doesn't add
        # them to the include paths. Injecting the flags will fix most of the
        # projects, except one: opentelemetry_proto, which is apparently not honoring those flags.
        ("CMAKE_CXX_FLAGS",
            f"/D_HAS_EXCEPTIONS=0 /I{os.path.join(grpc_install_path, 'include')} /I{os.path.join(abseil_install_path, 'include')}"),

        ("gRPC_CPP_PLUGIN_EXECUTABLE:FILEPATH", os.path.join(grpc_install_path, "bin", "grpc_cpp_plugin.exe")),
        ("gRPC_ZLIB_PROVIDER", "package"),
        ("ZLIB_ROOT", zlib_install_path),
        ("ZLIB_USE_STATIC_LIBS", "True"),
        ("ZLIB_LIBRARY_RELEASE", os.path.join(zlib_install_path, "lib", zlib_static_lib_name)),
        ("ZLIB_LIBRARY_DEBUG", os.path.join(zlib_install_path, "lib", zlib_static_lib_name))  # We are on fake debug
    ]
    opentelemetry_dir = clone_git_tag("https://github.com/open-telemetry/opentelemetry-cpp.git",
                                      git_tag=opentelemetry_tag,
                                      recursive=False)
    opentelemetry_install_path = cmake_build_install(opentelemetry_dir, cmake_args=opentelemetry_cmake_args,
                                                     abseil_prefix=abseil_install_path,
                                                     grpc_prefix=grpc_install_path)

    # So, compared to all this building and installing the AWS SDK will seem like a walk in the park.
    # Only the clone takes forever as I clone it recursively. I cannot be arsed to init all the
    # submodules manually. Feel free to submit a PR to optimize this
    print_banner("Building AWS SDK")
    aws_sdk_cmake_args = [
        ("AUTORUN_UNIT_TESTS:BOOL", "OFF"),
        ("ENABLE_TESTING:BOOL", "OFF"),
        ("ENABLE_RTTI:BOOL", "OFF"),
        ("CPP_STANDARD:STRING", "17"),
        ("CUSTOM_MEMORY_MANAGEMENT", "ON"),
        ("BUILD_DEPS:BOOL", "ON"),
        ("BUILD_SHARED_LIBS:BOOL", "ON"),  # DLLs are okay for the AWS SDK. I'll override the global flag
        ("BUILD_ONLY:STRING", "s3;sqs;xray;monitoring;logs;s3-crt;sns"),
        ("USE_OPENSSL:BOOL", "ON"),
        ("OPENSSL_ROOT_DIR:PATH", openssl_install_path),
        ("OPENSSL_USE_STATIC_LIBS:BOOL", "ON")
    ]

    aws_sdk_dir = clone_git_tag("https://github.com/aws/aws-sdk-cpp.git", git_tag=aws_sdk_tag, recursive=True)
    aws_sdk_install_path = cmake_build_install(aws_sdk_dir, cmake_args=aws_sdk_cmake_args)

    # Now, let's hope we have a more or less sane build here. Copy everything to the appropriate subdirs
    # at Cloud Connector. I assume CloudConnector itself to be versioned by git, so it should be safe
    # to just delete and replace everything to make sure we got a clean state
    print_banner(f"Copy binary artifacts to destination in CloudConnector directory {cc_dir}")
    cc_google_sdk_include_dir = os.path.join(cc_dir, "Source", "GoogleCloud_SDK", "include")
    cc_aws_sdk_include_dir = os.path.join(cc_dir, "Source", "AWS_SDK", "include")

    if os.name == "nt":
        cc_google_sdk_lib_dir = os.path.join(cc_dir, "Source", "GoogleCloud_SDK", "lib", "Win64")
        cc_aws_sdk_lib_dir = os.path.join(cc_dir, "Source", "AWS_SDK", "lib", "Win64")
        cc_aws_sdk_bin_dir = os.path.join(cc_dir, "Source", "AWS_SDK", "bin", "Win64")
    elif os.name == "posix":
        cc_google_sdk_lib_dir = os.path.join(cc_dir, "Source", "GoogleCloud_SDK", "lib", "Linux")
        cc_aws_sdk_lib_dir = os.path.join(cc_dir, "Source", "AWS_SDK", "lib", "Linux")
        cc_aws_sdk_bin_dir = os.path.join(cc_dir, "Source", "AWS_SDK", "bin", "Linux")

    # Delete everything
    shutil.rmtree(cc_google_sdk_include_dir, ignore_errors=True)
    shutil.rmtree(cc_google_sdk_lib_dir, ignore_errors=True)
    shutil.rmtree(cc_aws_sdk_include_dir, ignore_errors=True)
    shutil.rmtree(cc_aws_sdk_lib_dir, ignore_errors=True)
    shutil.rmtree(cc_aws_sdk_bin_dir, ignore_errors=True)

    # Create new paths
    if not os.path.exists(cc_google_sdk_include_dir):
        os.makedirs(cc_google_sdk_include_dir)
    if not os.path.exists(cc_google_sdk_lib_dir):
        os.makedirs(cc_google_sdk_lib_dir)
    if not os.path.exists(cc_aws_sdk_include_dir):
        os.makedirs(cc_aws_sdk_include_dir)
    if not os.path.exists(cc_aws_sdk_lib_dir):
        os.makedirs(cc_aws_sdk_lib_dir)
    if not os.path.exists(cc_aws_sdk_bin_dir):
        os.makedirs(cc_aws_sdk_bin_dir)

    # Copy all the Google Cloud SDK and dependency includes and libs
    for file in glob.glob(os.path.join(cares_install_path, "include", "*.h")):
        shutil.copy(file, cc_google_sdk_include_dir)
    shutil.copytree(os.path.join(abseil_install_path, "include", "absl"),
                    os.path.join(cc_google_sdk_include_dir, "absl"))
    shutil.copytree(os.path.join(re2_install_path, "include", "re2"),
                    os.path.join(cc_google_sdk_include_dir, "re2"))
    shutil.copytree(os.path.join(grpc_install_path, "include", "grpc"),
                    os.path.join(cc_google_sdk_include_dir, "grpc"))
    shutil.copytree(os.path.join(grpc_install_path, "include", "grpc++"),
                    os.path.join(cc_google_sdk_include_dir, "grpc++"))
    shutil.copytree(os.path.join(grpc_install_path, "include", "grpcpp"),
                    os.path.join(cc_google_sdk_include_dir, "grpcpp"))
    shutil.copytree(os.path.join(gcloud_sdk_install_path, "include", "google"),
                    os.path.join(cc_google_sdk_include_dir, "google"))
    shutil.copytree(os.path.join(protobuf_install_path, "include", "google", "protobuf"),
                    os.path.join(cc_google_sdk_include_dir, "google", "protobuf"))
    shutil.copytree(os.path.join(opentelemetry_install_path, "include", "opentelemetry"),
                    os.path.join(cc_google_sdk_include_dir, "opentelemetry"))

    copy_libs(cares_install_path, cc_google_sdk_lib_dir)
    copy_libs(crc32c_install_path, cc_google_sdk_lib_dir)
    copy_libs(re2_install_path, cc_google_sdk_lib_dir)
    copy_libs(zlib_install_path, cc_google_sdk_lib_dir)
    copy_libs(abseil_install_path, cc_google_sdk_lib_dir)
    copy_libs(openssl_install_path, cc_google_sdk_lib_dir)
    copy_libs(curl_install_path, cc_google_sdk_lib_dir)
    copy_libs(protobuf_install_path, cc_google_sdk_lib_dir)
    copy_libs(grpc_install_path, cc_google_sdk_lib_dir)
    copy_libs(gcloud_sdk_install_path, cc_google_sdk_lib_dir)
    copy_libs(opentelemetry_install_path, cc_google_sdk_lib_dir)

    # Copy all the AWS SDK includes, libs and DLLs
    shutil.copytree(os.path.join(aws_sdk_install_path, "include", "aws"),
                    os.path.join(cc_aws_sdk_include_dir, "aws"))
    copy_libs(aws_sdk_install_path, cc_aws_sdk_lib_dir)
    for libfile in glob.glob(os.path.join(aws_sdk_install_path, "bin", lib_wildcard)):
        shutil.copy(libfile, cc_aws_sdk_lib_dir)
    for dllfile in glob.glob(os.path.join(aws_sdk_install_path, "bin", dll_wildcard)):
        shutil.copy(dllfile, cc_aws_sdk_bin_dir)

    print_banner("Done")

    exit(0)
