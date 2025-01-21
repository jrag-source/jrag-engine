#include <input/dispatcher.hpp>

#include <input/commands.hpp>
#include <input/keyboard.hpp>
#include <input/mouse.hpp>
#include <input/mouse_wheel.hpp>
#include <input/types.hpp>
#include <input/variant.hpp>

#include <util/util.hpp>

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <ranges>

using namespace jrag::input;

namespace
{
   enum class command_state : std::uint8_t
   {
      Inactive,
      Active,
   };

   using command_state_collection =
      std::array<command_state, static_cast<std::size_t>(command::Count)>;

   [[nodiscard]] auto default_bindings() -> binding_set_collection
   {
      binding_collection bindings;

      using jinput = jrag::input::variant;
      using jkey_input = jrag::input::keyboard;
      using jmouse_input = jrag::input::mouse;
      using jwheel_input = jrag::input::mouse_wheel;

      using sf_key = sf::Keyboard::Key;
      using sf_button = sf::Mouse::Button;

      input_collection wasd_right{jinput{jkey_input{sf_key::D}}};
      input_collection keys_right{jinput{jkey_input{sf_key::Right}}};

      input_collection wasd_up{jinput{jkey_input{sf_key::W}}};
      input_collection keys_up{jinput{jkey_input{sf_key::Up}}};

      input_collection wasd_left{jinput{jkey_input{sf_key::A}}};
      input_collection keys_left{jinput{jkey_input{sf_key::Left}}};

      input_collection wasd_down{jinput{jkey_input{sf_key::S}}};
      input_collection keys_down{jinput{jkey_input{sf_key::Down}}};

      input_collection shoot{jinput{jmouse_input{sf_button::Left}}};

      input_collection jump{jinput{jkey_input{sf_key::Space}}};

      input_collection scroll_up{jinput{jkey_input{sf_key::LShift}},
         jinput{jwheel_input{jwheel_input::scroll_direction::Up}}};

      input_collection scroll_down{jinput{jkey_input{sf_key::LShift}},
         jinput{jwheel_input{jwheel_input::scroll_direction::Down}}};

      using namespace jrag::util;
      bindings[as_index(command::MoveRight)] = {wasd_right, keys_right};
      bindings[as_index(command::MoveUp)] = {wasd_up, keys_up};
      bindings[as_index(command::MoveLeft)] = {wasd_left, keys_left};
      bindings[as_index(command::MoveDown)] = {wasd_down, keys_down};
      bindings[as_index(command::Shoot)] = {shoot};
      bindings[as_index(command::Jump)] = {jump};
      bindings[as_index(command::ScrollUp)] = {scroll_up};
      bindings[as_index(command::ScrollDown)] = {scroll_down};

      return binding_set_collection{{"default", bindings}};
   }

   [[nodiscard]] auto has_active_input_group(
      input_collection const & event_inputs,
      input_alternatives const & input_alternatives) -> bool
   {
      auto found_in_event_inputs =
         [&event_inputs](const jrag::input::variant & input) -> bool
      {
         return std::ranges::any_of(event_inputs,
            [&input](const jrag::input::variant & event_input) -> bool
            { return input == event_input; });
      };

      auto is_active_input = [&found_in_event_inputs](
                                const jrag::input::variant & input)
      {
         bool is_active{false};

         switch (input.get_type())
         {
            using enum jrag::input::type;

            case Keyboard:
               is_active = sf::Keyboard::isKeyPressed(
                  input.get<jrag::input::keyboard>().get_key());
               break;

            case Mouse:
               is_active = sf::Mouse::isButtonPressed(
                  input.get<jrag::input::mouse>().get_button());
               break;

            case MouseWheel: is_active = found_in_event_inputs(input); break;

            default: break;
         }

         return is_active;
      };

      auto is_active_input_grouping =
         [&is_active_input](input_collection const & input_grouping) -> bool
      { return std::ranges::all_of(input_grouping, is_active_input); };

      return std::ranges::any_of(input_alternatives, is_active_input_grouping);
   }

   [[nodiscard]] auto initial_cmd_states() -> command_state_collection
   {
      command_state_collection states;
      states.fill(command_state::Inactive);
      return states;
   }
}

struct dispatcher::data
{
      binding_set_collection m_binding_sets;
      command_state_collection m_cmd_states;
      command_state_collection m_prev_cmd_states;

      input_collection m_event_inputs;
      bool m_has_focus;
      size_t m_active_binding_idx;
};

dispatcher::dispatcher()
   : m_data{std::make_unique<data>(default_bindings(), initial_cmd_states(),
        initial_cmd_states(), input_collection{}, false, 0)}
{}

dispatcher::~dispatcher() = default;

void dispatcher::record_event(const sf::Event & event)
{
   switch (event.type)
   {
      using enum sf::Event::EventType;

      case MouseWheelScrolled:
         using direction = jrag::input::mouse_wheel::scroll_direction;
         // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access): SFML's Event
         // API relies on direct union access
         m_data->m_event_inputs.emplace_back(jrag::input::mouse_wheel{
            event.mouseWheelScroll.delta > 0 ? direction::Up
                                             : direction::Down});
         // NOLINTEND(cppcoreguidelines-pro-type-union-access)
         break;

      case GainedFocus: m_data->m_has_focus = true; break;

      case LostFocus: m_data->m_has_focus = false; break;

      default: break;
   }
}

void dispatcher::evaluate_bindings()
{
   m_data->m_prev_cmd_states = m_data->m_cmd_states;

   if (m_data->m_has_focus)
   {
      binding_collection const & active_bindings{
         m_data->m_binding_sets[m_data->m_active_binding_idx].second};

      for (auto [input_alternatives, cmd_state] :
         std::views::zip(active_bindings, m_data->m_cmd_states))
      {
         using enum command_state;
         cmd_state =
            has_active_input_group(m_data->m_event_inputs, input_alternatives)
            ? Active
            : Inactive;
      }
   }
   else { m_data->m_cmd_states.fill(command_state::Inactive); }

   m_data->m_event_inputs.clear();
}

auto dispatcher::is_command_active(command const command) const -> bool
{
   return m_data->m_cmd_states.at(jrag::util::as_index(command)) ==
      command_state::Active;
}

auto dispatcher::is_command_activating(command const command) const -> bool
{
   using jrag::util::as_index;
   using enum command_state;

   return m_data->m_cmd_states.at(as_index(command)) == Active &&
      m_data->m_prev_cmd_states.at(as_index(command)) == Inactive;
}

// TODO: Use move semantics
auto dispatcher::set_bindings(binding_set_collection const & sets) -> void
{
   // TODO: Keep default bindings separate to avoid copying them everytime we
   // reset
   m_data->m_binding_sets = default_bindings();
   m_data->m_binding_sets.insert(
      m_data->m_binding_sets.cend(), sets.cbegin(), sets.cend());
}

auto dispatcher::get_bindings() const -> binding_set_collection const &
{
   return m_data->m_binding_sets;
}

auto dispatcher::get_active_bindings() const -> binding_set const &
{
   return m_data->m_binding_sets[m_data->m_active_binding_idx];
}

auto dispatcher::next_bindings() -> void
{
   const bool wrap{
      m_data->m_active_binding_idx + 1 == m_data->m_binding_sets.size()};
   m_data->m_active_binding_idx = wrap ? 0 : m_data->m_active_binding_idx + 1;

   std::cout << "New bindings active: "
             << m_data->m_binding_sets[m_data->m_active_binding_idx].first
             << '\n';
}
