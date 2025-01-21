#include <input/loader.hpp>

#include "parser.hpp"

#include <input/bind_types.hpp>
#include <input/commands.hpp>

#include <path/paths.hpp>
#include <util/util.hpp>

#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>

using namespace jrag::input;

using namespace std::literals;
namespace
{
   constexpr std::string_view CONFIG_FILE_NAME{"bindings.cfg"sv};

   [[nodiscard]] auto parse_input(
      std::string_view const input_token) -> std::optional<variant>
   {
      if (auto const key{parser::str_to_key(input_token)})
      {
         return std::optional<variant>{std::in_place, keyboard{*key}};
      }

      if (auto const button{parser::str_to_mb(input_token)})
      {
         return std::optional<variant>{std::in_place, mouse{*button}};
      }

      if (auto const scroll_dir{parser::str_to_mw(input_token)})
      {
         return std::optional<variant>{std::in_place, mouse_wheel{*scroll_dir}};
      }

      return std::nullopt;
   };

   [[nodiscard]] auto parse_set_bindings(
      std::istream & istream) -> binding_collection
   {
      binding_collection bindings;

      std::string bind_line;
      std::getline(istream, bind_line);

      while (!bind_line.empty())
      {
         std::istringstream bind_stream{bind_line};

         std::string cmd_token;
         if (!(bind_stream >> cmd_token))
         {
            std::getline(istream, bind_line);
            continue;
         }

         std::optional<command> cmd{parser::str_to_cmd(cmd_token)};
         if (!cmd)
         {
            std::getline(istream, bind_line);
            continue;
         }

         input_collection input_grouping;

         std::string input_token;
         while (bind_stream >> input_token)
         {
            std::optional<variant> input{parse_input(input_token)};
            if (input) { input_grouping.emplace_back(*input); }
         }

         bindings.at(jrag::util::as_index(*cmd))
            .push_back(std::move(input_grouping));

         std::getline(istream, bind_line);
      }

      return bindings;
   }

   [[nodiscard]] auto parse_set(
      std::istream & istream) -> std::optional<binding_set>
   {
      std::string name_line;
      std::getline(istream, name_line);

      std::istringstream set_name_stream{name_line};
      std::string set_name;
      set_name_stream >> set_name;

      if (!set_name.empty())
      {
         return std::optional<binding_set>{
            std::in_place, set_name, parse_set_bindings(istream)};
      }

      return std::nullopt;
   };
}

auto jrag::input::loader::load() -> binding_set_collection
{
   binding_set_collection loaded_collection;

   static std::filesystem::path const config_path{
      jrag::path::assets_dir() / CONFIG_FILE_NAME};
   std::ifstream config_stream{config_path};

   while (config_stream)
   {
      std::optional<binding_set> set{parse_set(config_stream)};
      if (set) { loaded_collection.emplace_back(std::move(*set)); }
   }

   return loaded_collection;
}