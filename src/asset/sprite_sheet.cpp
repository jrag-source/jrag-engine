#include "asset/sprite_sheet.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include <utility>

using namespace jrag::asset::sprite_sheet;
using namespace jrag::math;
using namespace std::chrono_literals;

namespace
{
   auto get_cell_size(sf::Texture const & sheet, vector2u const dimensions,
      vector2u const offset) noexcept -> vector2u
   {
      auto const trimmed_size =
         vector2u{sheet.getSize().x, sheet.getSize().y} - (2U * offset);
      return {trimmed_size.x / dimensions.x, trimmed_size.y / dimensions.y};
   }

   auto get_sprite(sf::Texture const & sheet, vector2u const cell_size,
      vector2u const cell_position,
      vector2u const offset) noexcept -> sf::Sprite
   {
      auto const top_left{offset +
         vector2u{
            cell_size.x * cell_position.x, cell_size.y * cell_position.y}};
      return {sheet,
         sf::Rect{sf::Vector2{static_cast<int>(top_left.x),
                     static_cast<int>(top_left.y)},
            sf::Vector2{
               static_cast<int>(cell_size.x), static_cast<int>(cell_size.y)}}};
   }
}

auto view::get_sprite_impl(sf::Texture const & sheet, vector2u const dimensions,
   vector2u const cell_position, vector2u const offset) noexcept -> sf::Sprite
{
   auto const cell_size{get_cell_size(sheet, dimensions, offset)};
   return ::get_sprite(sheet, cell_size, cell_position, offset);
}

animation::animation(sf::Texture const & sheet, vector2u const dimensions,
   bool const loop, vector2u const offset,
   std::vector<frame_data> frames) noexcept
   : m_sheet{sheet}
   , m_dimensions{dimensions}
   , m_loop{loop}
   , m_offset{offset}
   , m_frames{std::move(frames)}
   , m_cell_size{get_cell_size(m_sheet, m_dimensions, m_offset)}
   , m_frame_index{0UZ}
   , m_elapsed_frame_time{0.0F}
{}

auto animation::advance(duration const delta_time) noexcept -> void
{
   if (delta_time < 0s) { return; }

   auto reached_end = [&self = std::as_const(*this)]() -> bool
   { return self.m_frame_index == self.m_frames.size() - 1UZ; };

   if (reached_end() && !m_loop) { return; }

   m_elapsed_frame_time += delta_time;

   auto const & current_frame{m_frames[m_frame_index]};

   while (m_elapsed_frame_time >= current_frame.m_duration)
   {
      m_elapsed_frame_time -= current_frame.m_duration;

      if (!reached_end()) { ++m_frame_index; }
      else if (m_loop) { m_frame_index = 0UZ; }
      else { break; }
   }
}

auto animation::get_frame() const noexcept -> sf::Sprite
{
   auto const & current_frame{m_frames[m_frame_index]};
   return get_sprite(
      m_sheet, m_cell_size, current_frame.m_cell_position, m_offset);
}
