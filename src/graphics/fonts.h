#pragma once

#include <SDL3_ttf/SDL_ttf.h>

#include <memory>

class FontRef {
 private:
  struct TTF_FontDeleter {
    void operator()(TTF_Font* font) const;
  };

  std::unique_ptr<TTF_Font, TTF_FontDeleter> font_ptr;

 public:
  FontRef();
  FontRef(TTF_Font* font);

  static FontRef load();

  // Method to load font after initialization
  void loadFont();

  TTF_Font* get() const;
  TTF_Font* operator->() const;
  TTF_Font& operator*() const;
};
