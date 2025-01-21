#include "parser.hpp"

#include <cassert>
#include <optional>
#include <unordered_map>

namespace
{
   using KeyToStringMap =
      std::unordered_map<sf::Keyboard::Key, std::string_view>;
   using StringToKeyMap =
      std::unordered_map<std::string_view, sf::Keyboard::Key>;

   class KeyConverter
   {
      public:
         explicit KeyConverter(
            std::pair<KeyToStringMap, StringToKeyMap> conversion_maps)
            : key_to_string(std::move(conversion_maps.first))
            , string_to_key(std::move(conversion_maps.second))
         {}

         auto to_string(const sf::Keyboard::Key key) const -> std::string_view
         {
            auto it = key_to_string.find(key);
            assert(it != key_to_string.cend() &&
               "Invalid sf::Keyboard::Key does not have string form.");
            return it->second;
         }

         auto to_key(const std::string_view str) const
            -> std::optional<sf::Keyboard::Key>
         {
            auto it = string_to_key.find(str);
            return it != string_to_key.cend()
               ? std::optional<sf::Keyboard::Key>{it->second}
               : std::nullopt;
         }

      private:
         KeyToStringMap key_to_string;
         StringToKeyMap string_to_key;
   };

   auto get_key_converter() -> KeyConverter const &
   {
      static constexpr auto init_conversion_maps =
         []() -> std::pair<KeyToStringMap, StringToKeyMap>
      {
         KeyToStringMap key_to_string;
         StringToKeyMap string_to_key;

         auto register_key = [&key_to_string, &string_to_key](
                                std::string_view key_name,
                                sf::Keyboard::Key key_code) -> void
         {
            key_to_string.emplace(key_code, key_name);
            string_to_key.emplace(key_name, key_code);
         };

         using namespace std::string_view_literals;
#define REGISTER_KEY(identifier) \
   register_key(#identifier##sv, sf::Keyboard::Key::identifier)

         REGISTER_KEY(A);
         REGISTER_KEY(B);
         REGISTER_KEY(C);
         REGISTER_KEY(D);
         REGISTER_KEY(E);
         REGISTER_KEY(F);
         REGISTER_KEY(G);
         REGISTER_KEY(H);
         REGISTER_KEY(I);
         REGISTER_KEY(J);
         REGISTER_KEY(K);
         REGISTER_KEY(L);
         REGISTER_KEY(M);
         REGISTER_KEY(N);
         REGISTER_KEY(O);
         REGISTER_KEY(P);
         REGISTER_KEY(Q);
         REGISTER_KEY(R);
         REGISTER_KEY(S);
         REGISTER_KEY(T);
         REGISTER_KEY(U);
         REGISTER_KEY(V);
         REGISTER_KEY(W);
         REGISTER_KEY(X);
         REGISTER_KEY(Y);
         REGISTER_KEY(Z);
         REGISTER_KEY(Num0);
         REGISTER_KEY(Num1);
         REGISTER_KEY(Num2);
         REGISTER_KEY(Num3);
         REGISTER_KEY(Num4);
         REGISTER_KEY(Num5);
         REGISTER_KEY(Num6);
         REGISTER_KEY(Num7);
         REGISTER_KEY(Num8);
         REGISTER_KEY(Num9);
         REGISTER_KEY(Escape);
         REGISTER_KEY(LControl);
         REGISTER_KEY(LShift);
         REGISTER_KEY(LAlt);
         REGISTER_KEY(LSystem);
         REGISTER_KEY(RControl);
         REGISTER_KEY(RShift);
         REGISTER_KEY(RAlt);
         REGISTER_KEY(RSystem);
         REGISTER_KEY(Menu);
         REGISTER_KEY(LBracket);
         REGISTER_KEY(RBracket);
         REGISTER_KEY(Semicolon);
         REGISTER_KEY(Comma);
         REGISTER_KEY(Period);
         REGISTER_KEY(Apostrophe);
         REGISTER_KEY(Slash);
         REGISTER_KEY(Backslash);
         REGISTER_KEY(Grave);
         REGISTER_KEY(Equal);
         REGISTER_KEY(Hyphen);
         REGISTER_KEY(Space);
         REGISTER_KEY(Enter);
         REGISTER_KEY(Backspace);
         REGISTER_KEY(Tab);
         REGISTER_KEY(PageUp);
         REGISTER_KEY(PageDown);
         REGISTER_KEY(End);
         REGISTER_KEY(Home);
         REGISTER_KEY(Insert);
         REGISTER_KEY(Delete);
         REGISTER_KEY(Add);
         REGISTER_KEY(Subtract);
         REGISTER_KEY(Multiply);
         REGISTER_KEY(Divide);
         REGISTER_KEY(Left);
         REGISTER_KEY(Right);
         REGISTER_KEY(Up);
         REGISTER_KEY(Down);
         REGISTER_KEY(Numpad0);
         REGISTER_KEY(Numpad1);
         REGISTER_KEY(Numpad2);
         REGISTER_KEY(Numpad3);
         REGISTER_KEY(Numpad4);
         REGISTER_KEY(Numpad5);
         REGISTER_KEY(Numpad6);
         REGISTER_KEY(Numpad7);
         REGISTER_KEY(Numpad8);
         REGISTER_KEY(Numpad9);
         REGISTER_KEY(F1);
         REGISTER_KEY(F2);
         REGISTER_KEY(F3);
         REGISTER_KEY(F4);
         REGISTER_KEY(F5);
         REGISTER_KEY(F6);
         REGISTER_KEY(F7);
         REGISTER_KEY(F8);
         REGISTER_KEY(F9);
         REGISTER_KEY(F10);
         REGISTER_KEY(F11);
         REGISTER_KEY(F12);
         REGISTER_KEY(F13);
         REGISTER_KEY(F14);
         REGISTER_KEY(F15);
         REGISTER_KEY(Pause);

#undef REGISTER_KEY

         return {key_to_string, string_to_key};
      };

      static const KeyConverter key_converter{init_conversion_maps()};

      return key_converter;
   }

   using CmdToStringMap =
      std::unordered_map<jrag::input::command, std::string_view>;
   using StringToCmdMap =
      std::unordered_map<std::string_view, jrag::input::command>;

   class CommandConverter
   {
      public:
         explicit CommandConverter(
            std::pair<CmdToStringMap, StringToCmdMap> conversion_maps)
            : cmd_to_str{std::move(conversion_maps.first)}
            , str_to_cmd{std::move(conversion_maps.second)}
         {}

         auto to_string(
            const jrag::input::command cmd) const -> std::string_view
         {
            auto it = cmd_to_str.find(cmd);
            assert(it != cmd_to_str.cend() &&
               "Invalid jrag::input::command does not have string form.");
            return it->second;
         }

         auto to_cmd(const std::string_view str) const
            -> std::optional<jrag::input::command>
         {
            auto it = str_to_cmd.find(str);
            return it != str_to_cmd.cend()
               ? std::optional<jrag::input::command>{it->second}
               : std::nullopt;
         }

      private:
         CmdToStringMap cmd_to_str;
         StringToCmdMap str_to_cmd;
   };

   auto get_cmd_converter() -> CommandConverter const &
   {
      static constexpr auto init_conversion_maps =
         []() -> std::pair<CmdToStringMap, StringToCmdMap>
      {
         CmdToStringMap cmd_to_str;
         StringToCmdMap str_to_cmd;

         auto register_command = [&cmd_to_str, &str_to_cmd](
                                    std::string_view cmd_str,
                                    jrag::input::command cmd_code) -> void
         {
            cmd_to_str.emplace(cmd_code, cmd_str);
            str_to_cmd.emplace(cmd_str, cmd_code);
         };

         using namespace std::string_view_literals;
#define REGISTER_COMMAND(identifier) \
   register_command(#identifier##sv, jrag::input::command::identifier)

         REGISTER_COMMAND(MoveRight);
         REGISTER_COMMAND(MoveUp);
         REGISTER_COMMAND(MoveLeft);
         REGISTER_COMMAND(MoveDown);
         REGISTER_COMMAND(Shoot);
         REGISTER_COMMAND(Jump);
         REGISTER_COMMAND(ScrollUp);
         REGISTER_COMMAND(ScrollDown);

#undef REGISTER_COMMAND

         return {cmd_to_str, str_to_cmd};
      };

      static const CommandConverter cmd_converter{init_conversion_maps()};

      return cmd_converter;
   }

   using MouseButtonToStringMap =
      std::unordered_map<sf::Mouse::Button, std::string_view>;
   using StringToMouseButtonMap =
      std::unordered_map<std::string_view, sf::Mouse::Button>;

   class MouseButtonConverter
   {
      public:
         explicit MouseButtonConverter(
            std::pair<MouseButtonToStringMap, StringToMouseButtonMap>
               conversion_maps)
            : button_to_string(std::move(conversion_maps.first))
            , string_to_button(std::move(conversion_maps.second))
         {}

         auto to_string(
            const sf::Mouse::Button button) const -> std::string_view
         {
            auto it = button_to_string.find(button);
            assert(it != button_to_string.cend() &&
               "Invalid sf::Mouse::Button does not have string form.");
            return it->second;
         }

         auto to_button(const std::string_view str) const
            -> std::optional<sf::Mouse::Button>
         {
            auto it = string_to_button.find(str);
            return it != string_to_button.cend()
               ? std::optional<sf::Mouse::Button>{it->second}
               : std::nullopt;
         }

      private:
         MouseButtonToStringMap button_to_string;
         StringToMouseButtonMap string_to_button;
   };

   auto get_mouse_converter() -> MouseButtonConverter const &
   {
      static constexpr auto init_conversion_maps =
         []() -> std::pair<MouseButtonToStringMap, StringToMouseButtonMap>
      {
         MouseButtonToStringMap button_to_string;
         StringToMouseButtonMap string_to_button;

         auto register_key = [&button_to_string, &string_to_button](
                                std::string_view button_name,
                                sf::Mouse::Button button) -> void
         {
            button_to_string.emplace(button, button_name);
            string_to_button.emplace(button_name, button);
         };

         using namespace std::string_view_literals;
#define REGISTER_BUTTON(identifier) \
   register_key("MB" #identifier##sv, sf::Mouse::identifier)

         REGISTER_BUTTON(Left);
         REGISTER_BUTTON(Right);
         REGISTER_BUTTON(Middle);
         REGISTER_BUTTON(XButton1);
         REGISTER_BUTTON(XButton2);

#undef REGISTER_BUTTON

         return {button_to_string, string_to_button};
      };

      static const MouseButtonConverter mouse_converter{init_conversion_maps()};

      return mouse_converter;
   }
}

namespace jrag::input::parser
{
   auto key_to_str(sf::Keyboard::Key const key) -> std::string_view
   {
      return get_key_converter().to_string(key);
   }

   auto str_to_key(
      std::string_view const str) -> std::optional<sf::Keyboard::Key>
   {
      return get_key_converter().to_key(str);
   }

   auto cmd_to_str(const jrag::input::command cmd) -> std::string_view
   {
      return get_cmd_converter().to_string(cmd);
   }

   auto str_to_cmd(
      std::string_view const str) -> std::optional<jrag::input::command>
   {
      return get_cmd_converter().to_cmd(str);
   }

   auto mb_to_str(sf::Mouse::Button const button) -> std::string_view
   {
      return get_mouse_converter().to_string(button);
   }

   auto str_to_mb(
      std::string_view const str) -> std::optional<sf::Mouse::Button>
   {
      return get_mouse_converter().to_button(str);
   }

   auto mw_to_str(
      jrag::input::mouse_wheel::scroll_direction const dir) -> std::string_view
   {
      return dir == jrag::input::mouse_wheel::scroll_direction::Up ? "MWUp"
                                                                   : "MWDown";
   }

   auto str_to_mw(const std::string_view str)
      -> std::optional<jrag::input::mouse_wheel::scroll_direction>
   {
      static constexpr std::string_view up_string{"MWUp"};
      static constexpr std::string_view down_string{"MWDown"};

      using direction = jrag::input::mouse_wheel::scroll_direction;

      if (str == up_string) { return direction::Up; }

      if (str == down_string) { return direction::Down; }

      return std::nullopt;
   }
}