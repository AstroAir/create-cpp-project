# CI-optimized builder image for fast automated builds
# This container is optimized for CI/CD pipelines with minimal size and maximum speed

FROM ubuntu:24.04

# Set environment variables for CI
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=UTC
ENV CI=true
ENV CCACHE_DIR=/root/.ccache
ENV CCACHE_MAX_SIZE=1G
ENV CCACHE_COMPRESS=true

# Install only essential dependencies for CI builds
RUN apt-get update && apt-get install -y \
    # Essential build tools
    build-essential \
    cmake \
    ninja-build \
    pkg-config \
    # Minimal compiler set for CI
    gcc-12 \
    g++-12 \
    # Essential development tools
    git \
    curl \
    wget \
    unzip \
    # Build acceleration
    ccache \
    # Testing tools
    python3 \
    python3-pip \
    # Cleanup
    && rm -rf /var/lib/apt/lists/* \
    && apt-get clean

# Install minimal Python packages for CI
RUN pip3 install --no-cache-dir \
    conan \
    gcovr \
    && rm -rf ~/.cache/pip

# Set up ccache for optimal CI performance
RUN ccache --set-config=compression=true \
    && ccache --set-config=max_size=1G \
    && ccache --set-config=cache_dir=/root/.ccache

# Create workspace directory
WORKDIR /workspace

# Copy project files
COPY . /workspace/

# Build arguments for CI customization
ARG BUILD_TYPE=Release
ARG ENABLE_TESTING=ON
ARG ENABLE_COVERAGE=OFF

# Configure and build the project with CI optimizations
RUN cmake -B build \
    -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
    -G Ninja \
    -DBUILD_TESTING=${ENABLE_TESTING} \
    -DENABLE_COVERAGE=${ENABLE_COVERAGE} \
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
    -DCMAKE_C_COMPILER_LAUNCHER=ccache \
    # CI-specific optimizations
    -DCMAKE_VERBOSE_MAKEFILE=OFF \
    -DCMAKE_COLOR_MAKEFILE=OFF \
    && cmake --build build --parallel $(nproc)

# Install the built application
RUN cmake --install build --prefix /usr/local

# Create a non-root user for security
RUN useradd -m -s /bin/bash ciuser \
    && chown -R ciuser:ciuser /workspace

# Switch to non-root user
USER ciuser

# Set up entrypoint
ENTRYPOINT ["cpp-scaffold"]
