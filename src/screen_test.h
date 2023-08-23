#ifndef JUMPARTIFACT_DOT_COM_DEMO_O_TEST_SCREEN_H_
#define JUMPARTIFACT_DOT_COM_DEMO_O_TEST_SCREEN_H_

#include "screen.h"

class TestScreen : public Screen {
 public:
  TestScreen(ScreenStack::Weak weak_ptr);
  ~TestScreen() override;

  bool update(float dt, bool is_resized) override;
  bool draw(RenderTexture *render_texture) override;

 private:
  float TEMP_cached_dt;
};

#endif
