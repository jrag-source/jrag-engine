#include "app_state/command.hpp"

using namespace jrag::app_state;

auto command::none() -> command { return command{type::None, nullptr}; }

auto command::push(state & push_state) -> command
{
   return command{type::Push, &push_state};
}

auto command::pop() -> command { return command{type::Pop, nullptr}; }

command::command(type const command_type, state * const pushed_state)
   : m_command_type{command_type}
   , m_pushed_state{pushed_state}
{}