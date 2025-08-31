# Alpine minimal development environment for C++ builds
# This container provides a lightweight C++ development environment

FROM alpine:3.18

# Set environment variables
ENV CC=clang
ENV CXX=clang++

# Install system dependencies
RUN apk add --no-cache \
    # Build essentials
    build-base \
    cmake \
    ninja \
    pkgconfig \
    # Compilers
    gcc \
    g++ \
    clang \
    clang-dev \
    # Development tools
    git \
    curl \
    wget \
    unzip \
    tar \
    gzip \
    # Build optimization tools
    ccache \
    # Static analysis tools
    clang-extra-tools \
    # Libraries
    openssl-dev \
    curl-dev \
    zlib-dev \
    bzip2-dev \
    xz-dev \
    # Python for build scripts
    python3 \
    py3-pip \
    # Additional utilities
    bash \
    vim \
    nano \
    htop \
    tree

# Install vcpkg
RUN git clone https://github.com/Microsoft/vcpkg.git /opt/vcpkg \
    && /opt/vcpkg/bootstrap-vcpkg.sh \
    && ln -s /opt/vcpkg/vcpkg /usr/local/bin/vcpkg

# Install Conan
RUN pip3 install conan

# Configure ccache
RUN mkdir -p /root/.ccache \
    && echo "max_size = 2.0G" > /root/.ccache/ccache.conf \
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
LABEL description="Alpine minimal C++ development environment"
LABEL version="1.0"

ENTRYPOINT ["/usr/local/bin/entrypoint"]
CMD ["/bin/bash"]
