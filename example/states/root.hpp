#pragma once

#include "game/game.hpp"
#include "intro/intro.hpp"
#include "main_menu/main_menu.hpp"

#include <app_state/base_state.hpp>

namespace sf
{
   class RenderWindow;
}

namespace states
{
   class root final : public jrag::app_state::base_state
   {
      public:
         root();
      
         void on_enter() override;

         jrag::app_state::command eval_manager_command(logic_tooling tooling) override;
      
      private:
         intro m_intro_state;
         main_menu m_main_menu_state;
         game m_game_state;

         enum class successor_id
         {
            none,
            intro,
            main_menu,
            game,
         };

         successor_id m_successor_id;
   };
}