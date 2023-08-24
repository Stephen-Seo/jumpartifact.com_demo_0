#ifndef JUMPARTIFACT_DOT_COM_DEMO_0_WALKER_HACK_SCREEN_H_
#define JUMPARTIFACT_DOT_COM_DEMO_0_WALKER_HACK_SCREEN_H_

// local includes
#include "screen.h"

constexpr float WALKER_HACK_SCREEN_DURATION = 2.0F;
constexpr float BUTTON_FONT_SIZE_RATIO = 0.28F;
constexpr int BUTTON_DRAW_OFFSET = 5;

class WalkerHackScreen : public Screen {
 public:
  WalkerHackScreen(ScreenStack::Weak ss_weak, bool *walkerHackSuccess);
  ~WalkerHackScreen() override;

  // Disallow copy.
  WalkerHackScreen(const WalkerHackScreen &) = delete;
  WalkerHackScreen &operator=(const WalkerHackScreen &) = delete;

  // Allow move.
  WalkerHackScreen(WalkerHackScreen &&) = default;
  WalkerHackScreen &operator=(WalkerHackScreen &&) = default;

  bool update(float dt, bool is_resized) override;
  bool draw(RenderTexture *render_texture) override;

 private:
  enum ButtonType {
    BUTTON_TYPE_F = 0,
    BUTTON_TYPE_J,
    BUTTON_TYPE_A,
    BUTTON_TYPE_L,
    BUTTON_TYPE_SIZE
  };

  bool *walker_hack_success;
  float timer;
  int font_size;
  int instructions_size;
  int instructions_font_size;
  int type_f_size;
  int type_j_size;
  int type_a_size;
  int type_l_size;
  ButtonType button_type;
  bool screen_pop_requested;

  void set_sizes();
  void on_success();
};

#endif
