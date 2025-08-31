# Boost Network Server - Complete Network Application Example

A comprehensive example of a modern C++ network server using Boost libraries, demonstrating asynchronous networking, HTTP services, and enterprise-grade server development practices.

## 🎯 What This Example Demonstrates

- **Boost.Asio** for asynchronous network programming
- **Boost.Beast** for HTTP/WebSocket server implementation
- **Boost.JSON** for modern JSON handling
- **Boost.Program_options** for command-line configuration
- **Boost.Log** for structured logging
- **Thread pool** and connection management
- **SSL/TLS support** for secure communications
- **RESTful API** design and implementation
- **WebSocket** real-time communication
- **Database integration** with connection pooling
- **Configuration management** with multiple sources

## 🚀 Generated Command

This project was created using:

```bash
cpp-scaffold boost-network-server \
  --type network \
  --framework boost \
  --build cmake \
  --package vcpkg \
  --test gtest \
  --ci github \
  --editor vscode \
  --std cpp20 \
  --docs
```

## 📁 Project Structure

```
boost-network-server/
├── CMakeLists.txt              # Modern CMake with Boost integration
├── vcpkg.json                  # Package dependencies
├── .gitignore                  # Git ignore patterns
├── LICENSE                     # MIT license
├── README.md                   # Project documentation
├── config/
│   ├── server.json             # Server configuration
│   ├── logging.json            # Logging configuration
│   └── ssl/                    # SSL certificates
│       ├── server.crt
│       └── server.key
├── .github/
│   └── workflows/
│       ├── ci.yml              # Multi-platform CI/CD
│       └── security.yml        # Security scanning
├── .vscode/
│   ├── settings.json           # VSCode settings
│   ├── tasks.json              # Build tasks
│   └── launch.json             # Debug configuration
├── src/
│   ├── main.cpp                # Application entry point
│   ├── server/
│   │   ├── http_server.h       # HTTP server implementation
│   │   ├── http_server.cpp
│   │   ├── websocket_server.h  # WebSocket server
│   │   ├── websocket_server.cpp
│   │   ├── connection.h        # Connection management
│   │   ├── connection.cpp
│   │   └── session.h           # Session handling
│   │       └── session.cpp
│   ├── handlers/
│   │   ├── api_handler.h       # REST API handlers
│   │   ├── api_handler.cpp
│   │   ├── file_handler.h      # Static file serving
│   │   ├── file_handler.cpp
│   │   ├── auth_handler.h      # Authentication
│   │   └── auth_handler.cpp
│   ├── middleware/
│   │   ├── cors.h              # CORS middleware
│   │   ├── cors.cpp
│   │   ├── rate_limiter.h      # Rate limiting
│   │   ├── rate_limiter.cpp
│   │   ├── logger.h            # Request logging
│   │   └── logger.cpp
│   ├── database/
│   │   ├── connection_pool.h   # Database connection pool
│   │   ├── connection_pool.cpp
│   │   ├── models/             # Data models
│   │   │   ├── user.h
│   │   │   ├── user.cpp
│   │   │   ├── session.h
│   │   │   └── session.cpp
│   │   └── migrations/         # Database migrations
│   │       └── 001_initial.sql
│   ├── utils/
│   │   ├── config.h            # Configuration management
│   │   ├── config.cpp
│   │   ├── logger.h            # Logging utilities
│   │   ├── logger.cpp
│   │   ├── crypto.h            # Cryptographic utilities
│   │   ├── crypto.cpp
│   │   ├── json_utils.h        # JSON utilities
│   │   └── json_utils.cpp
│   └── security/
│       ├── jwt.h               # JWT token handling
│       ├── jwt.cpp
│       ├── ssl_context.h       # SSL context management
│       └── ssl_context.cpp
├── tests/
│   ├── CMakeLists.txt          # Test configuration
│   ├── test_main.cpp           # Test runner
│   ├── test_server.cpp         # Server tests
│   ├── test_handlers.cpp       # Handler tests
│   ├── test_middleware.cpp     # Middleware tests
│   ├── test_database.cpp       # Database tests
│   └── integration/            # Integration tests
│       ├── test_api.cpp        # API integration tests
│       └── test_websocket.cpp  # WebSocket tests
├── docs/
│   ├── api/                    # API documentation
│   │   ├── openapi.yaml        # OpenAPI specification
│   │   └── endpoints.md        # Endpoint documentation
│   ├── deployment.md           # Deployment guide
│   ├── security.md             # Security considerations
│   └── performance.md          # Performance tuning
├── scripts/
│   ├── setup_db.sh             # Database setup script
│   ├── generate_certs.sh       # SSL certificate generation
│   └── load_test.py            # Load testing script
└── deployment/
    ├── docker/
    │   ├── Dockerfile          # Container definition
    │   └── docker-compose.yml  # Multi-service setup
    ├── kubernetes/             # Kubernetes manifests
    │   ├── deployment.yaml
    │   ├── service.yaml
    │   └── ingress.yaml
    └── systemd/                # Systemd service files
        └── boost-server.service
```

## 🌐 Server Features

### HTTP Server

- **RESTful API**: Full CRUD operations with JSON
- **Static File Serving**: Efficient file delivery with caching
- **Middleware Pipeline**: Modular request processing
- **Content Negotiation**: Multiple response formats
- **Compression**: Gzip/deflate response compression
- **Keep-Alive**: Persistent connection support
- **Virtual Hosts**: Multiple domain support

### WebSocket Server

- **Real-time Communication**: Bidirectional messaging
- **Room Management**: Group messaging capabilities
- **Message Broadcasting**: Efficient multi-client updates
- **Connection Lifecycle**: Proper connect/disconnect handling
- **Ping/Pong**: Connection health monitoring
- **Message Queuing**: Reliable message delivery

### Security Features

- **SSL/TLS Encryption**: Secure communication channels
- **JWT Authentication**: Stateless token-based auth
- **Rate Limiting**: DDoS protection and abuse prevention
- **CORS Support**: Cross-origin resource sharing
- **Input Validation**: Request sanitization and validation
- **Security Headers**: HSTS, CSP, and other security headers

## 🏗️ Building the Project

### Prerequisites

- **Boost 1.82+**: Asio, Beast, JSON, Program_options, Log
- **OpenSSL**: For SSL/TLS support
- **SQLite3**: Database backend (or PostgreSQL/MySQL)
- **C++20** compatible compiler
- **CMake 3.16** or later
- **vcpkg** package manager

### Dependencies (managed by vcpkg)

```json
{
  "name": "boost-network-server",
  "version": "1.0.0",
  "dependencies": [
    "boost-asio",
    "boost-beast",
    "boost-json",
    "boost-program-options",
    "boost-log",
    "boost-filesystem",
    "boost-thread",
    "openssl",
    "sqlite3",
    "nlohmann-json",
    "gtest"
  ]
}
```

### Build Steps

1. **Install dependencies with vcpkg:**
   ```bash
   # Install vcpkg if not already installed
   git clone https://github.com/Microsoft/vcpkg.git
   ./vcpkg/bootstrap-vcpkg.sh
   
   # Install Boost and dependencies
   ./vcpkg/vcpkg install boost-asio boost-beast boost-json boost-program-options boost-log openssl sqlite3 gtest
   ```

2. **Configure and build:**
   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake
   cmake --build . --config Release
   ```

3. **Setup database:**
   ```bash
   # Initialize SQLite database
   sqlite3 server.db < ../src/database/migrations/001_initial.sql
   ```

4. **Generate SSL certificates (for development):**
   ```bash
   ../scripts/generate_certs.sh
   ```

5. **Run the server:**
   ```bash
   ./boost-network-server --config ../config/server.json
   ```

## 🚀 Usage Examples

### Starting the Server

```bash
# Basic startup
./boost-network-server

# With custom configuration
./boost-network-server --config /path/to/config.json

# With command-line overrides
./boost-network-server --port 8080 --threads 4 --ssl-cert cert.pem

# Development mode with verbose logging
./boost-network-server --log-level debug --reload
```

### Configuration File

```json
{
  "server": {
    "host": "0.0.0.0",
    "port": 8443,
    "threads": 8,
    "keep_alive_timeout": 30,
    "max_connections": 1000
  },
  "ssl": {
    "enabled": true,
    "certificate": "config/ssl/server.crt",
    "private_key": "config/ssl/server.key",
    "protocols": ["TLSv1.2", "TLSv1.3"]
  },
  "database": {
    "type": "sqlite",
    "connection_string": "server.db",
    "pool_size": 10,
    "timeout": 30
  },
  "logging": {
    "level": "info",
    "file": "logs/server.log",
    "max_size": "100MB",
    "max_files": 10
  },
  "security": {
    "jwt_secret": "your-secret-key",
    "jwt_expiry": 3600,
    "rate_limit": {
      "requests_per_minute": 60,
      "burst_size": 10
    }
  }
}
```

### API Examples

#### REST API Usage

```bash
# Get all users
curl -X GET https://localhost:8443/api/v1/users \
  -H "Authorization: Bearer <jwt-token>"

# Create new user
curl -X POST https://localhost:8443/api/v1/users \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer <jwt-token>" \
  -d '{"username": "john", "email": "john@example.com"}'

# Update user
curl -X PUT https://localhost:8443/api/v1/users/123 \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer <jwt-token>" \
  -d '{"email": "newemail@example.com"}'

# Delete user
curl -X DELETE https://localhost:8443/api/v1/users/123 \
  -H "Authorization: Bearer <jwt-token>"
```

#### WebSocket Usage

```javascript
// JavaScript WebSocket client
const ws = new WebSocket('wss://localhost:8443/ws');

ws.onopen = function() {
    console.log('Connected to WebSocket');
    
    // Join a room
    ws.send(JSON.stringify({
        type: 'join_room',
        room: 'general'
    }));
};

ws.onmessage = function(event) {
    const message = JSON.parse(event.data);
    console.log('Received:', message);
};

// Send a message
ws.send(JSON.stringify({
    type: 'message',
    room: 'general',
    content: 'Hello, World!'
}));
```

## 🧪 Testing

### Test Categories

- **Unit Tests**: Individual component testing
- **Integration Tests**: API endpoint testing
- **Performance Tests**: Load and stress testing
- **Security Tests**: Authentication and authorization
- **WebSocket Tests**: Real-time communication testing

### Running Tests

```bash
# Run all tests
ctest

# Run specific test suite
ctest -R ServerTests

# Run with verbose output
./test_boost_network_server --gtest_verbose

# Integration tests (requires running server)
./test_integration
```

### Load Testing

```bash
# HTTP load test
python3 scripts/load_test.py --url https://localhost:8443/api/v1/health --concurrent 100 --requests 1000

# WebSocket load test
python3 scripts/ws_load_test.py --url wss://localhost:8443/ws --connections 500 --duration 60
```

## 🔒 Security Features

### Authentication & Authorization

```cpp
// JWT token validation middleware
class JWTMiddleware {
public:
    bool validate_token(const std::string& token) {
        try {
            auto decoded = jwt::decode(token);
            auto verifier = jwt::verify()
                .allow_algorithm(jwt::algorithm::hs256{secret_})
                .with_issuer("boost-server");
            
            verifier.verify(decoded);
            return true;
        } catch (const std::exception& e) {
            BOOST_LOG_TRIVIAL(warning) << "JWT validation failed: " << e.what();
            return false;
        }
    }
    
private:
    std::string secret_;
};
```

### Rate Limiting

```cpp
// Token bucket rate limiter
class RateLimiter {
public:
    bool allow_request(const std::string& client_ip) {
        auto now = std::chrono::steady_clock::now();
        auto& bucket = buckets_[client_ip];
        
        // Refill tokens
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            now - bucket.last_refill).count();
        bucket.tokens = std::min(bucket.capacity, 
            bucket.tokens + elapsed * refill_rate_);
        bucket.last_refill = now;
        
        // Check if request is allowed
        if (bucket.tokens >= 1) {
            bucket.tokens--;
            return true;
        }
        
        return false;
    }
    
private:
    struct TokenBucket {
        int tokens = 0;
        int capacity = 60;
        std::chrono::steady_clock::time_point last_refill;
    };
    
    std::unordered_map<std::string, TokenBucket> buckets_;
    int refill_rate_ = 1; // tokens per second
};
```

## 📊 Performance Optimization

### Connection Pooling

```cpp
// Database connection pool
class ConnectionPool {
public:
    ConnectionPool(const std::string& connection_string, size_t pool_size)
        : connection_string_(connection_string), pool_size_(pool_size) {
        for (size_t i = 0; i < pool_size_; ++i) {
            auto conn = std::make_unique<sqlite3*>();
            if (sqlite3_open(connection_string_.c_str(), conn.get()) == SQLITE_OK) {
                available_connections_.push(std::move(conn));
            }
        }
    }
    
    std::unique_ptr<sqlite3*> acquire() {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [this] { return !available_connections_.empty(); });
        
        auto conn = std::move(available_connections_.front());
        available_connections_.pop();
        return conn;
    }
    
    void release(std::unique_ptr<sqlite3*> conn) {
        std::lock_guard<std::mutex> lock(mutex_);
        available_connections_.push(std::move(conn));
        condition_.notify_one();
    }
    
private:
    std::string connection_string_;
    size_t pool_size_;
    std::queue<std::unique_ptr<sqlite3*>> available_connections_;
    std::mutex mutex_;
    std::condition_variable condition_;
};
```

## 📦 Deployment

### Docker Deployment

```dockerfile
# Multi-stage Docker build
FROM ubuntu:22.04 AS builder

RUN apt-get update && apt-get install -y \
    build-essential cmake git \
    libboost-all-dev libssl-dev libsqlite3-dev

COPY . /src
WORKDIR /src/build
RUN cmake .. && make -j$(nproc)

FROM ubuntu:22.04 AS runtime

RUN apt-get update && apt-get install -y \
    libboost-system1.74.0 libboost-filesystem1.74.0 \
    libssl3 libsqlite3-0 && \
    rm -rf /var/lib/apt/lists/*

COPY --from=builder /src/build/boost-network-server /usr/local/bin/
COPY --from=builder /src/config /etc/boost-server/

EXPOSE 8443
CMD ["boost-network-server", "--config", "/etc/boost-server/server.json"]
```

### Kubernetes Deployment

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: boost-network-server
spec:
  replicas: 3
  selector:
    matchLabels:
      app: boost-network-server
  template:
    metadata:
      labels:
        app: boost-network-server
    spec:
      containers:
      - name: server
        image: boost-network-server:latest
        ports:
        - containerPort: 8443
        env:
        - name: DATABASE_URL
          valueFrom:
            secretKeyRef:
              name: db-secret
              key: url
        resources:
          requests:
            memory: "256Mi"
            cpu: "250m"
          limits:
            memory: "512Mi"
            cpu: "500m"
```

## 🎓 Learning Outcomes

By studying this example, you'll learn:

1. **Asynchronous Programming**: Boost.Asio event-driven architecture
2. **HTTP Protocol**: Modern HTTP server implementation
3. **WebSocket Protocol**: Real-time bidirectional communication
4. **Security Practices**: Authentication, encryption, and protection
5. **Database Integration**: Connection pooling and ORM patterns
6. **Performance Optimization**: Threading, caching, and resource management
7. **Deployment Strategies**: Containerization and orchestration
8. **Enterprise Architecture**: Scalable server design patterns

## 🔗 Related Examples

- [Console File Processor](../console-file-processor/) - Command-line application
- [Qt Image Viewer](../qt-image-viewer/) - GUI application
- [SFML Game Demo](../sfml-game-demo/) - Game development

## 📚 Additional Resources

- [Boost.Asio Documentation](https://www.boost.org/doc/libs/release/doc/html/boost_asio.html)
- [Boost.Beast Documentation](https://www.boost.org/doc/libs/release/libs/beast/doc/html/index.html)
- [HTTP/1.1 Specification](https://tools.ietf.org/html/rfc7230)
- [WebSocket Protocol](https://tools.ietf.org/html/rfc6455)
