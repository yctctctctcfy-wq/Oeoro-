
FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    libspdlog-dev \
    valgrind \
    clang-format \
    cppcheck \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source code
COPY . .

# Build the application
RUN make release

# Create plugins directory
RUN mkdir -p /usr/local/lib/oroto/plugins

# Expose any ports if needed (placeholder)
EXPOSE 8080

# Set entry point
CMD ["./oroto-kernel"]
