#include "entity.h"
#include "component.h"

namespace test-gameengine-premake {

Entity::Entity() {
}

Entity::~Entity() {
    components_.clear();
}

// Template method implementations would typically go in the header file
// for template instantiation, but here's the basic structure

} // namespace test-gameengine-premake
