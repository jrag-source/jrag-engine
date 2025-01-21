#include <app_state/state.hpp>

using logic_tooling = jrag::app_state::state::logic_tooling;

logic_tooling::logic_tooling(sf::RenderWindow const & window,
   jrag::input::dispatcher const & input_dispatcher)
   : m_window{window}
   , m_input_dispatcher{input_dispatcher}
{}

auto logic_tooling::get_window() const -> sf::RenderWindow const &
{
   return m_window;
}

auto logic_tooling::get_input_dispatcher() const
   -> jrag::input::dispatcher const &
{
   return m_input_dispatcher;
}
