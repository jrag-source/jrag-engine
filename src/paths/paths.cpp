#include <path/paths.hpp>

#include "whereami.h"

#include <filesystem>
#include <vector>

auto jrag::path::executable() -> std::filesystem::path const &
{
   using path = std::filesystem::path;
   static const path exec_path = []() -> path
   {
      const int length = wai_getExecutablePath(nullptr, 0, nullptr);
      std::vector<char> path(length + 1);

      wai_getExecutablePath(path.data(), length, nullptr);
      path[length] = '\0';

      return std::filesystem::path{path.data()};
   }();

   return exec_path;
}

auto jrag::path::executable_dir() -> std::filesystem::path const &
{
   using path = std::filesystem::path;
   static const path dir_path = []() -> path
   { return executable().parent_path(); }();

   return dir_path;
}

auto jrag::path::assets_dir() -> std::filesystem::path const &
{
   using path = std::filesystem::path;
   static const path dir_path = []() -> path
   { return (path{executable_dir()} / "assets/"); }();

   return dir_path;
}