#ifndef JUMPARTIFACT_DOT_COM_DEMO_0_SCREEN_H_
#define JUMPARTIFACT_DOT_COM_DEMO_0_SCREEN_H_

#include <raylib.h>

#include <deque>
#include <functional>
#include <memory>
#include <variant>
#include <vector>

// Forward declarations.
class ScreenStack;
struct RenderTexture;

class Screen {
 public:
  using Ptr = std::unique_ptr<Screen>;

  template <typename SubScreen>
  static Ptr new_screen(std::weak_ptr<ScreenStack> stack);

  template <typename SubScreen, typename... Args>
  static Ptr new_screen_args(std::weak_ptr<ScreenStack> stack, Args... args);

  virtual ~Screen() {}

  // No copy.
  Screen(const Screen&) = delete;
  Screen& operator=(const Screen&) = delete;

  // Allow move.
  Screen(Screen&&) = default;
  Screen& operator=(Screen&&) = default;

  /// Return true if next screen should be updated.
  virtual bool update(float dt, bool screen_resized) = 0;
  /// Return true if next screen should be drawn.
  virtual bool draw(RenderTexture* renderTexture) = 0;

 protected:
  Screen(std::weak_ptr<ScreenStack> stack);
  std::weak_ptr<ScreenStack> stack;
};

class ScreenStack {
 public:
  using Ptr = std::shared_ptr<ScreenStack>;
  using Weak = std::weak_ptr<ScreenStack>;

 private:
  enum Action { PUSH_SCREEN, POP_SCREEN, CLEAR_SCREENS, CONSTRUCT_SCREEN, NOP };

  struct PendingAction {
    PendingAction();
    PendingAction(Action action);
    PendingAction(Screen::Ptr&&);
    PendingAction(std::function<Screen::Ptr(ScreenStack::Weak)>&&);

    // No copy.
    PendingAction(const PendingAction&) = delete;
    PendingAction& operator=(const PendingAction&) = delete;

    // Allow move.
    PendingAction(PendingAction&&) = default;
    PendingAction& operator=(PendingAction&&) = default;

    std::variant<Screen::Ptr, std::function<Screen::Ptr(ScreenStack::Weak)> >
        screen;
    Action action;
  };

 public:
  static Ptr new_instance();

  ~ScreenStack();

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

  template <typename SubScreen>
  void push_constructing_screen();

  template <typename SubScreen, typename... Args>
  void push_constructing_screen_args(Args... args);

  void pop_screen();

  void clear_screens();

  void reset_render_texture();

 private:
  ScreenStack();

  void handle_pending_actions();

  std::unique_ptr<RenderTexture> render_texture;
  Weak self_weak;
  std::vector<Screen::Ptr> stack;
  std::deque<PendingAction> actions;
};

template <typename SubScreen>
Screen::Ptr Screen::new_screen(std::weak_ptr<ScreenStack> stack) {
  return std::unique_ptr<SubScreen>(new SubScreen{stack});
}

template <typename SubScreen, typename... Args>
Screen::Ptr Screen::new_screen_args(std::weak_ptr<ScreenStack> stack,
                                    Args... args) {
  return std::unique_ptr<SubScreen>(new SubScreen{stack, args...});
}

template <typename SubScreen>
void ScreenStack::push_screen() {
  actions.emplace_back(Screen::new_screen<SubScreen>(self_weak));
}

template <typename SubScreen>
void ScreenStack::push_constructing_screen() {
  actions.emplace_back([](ScreenStack::Weak ss) -> Screen::Ptr {
    return Screen::new_screen<SubScreen>(ss);
  });
}

template <typename SubScreen, typename... Args>
void ScreenStack::push_constructing_screen_args(Args... args) {
  actions.emplace_back([args...](ScreenStack::Weak ss) -> Screen::Ptr {
    return Screen::new_screen_args<SubScreen>(ss, args...);
  });
}

#endif
