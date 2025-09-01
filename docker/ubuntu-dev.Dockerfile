# Ubuntu development environment for C++ builds
# This container provides a complete C++ development environment with modern tools

FROM ubuntu:24.04

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=UTC
ENV CC=clang-15
ENV CXX=clang++-15

# Install system dependencies
RUN apt-get update && apt-get install -y \
    # Build essentials
    build-essential \
    cmake \
    ninja-build \
    pkg-config \
    # Compilers
    gcc-12 \
    g++-12 \
    clang-15 \
    clang++-15 \
    # Cross-compilation toolchains
    gcc-aarch64-linux-gnu \
    g++-aarch64-linux-gnu \
    gcc-arm-linux-gnueabihf \
    g++-arm-linux-gnueabihf \
    # Development tools
    git \
    curl \
    wget \
    unzip \
    tar \
    gzip \
    # Build optimization tools
    ccache \
    distcc \
    # Static analysis tools
    clang-tidy-15 \
    clang-format-15 \
    cppcheck \
    # Documentation tools
    doxygen \
    graphviz \
    # Testing tools
    valgrind \
    gdb \
    # Libraries
    libssl-dev \
    libcurl4-openssl-dev \
    zlib1g-dev \
    libbz2-dev \
    liblzma-dev \
    # Python for build scripts
    python3 \
    python3-pip \
    python3-venv \
    # Additional utilities
    vim \
    nano \
    htop \
    tree \
    && rm -rf /var/lib/apt/lists/*

# Install vcpkg
RUN git clone https://github.com/Microsoft/vcpkg.git /opt/vcpkg \
    && /opt/vcpkg/bootstrap-vcpkg.sh \
    && ln -s /opt/vcpkg/vcpkg /usr/local/bin/vcpkg

# Install Conan
RUN pip3 install conan

# Install latest CMake
RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null \
    && echo 'deb https://apt.kitware.com/ubuntu/ jammy main' | tee /etc/apt/sources.list.d/kitware.list >/dev/null \
    && apt-get update \
    && apt-get install -y cmake \
    && rm -rf /var/lib/apt/lists/*

# Configure ccache
RUN mkdir -p /root/.ccache \
    && echo "max_size = 5.0G" > /root/.ccache/ccache.conf \
    && echo "compression = true" >> /root/.ccache/ccache.conf \
    && echo "compression_level = 6" >> /root/.ccache/ccache.conf

# Set up environment
ENV VCPKG_ROOT=/opt/vcpkg
ENV PATH="/opt/vcpkg:${PATH}"
ENV CCACHE_DIR=/root/.ccache

# Create workspace directory
WORKDIR /workspace

# Set default shell
SHELL ["/bin/bash", "-c"]

# Add build script
COPY docker/scripts/build.sh /usr/local/bin/build
RUN chmod +x /usr/local/bin/build

# Add entrypoint script
COPY docker/scripts/entrypoint.sh /usr/local/bin/entrypoint
RUN chmod +x /usr/local/bin/entrypoint

# Health check
HEALTHCHECK --interval=30s --timeout=10s --start-period=5s --retries=3 \
    CMD cmake --version && clang --version || exit 1

# Labels
LABEL maintainer="cpp-scaffold"
LABEL description="Ubuntu C++ development environment"
LABEL version="1.0"

ENTRYPOINT ["/usr/local/bin/entrypoint"]
CMD ["/bin/bash"]
