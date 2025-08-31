# Testing Patterns - Comprehensive Testing Examples

A comprehensive collection of testing patterns and examples for C++ projects created with CPP-Scaffold, demonstrating unit testing, integration testing, mocking, and test-driven development practices.

## 🎯 What This Example Demonstrates

- **Unit Testing** with Google Test framework
- **Integration Testing** for multi-component systems
- **Mocking and Stubbing** with Google Mock
- **Test-Driven Development (TDD)** workflow
- **Behavior-Driven Development (BDD)** patterns
- **Performance Testing** and benchmarking
- **Property-Based Testing** concepts
- **Test Fixtures** and test data management
- **Continuous Integration** testing strategies
- **Code Coverage** analysis and reporting

## 🚀 Generated Command

This testing example collection was created using:

```bash
cpp-scaffold testing-patterns \
  --type lib \
  --build cmake \
  --package vcpkg \
  --test gtest \
  --ci github \
  --editor vscode \
  --std cpp20 \
  --docs \
  --code-style
```

## 📁 Project Structure

```
testing-patterns/
├── CMakeLists.txt              # Modern CMake with testing
├── vcpkg.json                  # Package dependencies
├── .gitignore                  # Git ignore patterns
├── LICENSE                     # MIT license
├── README.md                   # Project documentation
├── .github/
│   └── workflows/
│       ├── ci.yml              # CI with comprehensive testing
│       └── coverage.yml        # Code coverage reporting
├── .vscode/
│   ├── settings.json           # VSCode testing settings
│   ├── tasks.json              # Test execution tasks
│   └── launch.json             # Debug configuration
├── src/
│   ├── calculator/             # Example library to test
│   │   ├── calculator.h
│   │   ├── calculator.cpp
│   │   ├── advanced_calculator.h
│   │   └── advanced_calculator.cpp
│   ├── database/               # Database abstraction example
│   │   ├── database_interface.h
│   │   ├── sqlite_database.h
│   │   ├── sqlite_database.cpp
│   │   ├── user_repository.h
│   │   └── user_repository.cpp
│   ├── network/                # Network service example
│   │   ├── http_client.h
│   │   ├── http_client.cpp
│   │   ├── api_service.h
│   │   └── api_service.cpp
│   └── utils/
│       ├── string_utils.h
│       ├── string_utils.cpp
│       ├── file_utils.h
│       └── file_utils.cpp
├── tests/
│   ├── CMakeLists.txt          # Test configuration
│   ├── test_main.cpp           # Custom test main
│   ├── unit/                   # Unit tests
│   │   ├── test_calculator.cpp
│   │   ├── test_advanced_calculator.cpp
│   │   ├── test_string_utils.cpp
│   │   └── test_file_utils.cpp
│   ├── integration/            # Integration tests
│   │   ├── test_database_integration.cpp
│   │   ├── test_api_integration.cpp
│   │   └── test_end_to_end.cpp
│   ├── mocks/                  # Mock objects
│   │   ├── mock_database.h
│   │   ├── mock_http_client.h
│   │   └── mock_file_system.h
│   ├── fixtures/               # Test fixtures and data
│   │   ├── test_data.h
│   │   ├── test_data.cpp
│   │   ├── sample_data.json
│   │   └── test_files/
│   │       ├── input.txt
│   │       └── expected_output.txt
│   ├── performance/            # Performance tests
│   │   ├── benchmark_calculator.cpp
│   │   ├── benchmark_string_utils.cpp
│   │   └── load_test_api.cpp
│   ├── property/               # Property-based tests
│   │   ├── property_calculator.cpp
│   │   └── property_string_utils.cpp
│   └── helpers/                # Test utilities
│       ├── test_helpers.h
│       ├── test_helpers.cpp
│       ├── temp_directory.h
│       └── temp_directory.cpp
├── docs/
│   ├── testing-guide.md        # Comprehensive testing guide
│   ├── tdd-workflow.md         # TDD workflow documentation
│   ├── ci-cd-testing.md        # CI/CD testing strategies
│   └── coverage-analysis.md    # Code coverage guide
├── scripts/
│   ├── run_tests.sh            # Test execution script
│   ├── generate_coverage.sh    # Coverage report generation
│   ├── run_benchmarks.sh       # Performance testing
│   └── setup_test_env.sh       # Test environment setup
└── tools/
    ├── test_runner.py          # Custom test runner
    ├── coverage_reporter.py    # Coverage analysis tool
    └── test_data_generator.py  # Test data generation
```

## 🧪 Testing Patterns and Examples

### 1. Unit Testing Patterns

#### Basic Unit Tests

```cpp
// tests/unit/test_calculator.cpp
#include <gtest/gtest.h>
#include "calculator/calculator.h"

class CalculatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        calculator_ = std::make_unique<Calculator>();
    }
    
    void TearDown() override {
        calculator_.reset();
    }
    
    std::unique_ptr<Calculator> calculator_;
};

TEST_F(CalculatorTest, Addition) {
    EXPECT_EQ(calculator_->add(2, 3), 5);
    EXPECT_EQ(calculator_->add(-1, 1), 0);
    EXPECT_EQ(calculator_->add(0, 0), 0);
}

TEST_F(CalculatorTest, Division) {
    EXPECT_EQ(calculator_->divide(10, 2), 5);
    EXPECT_THROW(calculator_->divide(10, 0), std::invalid_argument);
}

// Parameterized tests
class CalculatorParameterizedTest : public ::testing::TestWithParam<std::tuple<int, int, int>> {};

TEST_P(CalculatorParameterizedTest, AdditionParameterized) {
    auto [a, b, expected] = GetParam();
    Calculator calc;
    EXPECT_EQ(calc.add(a, b), expected);
}

INSTANTIATE_TEST_SUITE_P(
    AdditionTests,
    CalculatorParameterizedTest,
    ::testing::Values(
        std::make_tuple(1, 2, 3),
        std::make_tuple(-1, -2, -3),
        std::make_tuple(0, 5, 5),
        std::make_tuple(100, -50, 50)
    )
);
```

#### Advanced Testing with Fixtures

```cpp
// tests/fixtures/test_data.h
class DatabaseTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temporary database
        temp_db_path_ = std::filesystem::temp_directory_path() / "test.db";
        database_ = std::make_unique<SqliteDatabase>(temp_db_path_.string());
        
        // Initialize with test data
        setupTestData();
    }
    
    void TearDown() override {
        database_.reset();
        std::filesystem::remove(temp_db_path_);
    }
    
    void setupTestData() {
        database_->execute("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT, email TEXT)");
        database_->execute("INSERT INTO users (name, email) VALUES ('John', 'john@example.com')");
        database_->execute("INSERT INTO users (name, email) VALUES ('Jane', 'jane@example.com')");
    }
    
    std::filesystem::path temp_db_path_;
    std::unique_ptr<SqliteDatabase> database_;
};

TEST_F(DatabaseTestFixture, UserRetrieval) {
    auto users = database_->query("SELECT * FROM users");
    EXPECT_EQ(users.size(), 2);
    EXPECT_EQ(users[0]["name"], "John");
    EXPECT_EQ(users[1]["name"], "Jane");
}
```

### 2. Mocking and Stubbing

#### Google Mock Examples

```cpp
// tests/mocks/mock_database.h
#include <gmock/gmock.h>
#include "database/database_interface.h"

class MockDatabase : public DatabaseInterface {
public:
    MOCK_METHOD(bool, connect, (const std::string& connection_string), (override));
    MOCK_METHOD(void, disconnect, (), (override));
    MOCK_METHOD(std::vector<User>, getUsers, (), (override));
    MOCK_METHOD(bool, saveUser, (const User& user), (override));
    MOCK_METHOD(bool, deleteUser, (int user_id), (override));
};

// Usage in tests
TEST(UserRepositoryTest, GetAllUsers) {
    MockDatabase mock_db;
    UserRepository repository(&mock_db);
    
    std::vector<User> expected_users = {
        User{1, "John", "john@example.com"},
        User{2, "Jane", "jane@example.com"}
    };
    
    EXPECT_CALL(mock_db, getUsers())
        .WillOnce(::testing::Return(expected_users));
    
    auto users = repository.getAllUsers();
    EXPECT_EQ(users.size(), 2);
    EXPECT_EQ(users[0].name, "John");
}

// Advanced mock expectations
TEST(ApiServiceTest, RetryOnFailure) {
    MockHttpClient mock_client;
    ApiService service(&mock_client);
    
    // Expect first call to fail, second to succeed
    EXPECT_CALL(mock_client, get(::testing::_))
        .WillOnce(::testing::Throw(std::runtime_error("Network error")))
        .WillOnce(::testing::Return(HttpResponse{200, "Success"}));
    
    auto response = service.getDataWithRetry("/api/data");
    EXPECT_EQ(response.status_code, 200);
}
```

### 3. Integration Testing

#### Multi-Component Integration Tests

```cpp
// tests/integration/test_api_integration.cpp
class ApiIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Start test server
        server_ = std::make_unique<TestServer>(8080);
        server_->start();
        
        // Create real HTTP client
        client_ = std::make_unique<HttpClient>("http://localhost:8080");
        
        // Setup test database
        setupTestDatabase();
    }
    
    void TearDown() override {
        server_->stop();
        cleanupTestDatabase();
    }
    
    std::unique_ptr<TestServer> server_;
    std::unique_ptr<HttpClient> client_;
};

TEST_F(ApiIntegrationTest, UserCrudOperations) {
    // Create user
    nlohmann::json user_data = {
        {"name", "Test User"},
        {"email", "test@example.com"}
    };
    
    auto create_response = client_->post("/api/users", user_data.dump());
    EXPECT_EQ(create_response.status_code, 201);
    
    auto created_user = nlohmann::json::parse(create_response.body);
    int user_id = created_user["id"];
    
    // Read user
    auto get_response = client_->get("/api/users/" + std::to_string(user_id));
    EXPECT_EQ(get_response.status_code, 200);
    
    auto retrieved_user = nlohmann::json::parse(get_response.body);
    EXPECT_EQ(retrieved_user["name"], "Test User");
    
    // Update user
    nlohmann::json update_data = {{"email", "updated@example.com"}};
    auto update_response = client_->put("/api/users/" + std::to_string(user_id), update_data.dump());
    EXPECT_EQ(update_response.status_code, 200);
    
    // Delete user
    auto delete_response = client_->delete_("/api/users/" + std::to_string(user_id));
    EXPECT_EQ(delete_response.status_code, 204);
    
    // Verify deletion
    auto verify_response = client_->get("/api/users/" + std::to_string(user_id));
    EXPECT_EQ(verify_response.status_code, 404);
}
```

### 4. Performance Testing

#### Benchmark Tests

```cpp
// tests/performance/benchmark_calculator.cpp
#include <benchmark/benchmark.h>
#include "calculator/advanced_calculator.h"

static void BM_BasicAddition(benchmark::State& state) {
    AdvancedCalculator calc;
    for (auto _ : state) {
        benchmark::DoNotOptimize(calc.add(123.456, 789.012));
    }
}
BENCHMARK(BM_BasicAddition);

static void BM_ComplexCalculation(benchmark::State& state) {
    AdvancedCalculator calc;
    for (auto _ : state) {
        auto result = calc.sqrt(calc.multiply(calc.add(123.456, 789.012), 2.5));
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_ComplexCalculation);

// Parameterized benchmarks
static void BM_StringProcessing(benchmark::State& state) {
    std::string input(state.range(0), 'a');
    for (auto _ : state) {
        auto result = StringUtils::reverse(input);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_StringProcessing)->Range(8, 8<<10);

BENCHMARK_MAIN();
```

### 5. Property-Based Testing

#### Property Tests with Custom Generators

```cpp
// tests/property/property_calculator.cpp
#include <gtest/gtest.h>
#include <random>
#include "calculator/calculator.h"

class PropertyTest : public ::testing::Test {
protected:
    std::random_device rd_;
    std::mt19937 gen_{rd_()};
    std::uniform_int_distribution<> int_dist_{-1000, 1000};
    std::uniform_real_distribution<> real_dist_{-1000.0, 1000.0};
    
    int randomInt() { return int_dist_(gen_); }
    double randomDouble() { return real_dist_(gen_); }
};

TEST_F(PropertyTest, AdditionCommutative) {
    Calculator calc;
    
    // Property: a + b = b + a
    for (int i = 0; i < 100; ++i) {
        int a = randomInt();
        int b = randomInt();
        
        EXPECT_EQ(calc.add(a, b), calc.add(b, a))
            << "Addition should be commutative for a=" << a << ", b=" << b;
    }
}

TEST_F(PropertyTest, AdditionAssociative) {
    Calculator calc;
    
    // Property: (a + b) + c = a + (b + c)
    for (int i = 0; i < 100; ++i) {
        int a = randomInt();
        int b = randomInt();
        int c = randomInt();
        
        auto left = calc.add(calc.add(a, b), c);
        auto right = calc.add(a, calc.add(b, c));
        
        EXPECT_EQ(left, right)
            << "Addition should be associative for a=" << a << ", b=" << b << ", c=" << c;
    }
}

TEST_F(PropertyTest, MultiplicationIdentity) {
    Calculator calc;
    
    // Property: a * 1 = a
    for (int i = 0; i < 100; ++i) {
        double a = randomDouble();
        
        EXPECT_DOUBLE_EQ(calc.multiply(a, 1.0), a)
            << "Multiplication by 1 should be identity for a=" << a;
    }
}
```

### 6. Test-Driven Development (TDD) Example

#### TDD Workflow Documentation

```cpp
// Example TDD workflow for implementing a new feature

// Step 1: Write failing test
TEST(StringUtilsTest, ShouldCapitalizeFirstLetter) {
    // This test will fail initially because capitalize() doesn't exist
    EXPECT_EQ(StringUtils::capitalize("hello"), "Hello");
    EXPECT_EQ(StringUtils::capitalize("WORLD"), "World");
    EXPECT_EQ(StringUtils::capitalize(""), "");
}

// Step 2: Implement minimal code to make test pass
std::string StringUtils::capitalize(const std::string& input) {
    if (input.empty()) return input;
    
    std::string result = input;
    result[0] = std::toupper(result[0]);
    for (size_t i = 1; i < result.length(); ++i) {
        result[i] = std::tolower(result[i]);
    }
    return result;
}

// Step 3: Refactor and add more tests
TEST(StringUtilsTest, CapitalizeEdgeCases) {
    EXPECT_EQ(StringUtils::capitalize("a"), "A");
    EXPECT_EQ(StringUtils::capitalize("123abc"), "123abc");
    EXPECT_EQ(StringUtils::capitalize("hello world"), "Hello world");
}
```

## 🚀 Running Tests

### Basic Test Execution

```bash
# Build and run all tests
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTING=ON
cmake --build .
ctest --output-on-failure

# Run specific test suites
ctest -R "Unit"
ctest -R "Integration"
ctest -R "Performance"

# Run tests with verbose output
ctest --verbose

# Run tests in parallel
ctest -j 4
```

### Advanced Test Execution

```bash
# Run tests with coverage
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON
cmake --build .
ctest
make coverage

# Run specific test executable
./test_calculator --gtest_filter="CalculatorTest.*"
./test_calculator --gtest_repeat=10
./test_calculator --gtest_shuffle

# Run benchmarks
./benchmark_calculator --benchmark_format=json --benchmark_out=results.json

# Run integration tests with test server
./scripts/setup_test_env.sh
./test_integration
```

## 📊 Code Coverage Analysis

### Coverage Configuration

```cmake
# CMakeLists.txt coverage setup
if(ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
        
        find_program(LCOV_PATH lcov)
        find_program(GENHTML_PATH genhtml)
        
        if(LCOV_PATH AND GENHTML_PATH)
            add_custom_target(coverage
                COMMAND ${LCOV_PATH} --capture --directory . --output-file coverage.info
                COMMAND ${LCOV_PATH} --remove coverage.info '/usr/*' --output-file coverage.info
                COMMAND ${LCOV_PATH} --list coverage.info
                COMMAND ${GENHTML_PATH} coverage.info --output-directory coverage_html
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Generating code coverage report"
            )
        endif()
    endif()
endif()
```

### Coverage Reporting

```bash
# Generate coverage report
./scripts/generate_coverage.sh

# View coverage in browser
open coverage_html/index.html

# Coverage summary
lcov --summary coverage.info
```

## 🔄 Continuous Integration Testing

### GitHub Actions Workflow

```yaml
# .github/workflows/ci.yml
name: Comprehensive Testing

on: [push, pull_request]

jobs:
  test:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]
        build_type: [Debug, Release]
        
    steps:
    - uses: actions/checkout@v3
    
    - name: Install dependencies
      run: |
        # Install vcpkg and dependencies
        git clone https://github.com/Microsoft/vcpkg.git
        ./vcpkg/bootstrap-vcpkg.sh
        ./vcpkg/vcpkg install gtest gmock benchmark
    
    - name: Configure CMake
      run: |
        cmake -B build -DCMAKE_BUILD_TYPE=${{ matrix.build_type }} \
              -DENABLE_TESTING=ON -DENABLE_COVERAGE=ON \
              -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake
    
    - name: Build
      run: cmake --build build --config ${{ matrix.build_type }}
    
    - name: Run Unit Tests
      run: |
        cd build
        ctest -C ${{ matrix.build_type }} --output-on-failure -R "Unit"
    
    - name: Run Integration Tests
      run: |
        cd build
        ctest -C ${{ matrix.build_type }} --output-on-failure -R "Integration"
    
    - name: Run Performance Tests
      if: matrix.build_type == 'Release'
      run: |
        cd build
        ./benchmark_calculator --benchmark_format=json --benchmark_out=benchmark_results.json
    
    - name: Generate Coverage Report
      if: matrix.os == 'ubuntu-latest' && matrix.build_type == 'Debug'
      run: |
        cd build
        make coverage
    
    - name: Upload Coverage to Codecov
      if: matrix.os == 'ubuntu-latest' && matrix.build_type == 'Debug'
      uses: codecov/codecov-action@v3
      with:
        file: build/coverage.info
```

## 🎓 Learning Outcomes

By studying these testing examples, you'll learn:

1. **Unit Testing**: Comprehensive component testing strategies
2. **Integration Testing**: Multi-component system validation
3. **Mocking**: Isolation testing with mock objects
4. **TDD/BDD**: Test-driven and behavior-driven development
5. **Performance Testing**: Benchmarking and load testing
6. **Property Testing**: Mathematical property validation
7. **CI/CD Testing**: Automated testing in pipelines
8. **Coverage Analysis**: Code coverage measurement and reporting

## 🔗 Related Examples

- [Console File Processor](../console-file-processor/) - Real application with tests
- [Qt Image Viewer](../qt-image-viewer/) - GUI application testing
- [Boost Network Server](../boost-network-server/) - Network service testing

## 📚 Additional Resources

- [Google Test Documentation](https://google.github.io/googletest/)
- [Google Mock Documentation](https://google.github.io/googletest/gmock_for_dummies.html)
- [Test-Driven Development](https://martinfowler.com/bliki/TestDrivenDevelopment.html)
- [C++ Testing Best Practices](https://github.com/cpp-best-practices/cppbestpractices)
