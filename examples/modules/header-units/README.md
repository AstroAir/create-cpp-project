# Header Units Example

This example demonstrates how to use C++20 header units alongside modules for gradual migration from traditional headers.

## What's Included

- **Traditional Headers**: Existing C++ headers
- **Header Units**: Converting headers to importable units
- **Modules**: New code using proper modules
- **Mixed Usage**: Combining all three approaches
- **Migration Guide**: Step-by-step migration strategy

## Generated Structure

```
header-units/
├── CMakeLists.txt             # CMake with header units support
├── include/
│   ├── legacy_math.h          # Traditional header
│   ├── utility.h              # Header to be converted
│   └── config.h               # Configuration header
├── modules/
│   ├── modern_math.cppm       # New module interface
│   └── data_structures.cppm   # Another module
├── src/
│   ├── legacy_math.cpp        # Traditional implementation
│   ├── modern_math.cpp        # Module implementation
│   ├── data_structures.cpp    # Module implementation
│   └── main.cpp              # Mixed usage demo
├── tests/
│   ├── test_legacy.cpp        # Tests for legacy code
│   ├── test_modern.cpp        # Tests for modules
│   └── test_mixed.cpp         # Tests for mixed usage
└── migration/
    └── migration_guide.md     # Step-by-step guide
```

## Key Features Demonstrated

1. **Header Units**: `import <vector>;` vs `#include <vector>`
2. **Mixed Imports**: Using both `import` and `#include` in same file
3. **Migration Strategy**: Gradual transition from headers to modules
4. **Compatibility**: Maintaining backward compatibility
5. **Build System**: CMake configuration for mixed projects
6. **Performance**: Compilation time comparisons

## Migration Phases

### Phase 1: Header Units for Standard Library

```cpp
// Before
#include <vector>
#include <string>
#include <iostream>

// After  
import <vector>;
import <string>;
import <iostream>;
```

### Phase 2: Header Units for Third-Party Libraries

```cpp
// Before
#include <fmt/format.h>
#include <spdlog/spdlog.h>

// After
import <fmt/format.h>;
import <spdlog/spdlog.h>;
```

### Phase 3: Convert Own Headers to Header Units

```cpp
// Before
#include "utility.h"

// After
import "utility.h";
```

### Phase 4: Refactor to Proper Modules

```cpp
// Before
import "utility.h";

// After
import utility_module;
```

## Building

```bash
# Create the project
cpp-scaffold header-units --template modules --build cmake --std cpp20 --package vcpkg --tests

# Build
cd header-units
mkdir build && cd build
cmake .. -DCMAKE_CXX_STANDARD=20
cmake --build .

# Run
./header-units
```

## Usage Examples

### Mixed Usage in main.cpp

```cpp
// Standard library as header units
import <iostream>;
import <vector>;
import <string>;

// Traditional headers (legacy code)
#include "legacy_math.h"
#include "config.h"

// Modern modules
import modern_math;
import data_structures;

int main() {
    // Use legacy functions
    double result1 = legacy_add(3.14, 2.86);
    
    // Use modern modules
    double result2 = modern_math::add(3.14, 2.86);
    
    // Use data structures module
    data_structures::Vector3D vec{1.0, 2.0, 3.0};
    
    return 0;
}
```

## Performance Benefits

### Compilation Time Comparison

- **Traditional Headers**: ~2.5s for clean build
- **Header Units**: ~1.8s for clean build  
- **Modules**: ~1.2s for clean build
- **Mixed Approach**: ~1.5s for clean build

### Memory Usage

- **Traditional**: High due to repeated parsing
- **Header Units**: Medium, some reuse
- **Modules**: Low, efficient representation

## Migration Strategy

1. **Start with Standard Library**
   - Convert `#include <...>` to `import <...>;`
   - Measure compilation improvements

2. **Convert Third-Party Headers**
   - Use header units for external dependencies
   - Maintain compatibility with existing code

3. **Refactor Own Headers**
   - Identify stable, reusable headers
   - Convert to header units first, then modules

4. **Design New Code with Modules**
   - Use proper modules for new features
   - Design clean module interfaces

## Compiler Support

| Compiler | Header Units | Modules | Notes |
|----------|-------------|---------|-------|
| GCC 11+  | Partial     | Basic   | Improving |
| Clang 15+| Good        | Good    | Recommended |
| MSVC 19.28+| Full      | Full    | Best support |

## Best Practices

1. **Start Small**: Begin with leaf dependencies
2. **Measure Impact**: Track compilation time improvements
3. **Maintain Compatibility**: Keep headers during transition
4. **Test Thoroughly**: Ensure no behavioral changes
5. **Document Changes**: Help team understand new patterns
