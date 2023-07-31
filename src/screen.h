#ifndef JUMPARTIFACT_DOT_COM_DEMO_0_SCREEN_H_
#define JUMPARTIFACT_DOT_COM_DEMO_0_SCREEN_H_

#include <memory>
#include <vector>

// Forward declaration.
class ScreenStack;

class Screen {
 public:
  using Ptr = std::unique_ptr<Screen>;

  template <typename SubScreen>
  static Ptr new_screen(std::weak_ptr<ScreenStack> stack);

  virtual ~Screen() {}

  // No copy.
  Screen(const Screen&) = delete;
  Screen& operator=(const Screen&) = delete;

  // Allow move.
  Screen(Screen&&) = default;
  Screen& operator=(Screen&&) = default;

  /// Return true if next screen should be updated.
  virtual bool update(float dt) = 0;
  /// Return true if next screen should be drawn.
  virtual bool draw() = 0;

 protected:
  Screen(std::weak_ptr<ScreenStack> stack);

 private:
  std::weak_ptr<ScreenStack> stack;
};

class ScreenStack {
 public:
  using Ptr = std::shared_ptr<ScreenStack>;
  using Weak = std::weak_ptr<ScreenStack>;

  static Ptr new_instance();

  // No copy.
  ScreenStack(const ScreenStack&) = delete;
  ScreenStack& operator=(const ScreenStack&) = delete;

  // Allow move.
  ScreenStack(ScreenStack&&) = default;
  ScreenStack& operator=(ScreenStack&&) = default;

  void update(float dt);
  void draw();

  void push_screen(Screen::Ptr&& screen);

  template <typename SubScreen>
  void push_screen();

  void pop_screen();

 private:
  ScreenStack();

  Weak self_weak;
  std::vector<Screen::Ptr> stack;
};

template <typename SubScreen>
Screen::Ptr Screen::new_screen(std::weak_ptr<ScreenStack> stack) {
  return std::unique_ptr<SubScreen>(new SubScreen{stack});
}

template <typename SubScreen>
void ScreenStack::push_screen() {
  stack.emplace_back(Screen::new_screen<SubScreen>(self_weak));
}

#endif
