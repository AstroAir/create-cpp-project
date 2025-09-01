#include "entity.h"
#include "component.h"

namespace quick-test-premake-game {

Entity::Entity() {
}

Entity::~Entity() {
    components_.clear();
}

// Template method implementations would typically go in the header file
// for template instantiation, but here's the basic structure

} // namespace quick-test-premake-game
