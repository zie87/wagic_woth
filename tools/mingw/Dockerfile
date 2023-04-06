FROM fedora:37

RUN dnf update -y \
    && dnf install -y meson ninja-build \
    && dnf install -y mingw64-gcc-c++ mingw64-boost \
    && dnf install -y mingw32-gcc-c++ mingw32-boost \
    && dnf install -y wine \
    && dnf clean all
