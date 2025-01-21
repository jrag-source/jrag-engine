#include <input/print.hpp>

#include "parser.hpp"

#include <util/util.hpp>

#include <iostream>

auto jrag::input::print(const binding_set & set) -> void
{
   std::cout << set.first << '\n';

   using namespace jrag::util;

   const binding_collection & bindings{set.second};
   for (const size_t cmd : as_iterable<command>())
   {
      std::cout << "\t"
                << jrag::input::parser::cmd_to_str(as_enum<command>(cmd))
                << "\n";

      const input_alternatives & alts{bindings.at(cmd)};

      if (alts.empty())
      {
         std::cout << "\n";
         continue;
      }

      for (const input_collection & grouping : alts)
      {
         std::cout << "\t\t";

         static constexpr auto get_input_str =
            [](const jrag::input::variant & input) -> std::string_view
         {
            switch (input.get_type())
            {
               using type = jrag::input::type;

               case type::Keyboard:
                  return jrag::input::parser::key_to_str(
                     input.get<jrag::input::keyboard>().get_key());

               case type::Mouse:
                  return jrag::input::parser::mb_to_str(
                     input.get<jrag::input::mouse>().get_button());

               case type::MouseWheel:
                  return jrag::input::parser::mw_to_str(
                     input.get<jrag::input::mouse_wheel>().get_direction());

               default: return std::string_view{"Invalid"};
            }
         };

         for (const jrag::input::variant & input :
            std::span(grouping.cbegin(), grouping.cend() - 1))
         {
            std::cout << get_input_str(input) << " ";
         }

         std::cout << get_input_str(grouping.back()) << '\n';
      }

      std::cout << '\n';
   }
}

auto jrag::input::print(const binding_set_collection & set_collection) -> void
{
   for (const auto & set : set_collection) { print(set); }
}