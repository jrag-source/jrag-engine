#pragma once

#include <input/bind_types.hpp>
#include <input/commands.hpp>

#include <memory>

namespace sf
{
   class Event;
}

namespace jrag::input
{
   class dispatcher final
   {
      // TODO: Add method to check position of mouse within window (will require constructor to take in a sf::RenderWindow const &)
      public:
         explicit dispatcher();
         ~dispatcher();

         dispatcher(dispatcher const &) = delete;
         auto operator=(dispatcher const &) -> dispatcher & = delete;

         auto record_event(sf::Event const & event) -> void;

         auto evaluate_bindings() -> void;

         [[nodiscard]] auto is_command_active(command command) const -> bool;

         [[nodiscard]] auto is_command_activating(command command) const -> bool;

         auto set_bindings(binding_set_collection const & sets) -> void;

         [[nodiscard]] auto get_bindings() const -> binding_set_collection const &;

         [[nodiscard]] auto get_active_bindings() const -> binding_set const &;

         auto next_bindings() -> void;
      
      private:
         struct data;
         std::unique_ptr<data> m_data;
   };
}