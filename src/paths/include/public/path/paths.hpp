#include <filesystem>

namespace jrag::path
{
   auto executable() -> std::filesystem::path const &;
   auto executable_dir() -> std::filesystem::path const &;
   auto assets_dir() -> std::filesystem::path const &;
}
