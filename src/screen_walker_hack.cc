#include "screen_walker_hack.h"

// standard library includes
#include <cassert>
#ifndef NDEBUG
#include <iostream>
#endif

// third party includes
#include <raylib.h>

// local includes
#include "ems.h"

static const char *WALKER_HACK_SCREEN_INSTRUCTIONS =
    "Press The Correct Button!";

WalkerHackScreen::WalkerHackScreen(ScreenStack::Weak ss_weak,
                                   bool *walker_hack_success)
    : Screen(ss_weak),
      walker_hack_success(walker_hack_success),
      timer(WALKER_HACK_SCREEN_DURATION),
      font_size(10),
      instructions_size(1),
      instructions_font_size(font_size),
      type_f_size(1),
      type_j_size(1),
      type_a_size(1),
      type_l_size(1),
      button_type(BUTTON_TYPE_F) {
  button_type =
      (ButtonType)((int)(call_js_get_random() * (float)BUTTON_TYPE_SIZE));
  *walker_hack_success = false;
  set_sizes();
}

WalkerHackScreen::~WalkerHackScreen() {}

bool WalkerHackScreen::update(float dt, bool resized) {
  if (resized) {
    set_sizes();
  }

  timer -= dt;
  if (timer < 0.0F) {
#ifndef NDEBUG
    std::clog << "WalkerHackScreen: timer ended.\n";
#endif
    timer = WALKER_HACK_SCREEN_DURATION;
    auto s_stack = stack.lock();
    if (s_stack) {
      s_stack->pop_screen();
    }
  }

  if (IsMouseButtonPressed(0)) {
    switch (button_type) {
      case BUTTON_TYPE_F:
        if (GetTouchX() >=
                GetScreenWidth() / 2 - type_f_size - BUTTON_DRAW_OFFSET &&
            GetTouchX() <= GetScreenWidth() / 2 - BUTTON_DRAW_OFFSET &&
            GetTouchY() <= font_size) {
          *walker_hack_success = true;
          button_type = BUTTON_TYPE_SIZE;
        }
        break;
      case BUTTON_TYPE_J:
        if (GetTouchX() >= GetScreenWidth() / 2 + BUTTON_DRAW_OFFSET &&
            GetTouchX() <=
                GetScreenWidth() / 2 + BUTTON_DRAW_OFFSET + type_j_size &&
            GetTouchY() <= font_size) {
          *walker_hack_success = true;
          button_type = BUTTON_TYPE_SIZE;
        }
        break;
      case BUTTON_TYPE_A:
        if (GetTouchX() >=
                GetScreenWidth() / 2 - type_f_size - BUTTON_DRAW_OFFSET &&
            GetTouchX() <= GetScreenWidth() / 2 - BUTTON_DRAW_OFFSET &&
            GetTouchY() >= font_size + BUTTON_DRAW_OFFSET &&
            GetTouchY() <= font_size * 2 + BUTTON_DRAW_OFFSET) {
          *walker_hack_success = true;
          button_type = BUTTON_TYPE_SIZE;
        }
        break;
      case BUTTON_TYPE_L:
        if (GetTouchX() >= GetScreenWidth() / 2 + BUTTON_DRAW_OFFSET &&
            GetTouchX() <=
                GetScreenWidth() / 2 + BUTTON_DRAW_OFFSET + type_l_size &&
            GetTouchY() >= font_size + BUTTON_DRAW_OFFSET &&
            GetTouchY() <= font_size * 2 + BUTTON_DRAW_OFFSET) {
          *walker_hack_success = true;
          button_type = BUTTON_TYPE_SIZE;
        }
        break;
      default:
        break;
    }
  }

  switch (GetKeyPressed()) {
    case KEY_F:
      if (button_type == BUTTON_TYPE_F) {
        *walker_hack_success = true;
        button_type = BUTTON_TYPE_SIZE;
      }
      break;
    case KEY_J:
      if (button_type == BUTTON_TYPE_J) {
        *walker_hack_success = true;
        button_type = BUTTON_TYPE_SIZE;
      }
      break;
    case KEY_A:
      if (button_type == BUTTON_TYPE_A) {
        *walker_hack_success = true;
        button_type = BUTTON_TYPE_SIZE;
      }
      break;
    case KEY_L:
      if (button_type == BUTTON_TYPE_L) {
        *walker_hack_success = true;
        button_type = BUTTON_TYPE_SIZE;
      }
      break;
    default:
      break;
  }
  return false;
}

bool WalkerHackScreen::draw(RenderTexture *render_texture) {
  BeginTextureMode(*render_texture);

  DrawRectangle(GetScreenWidth() / 2 - instructions_size / 2,
                (GetScreenHeight() / 4) * 3, instructions_size,
                instructions_font_size, BLACK);
  DrawText(WALKER_HACK_SCREEN_INSTRUCTIONS,
           GetScreenWidth() / 2 - instructions_size / 2,
           (GetScreenHeight() / 4) * 3, instructions_font_size, WHITE);

  float ratio =
      (WALKER_HACK_SCREEN_DURATION - timer) / WALKER_HACK_SCREEN_DURATION;
  DrawRectangle(GetScreenWidth() * ratio,
                (float)GetScreenHeight() * BUTTON_FONT_SIZE_RATIO * 2.0F +
                    BUTTON_DRAW_OFFSET * 3.0F,
                GetScreenWidth() * (1.0F - ratio), 30, GREEN);

  DrawRectangle(GetScreenWidth() / 2 - type_f_size - BUTTON_DRAW_OFFSET, 0,
                type_f_size, font_size,
                button_type == BUTTON_TYPE_F ? GREEN : BLACK);
  DrawText("F", GetScreenWidth() / 2 - type_f_size - BUTTON_DRAW_OFFSET, 0,
           font_size, WHITE);

  DrawRectangle(GetScreenWidth() / 2 + BUTTON_DRAW_OFFSET, 0, type_j_size,
                font_size, button_type == BUTTON_TYPE_J ? GREEN : BLACK);
  DrawText("J", GetScreenWidth() / 2 + BUTTON_DRAW_OFFSET, 0, font_size, WHITE);

  DrawRectangle(GetScreenWidth() / 2 - type_a_size - BUTTON_DRAW_OFFSET,
                font_size + BUTTON_DRAW_OFFSET, type_a_size, font_size,
                button_type == BUTTON_TYPE_A ? GREEN : BLACK);
  DrawText("A", GetScreenWidth() / 2 - type_a_size - BUTTON_DRAW_OFFSET,
           font_size + BUTTON_DRAW_OFFSET, font_size, WHITE);

  DrawRectangle(GetScreenWidth() / 2 + BUTTON_DRAW_OFFSET,
                font_size + BUTTON_DRAW_OFFSET, type_l_size, font_size,
                button_type == BUTTON_TYPE_L ? GREEN : BLACK);
  DrawText("L", GetScreenWidth() / 2 + BUTTON_DRAW_OFFSET,
           font_size + BUTTON_DRAW_OFFSET, font_size, WHITE);

  EndTextureMode();

  return true;
}

void WalkerHackScreen::set_sizes() {
  font_size = GetScreenHeight() * BUTTON_FONT_SIZE_RATIO;
  instructions_font_size = font_size;
  instructions_size =
      MeasureText(WALKER_HACK_SCREEN_INSTRUCTIONS, instructions_font_size);
  while (instructions_size > GetScreenWidth() && instructions_font_size > 1) {
    --instructions_font_size;
    instructions_size =
        MeasureText(WALKER_HACK_SCREEN_INSTRUCTIONS, instructions_font_size);
  }

  type_f_size = MeasureText("F", font_size);
  type_j_size = MeasureText("J", font_size);
  type_a_size = MeasureText("A", font_size);
  type_l_size = MeasureText("L", font_size);
}
