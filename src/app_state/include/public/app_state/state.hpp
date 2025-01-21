#pragma once

#include <chrono>

namespace jrag::input
{
   class dispatcher;
}

namespace sf
{
   class RenderWindow;
}

namespace jrag::app_state
{
   class command;
   class state
   {
      public:
         virtual ~state() = default;

         [[nodiscard]] constexpr virtual auto is_opaque() const -> bool = 0;

         class logic_tooling;
         class graphics_tooling;

         // TODO: Make logic_tooling and graphics_tooling actual classes with
         // fully encapsulated members. Right now the window in graphics_tooling
         // could be modified in multiple ways that are not accounted for.

      private:
         virtual auto logic_process(logic_tooling tooling,
            std::chrono::duration<float> delta_time) -> void = 0;

         // graphics_process always starts with the default window view.
         // If drawing on a different view is desired, each state can safely set
         // the window view in this function. It will not affect subsequent
         // graphics_process calls.
         virtual auto graphics_process(
            graphics_tooling tooling, float interpolation) -> void = 0;

         virtual auto on_enter() -> void = 0;

         // TODO: get_management_command() const = 0 ???
         // command manage_engine(Engine & engine) const = 0 ???
         // At some point we should allow the user to modify the engine, and the
         // top state seems like a good place.
         [[nodiscard]] virtual auto eval_manager_command(
            logic_tooling tooling) -> command = 0;

         friend class state_manager;
   };

   class state::logic_tooling
   {
      public:
         logic_tooling(sf::RenderWindow const & window,
            jrag::input::dispatcher const & input_dispatcher);
         ~logic_tooling() = default;

         logic_tooling(logic_tooling const &) = delete;
         auto operator=(logic_tooling const &) = delete;

         [[nodiscard]] auto get_window() const -> sf::RenderWindow const &;
         [[nodiscard]] auto
         get_input_dispatcher() const -> jrag::input::dispatcher const &;

      private:
         sf::RenderWindow const & m_window;
         jrag::input::dispatcher const & m_input_dispatcher;
   };

   class state::graphics_tooling
   {
      public:
         explicit graphics_tooling(sf::RenderWindow & window);
         ~graphics_tooling() = default;

         graphics_tooling(graphics_tooling const &) = delete;
         auto operator=(graphics_tooling const &) = delete;

         [[nodiscard]] auto get_window() const -> sf::RenderWindow &;

      private:
         sf::RenderWindow & m_window;
   };
}