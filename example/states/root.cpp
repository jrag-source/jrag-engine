#include "root.hpp"
#include "main_menu/main_menu.hpp"

#include <app_state/command.hpp>
#include <input/commands.hpp>
#include <input/dispatcher.hpp>

#include <print>
#include <utility>

using namespace states;

root::root()
   : m_intro_state{}
   , m_main_menu_state{}
   , m_game_state{}
   , m_successor_id{successor_id::none}
{}

void root::on_enter()
{
   std::print("{}\n", __PRETTY_FUNCTION__);
}

jrag::app_state::command root::eval_manager_command(logic_tooling)
{
   using state_command = jrag::app_state::command;

   switch (m_successor_id)
   {
      using enum successor_id;

      case none:
         m_successor_id = intro;
         return state_command::push(m_intro_state);
      
      case intro:
      case game:
         m_successor_id = main_menu;
         return state_command::push(m_main_menu_state);

      case main_menu:
         switch (m_main_menu_state.get_selection())
         {
            using enum main_menu::selection;

            case play:
               m_successor_id = game;
               return state_command::push(m_game_state);
               
            case quit:
               m_successor_id = none;
               return state_command::pop();
         }
         
         std::unreachable();
   }

   std::unreachable();
}
