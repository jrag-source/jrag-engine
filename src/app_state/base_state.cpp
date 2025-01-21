#include "app_state/base_state.hpp"

#include "app_state/command.hpp"

#include <app_state/state.hpp>

using namespace jrag::app_state;

auto base_state::logic_process(logic_tooling /*tooling*/,
   std::chrono::duration<float> const /*delta_time*/) -> void
{}

auto base_state::graphics_process(
   graphics_tooling /*tooling*/, float const /*interpolation*/) -> void
{}

auto base_state::on_enter() -> void {}

auto base_state::eval_manager_command(logic_tooling /*tooling*/) -> command
{
   return command::none();
}
