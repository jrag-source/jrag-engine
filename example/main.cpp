#include "states/root.hpp"

#include <engine/engine.hpp>

auto main() -> int
{
   jrag::engine::run({
                      .root_state = std::make_unique<states::root>(),
                      .ticks_per_second = 60U,
                      .frames_per_second = 60U,
                     });
   
   return 0;
}