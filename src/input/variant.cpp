#include <input/variant.hpp>

using namespace jrag;

auto input::variant::get_type() const -> type { return m_type; }

auto input::operator==(
   const input::variant & lhs, const input::variant & rhs) -> bool
{
   return lhs.m_variant == rhs.m_variant;
}
