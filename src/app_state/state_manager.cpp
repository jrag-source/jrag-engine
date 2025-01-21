#include <app_state/state_manager.hpp>

#include <app_state/command.hpp>
#include <app_state/state.hpp>

#include <SFML/Graphics/RenderWindow.hpp>

#include <ranges>

using namespace jrag::app_state;

state_manager::state_manager(state & root_state)
   : m_state_stack{&root_state}
{
   root_state.on_enter();
}

state_manager::~state_manager() = default;

auto state_manager::logic_process(sf::RenderWindow const & window,
   jrag::input::dispatcher const & input_dispatcher,
   std::chrono::duration<float> const delta_time) -> process_status
{
   if (m_state_stack.empty()) { return process_status::inactive; }

   for (auto const & state : m_state_stack | std::views::reverse)
   {
      state->logic_process(
         state::logic_tooling{window, input_dispatcher}, delta_time);
   }

   auto const state_command{m_state_stack.back()->eval_manager_command(
      state::logic_tooling{window, input_dispatcher})};

   switch (state_command.m_command_type)
   {
      using enum command::type;

      case None: break;

      case Push:
         m_state_stack.push_back(state_command.m_pushed_state);
         state_command.m_pushed_state->on_enter();
         break;

      case Pop: m_state_stack.pop_back(); break;
   }

   return process_status::active;
}

void state_manager::graphics_process(
   sf::RenderWindow & window, float const interpolation)
{
   constexpr auto find_draw_start = [](std::vector<state *> const & state_stack)
   {
      for (auto const & [index, state] :
         state_stack | std::views::enumerate | std::views::reverse)
      {
         if (state->is_opaque()) { return index; }
      }

      return 0L;
   };

   auto const draw_start{find_draw_start(m_state_stack)};

   for (auto const & state :
      std::span{m_state_stack.cbegin() + draw_start, m_state_stack.cend()})
   {
      window.setView(window.getDefaultView());
      state->graphics_process(state::graphics_tooling{window}, interpolation);
   }
}
