#include "fonts.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <string>

#include "embedded_fonts/space_mono_regular.h"

const int DEFAULT_FONT_SIZE = 24;

void FontRef::TTF_FontDeleter::operator()(TTF_Font* font) const {
  if (font) {
    TTF_CloseFont(font);
  }
}

FontRef::FontRef() : font_ptr(nullptr) {}

FontRef::FontRef(TTF_Font* font) : font_ptr(font) {
  if (!font) {
    throw std::invalid_argument("Font cannot be null");
  }
}

FontRef FontRef::load() {
  // Load font from embedded data using SDL_IOStream
  SDL_IOStream* io =
      SDL_IOFromConstMem(assets_fonts_SpaceMono_Regular_ttf,
                         sizeof(assets_fonts_SpaceMono_Regular_ttf));
  if (!io) {
    throw std::runtime_error("Failed to create IOStream for embedded font");
  }

  TTF_Font* loaded_font = TTF_OpenFontIO(io, 1, DEFAULT_FONT_SIZE);
  if (!loaded_font) {
    SDL_CloseIO(io);
    throw std::runtime_error("Failed to load embedded font: " +
                             std::string(SDL_GetError()));
  }

  return FontRef(loaded_font);
}

void FontRef::loadFont() {
  if (font_ptr) {
    return;
  }

  SDL_IOStream* io =
      SDL_IOFromConstMem(assets_fonts_SpaceMono_Regular_ttf,
                         sizeof(assets_fonts_SpaceMono_Regular_ttf));
  if (!io) {
    SPDLOG_ERROR("Failed to create IOStream for embedded font");
    throw std::runtime_error("Failed to create IOStream for embedded font");
  }

  TTF_Font* loaded_font = TTF_OpenFontIO(io, 1, DEFAULT_FONT_SIZE);
  if (!loaded_font) {
    SDL_CloseIO(io);
    SPDLOG_ERROR("Failed to load embedded font: %s", SDL_GetError());
    throw std::runtime_error("Failed to load embedded font: " +
                             std::string(SDL_GetError()));
  }

  SPDLOG_INFO("Embedded font loaded successfully");
  font_ptr.reset(loaded_font);
}

TTF_Font* FontRef::get() const { return font_ptr.get(); }

TTF_Font* FontRef::operator->() const { return font_ptr.get(); }

TTF_Font& FontRef::operator*() const { return *font_ptr; }
