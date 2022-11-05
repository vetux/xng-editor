/**
 * Register user system and component classes using static initialization.
 *
 * Only systems with a default constructor can be registered.
 *
 * Registered systems can access the application through the static Application::getCurrentApplication interface.
*/

#include "xng/xng.hpp"

#include "systems/examplesystem.hpp"

using namespace xng;

REGISTER_COMPONENT(ExampleComponent)

REGISTER_SYSTEM(ExampleSystem)