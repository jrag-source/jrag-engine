#include "game.hpp"
#include "math/vector2.hpp"
#include "pause/pause.hpp"

#include <app_state/command.hpp>
#include <input/dispatcher.hpp>
#include <path/paths.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Time.hpp>

#include <cmath>
#include <print>
#include <random>

#include <collision/collider.hpp>
#include <collision/map.hpp>

using namespace states;

namespace
{
   sf::Texture make_goon_texture()
   {
      sf::Texture texture;
      texture.loadFromFile(jrag::path::assets_dir() / "goon.png", sf::IntRect { 0, 1, 16 , 14 });
      return texture;
   }

   sf::Sprite make_goon_sprite(const sf::Texture& texture)
   {
      sf::Sprite sprite { texture };
      sprite.setScale(10.0f, 10.0f);
      return sprite;
   }

   sf::Texture make_medkit_texture()
   {
      sf::Texture texture;
      texture.loadFromFile(jrag::path::assets_dir() / "medkit.png");
      return texture;
   }

   sf::Sprite make_medkit_sprite(const sf::Texture& texture)
   {
      sf::Sprite sprite { texture };
      sprite.setScale(10.0f, 10.0f);
      return sprite;
   }

   auto get_uuid() -> std::string
   {
      // TODO: Add uuid functionality in its own module
      // TODO: Make uuid generator seedable, if not seeded, then defaults to using a truly random seed.
      static std::random_device dev;
      static std::mt19937 rng(dev());

      std::uniform_int_distribution<int> dist(0, 15);

      const char *v = "0123456789abcdef";
      const bool dash[] = { 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0 };

      std::string res;
      for (int i = 0; i < 16; i++) {
         if (dash[i]) res += "-";
         res += v[dist(rng)];
         res += v[dist(rng)];
      }
      
      return res;
   }

   auto const player_collider_id {get_uuid()};
   auto const medkit_collider_id {get_uuid()};

   // jrag::collision::collider m_player{.position = {50.0F, 50.0F}, .size = {50.0F, 50.0F}};
   // jrag::collision::collider m_medkit{.position = {200.0F, 200.0F}, .size = {50.0F, 50.0F}};

   enum class tile_type : std::uint8_t
   {
      empty,
      wall,
      spike,

      size,
   };

   template<typename tile_enum, unsigned long width, unsigned long height>
   class logical_tilemap
   {
      public:
         using row_t = std::array<tile_enum, width>;
         using grid_t = std::array<row_t, height>;

         explicit constexpr logical_tilemap(grid_t data)
            : m_data{data}
         {
            static_assert(static_cast<unsigned int>(tile_enum::empty) == 0U, "tile_enum must define empty as its first tile type");
         }

         [[nodiscard]] auto get_iterable() const -> std::span<row_t const>
         {
            return m_data;
         }
      
         private:
            grid_t m_data;
   };

   constexpr logical_tilemap my_map
   {
      std::array{
         std::array{tile_type::wall, tile_type::empty, tile_type::empty, tile_type::empty,},
         std::array{tile_type::wall, tile_type::empty, tile_type::empty, tile_type::empty,},
         std::array{tile_type::wall, tile_type::empty, tile_type::empty, tile_type::empty,},
         std::array{tile_type::wall, tile_type::empty, tile_type::empty, tile_type::empty,},
      }
   };

   using wowser = decltype(my_map);
}

game::game()
   : m_goon_texture{make_goon_texture()}
   , m_goon_sprite{make_goon_sprite(m_goon_texture)}
   , m_medkit_texture{make_medkit_texture()}
   , m_medkit_sprite{make_medkit_sprite(m_medkit_texture)}
   , m_medkit_x_dir{1}
   , m_pause_state{}
   , m_entities{}
   , m_world{}
{  
   m_world.create_moving_collider(player_collider_id, {50.0F, 50.0F}, {50.0F, 50.0F});
   m_world.create_moving_collider(medkit_collider_id, {200.0F, 200.0F}, {50.0F, 50.0F});

   jrag::math::vector2 position{0.0F, 0.0F};
   jrag::math::vector2 const size{50.0F, 50.0F};
   
   for (auto const row : my_map.get_iterable())
   {
      for (auto const tile : row)
      {
         if (tile == tile_type::wall)
         {  
            m_entities.emplace_back(get_uuid());
            m_world.create_stationary_collider(m_entities.back(), position, size);
         }

         position.x += size.x;
      }

      position.x = 0.0F;
      position.y += size.y;
   }

   auto const & m_player {m_world.get_collider(player_collider_id)};
   auto const & m_medkit {m_world.get_collider(medkit_collider_id)};

   m_goon_sprite.setOrigin(m_goon_sprite.getLocalBounds().getSize().x / 2, m_goon_sprite.getLocalBounds().getSize().y / 2);
   m_goon_sprite.setScale(m_player.get_size().x / m_goon_sprite.getLocalBounds().width, m_player.get_size().y / m_goon_sprite.getLocalBounds().height);

   m_medkit_sprite.setOrigin(m_medkit_sprite.getLocalBounds().getSize().x / 2, m_medkit_sprite.getLocalBounds().getSize().y / 2);
   m_medkit_sprite.setScale(m_medkit.get_size().x / m_medkit_sprite.getLocalBounds().width, m_medkit.get_size().y / m_medkit_sprite.getLocalBounds().height);
}

game::~game() = default;

void game::logic_process(logic_tooling tooling, std::chrono::duration<float> const dt)
{
   if (m_pause_state.is_active())
   {
      return;
   }
   
   jrag::math::vector2 move_dir {0.0F, 0.0F};

   using Cmd = jrag::input::command;

   if (tooling.get_input_dispatcher().is_command_active(Cmd::MoveRight))
   {
      move_dir.x += 1.0f;
   }

   if (tooling.get_input_dispatcher().is_command_active(Cmd::MoveUp))
   {
      move_dir.y -= 1.0f;
   }

   if (tooling.get_input_dispatcher().is_command_active(Cmd::MoveLeft))
   {
      move_dir.x -= 1.0f;
   }

   if (tooling.get_input_dispatcher().is_command_active(Cmd::MoveDown))
   {
      move_dir.y += 1.0f;
   }

   const float magnitude { std::sqrt( (move_dir.x * move_dir.x) + (move_dir.y * move_dir.y) ) };
   
   if (magnitude != 0.0f) // TODO: Potentially bad comparison with 0. OK as long as there is an exact representation for 0.0f in the hardware. Fine for now...
   {
      move_dir.x /= magnitude;
      move_dir.y /= magnitude;
   }

   constexpr auto move_speed {200.0F};

   auto const displacement { move_dir * move_speed * dt.count() };

   auto & m_player {m_world.get_collider(player_collider_id)};
   m_player.set_center(m_player.get_center() + displacement);

   // std::println("process collisions");
   m_world.handle_collisions();

   if (tooling.get_input_dispatcher().is_command_activating(Cmd::Shoot))
   {
      std::print("Shoot active\n");
   }

   if (tooling.get_input_dispatcher().is_command_active(Cmd::ScrollUp))
   {
      std::print("ScrollUp active\n");
   }

   if (tooling.get_input_dispatcher().is_command_active(Cmd::ScrollDown))
   {
      std::print("ScrollDown active\n");
   }
}

void game::graphics_process(graphics_tooling tooling, float const)
{
   auto & window{tooling.get_window()};

   auto const & m_player {m_world.get_collider(player_collider_id)};
   auto const & m_medkit {m_world.get_collider(medkit_collider_id)};

   m_goon_sprite.setPosition(m_player.get_center().x, m_player.get_center().y);
   m_medkit_sprite.setPosition(m_medkit.get_center().x, m_medkit.get_center().y);

   sf::View playerView{m_goon_sprite.getPosition(), window.getDefaultView().getSize()};
   playerView.zoom(0.5f);
   window.setView(playerView);

   window.draw(m_goon_sprite);

   sf::RectangleShape goon_rect{{m_player.get_size().x, m_player.get_size().y}};
   goon_rect.setOutlineColor(sf::Color::Red);
   goon_rect.setOutlineThickness(-5.0F);
   goon_rect.setFillColor(sf::Color::Transparent);
   goon_rect.setOrigin(goon_rect.getLocalBounds().getSize().x / 2, goon_rect.getLocalBounds().getSize().y / 2);
   goon_rect.setPosition({m_player.get_center().x, m_player.get_center().y});
   window.draw(goon_rect);

   window.draw(m_medkit_sprite);

   sf::RectangleShape medkit_rect{{m_medkit.get_size().x, m_medkit.get_size().y}};
   medkit_rect.setOutlineColor(sf::Color::Red);
   medkit_rect.setOutlineThickness(-5.0F);
   medkit_rect.setFillColor(sf::Color::Transparent);
   medkit_rect.setOrigin(medkit_rect.getLocalBounds().getSize().x / 2, medkit_rect.getLocalBounds().getSize().y / 2);
   medkit_rect.setPosition({m_medkit.get_center().x, m_medkit.get_center().y});
   window.draw(medkit_rect);

   for (std::string_view const entity_id : m_entities)
   {
      auto const & entity {m_world.get_collider(entity_id)};
      
      sf::RectangleShape entity_rect{{entity.get_size().x, entity.get_size().y}};
      entity_rect.setOutlineColor(sf::Color::Yellow);
      entity_rect.setOutlineThickness(-5.0F);
      entity_rect.setFillColor(sf::Color::Transparent);
      entity_rect.setOrigin(entity_rect.getLocalBounds().getSize().x / 2, entity_rect.getLocalBounds().getSize().y / 2);
      entity_rect.setPosition({entity.get_center().x, entity.get_center().y});
      window.draw(entity_rect);

      // std::println("entity_rect pos: {{ {}, {} }}", entity_rect.getPosition().x, entity_rect.getPosition().y);
   }
}

void game::on_enter()
{
   std::print("{}\n", __PRETTY_FUNCTION__);
}

jrag::app_state::command game::eval_manager_command(logic_tooling tooling)
{
   using input_command = jrag::input::command;

   if (tooling.get_input_dispatcher().is_command_activating(input_command::Jump))
   {
      std::print("game_state exit\n");
      return jrag::app_state::command::pop();
   }
   else if (tooling.get_input_dispatcher().is_command_activating(input_command::ScrollUp))
   {
      std::print("game state push pause\n");
      return jrag::app_state::command::push(m_pause_state);
   }

   return jrag::app_state::command::none();
}
