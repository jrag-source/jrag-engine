#pragma once

#include <memory>

namespace jrag::app_state
{
   class state;
}

namespace jrag::engine
{
   struct parameters
   {
         std::unique_ptr<jrag::app_state::state> root_state;

         unsigned int ticks_per_second;
         unsigned int frames_per_second;
   };

   void run(parameters params);
}