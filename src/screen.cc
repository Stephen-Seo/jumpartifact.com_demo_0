#include "screen.h"

// standard library includes
#include <cassert>
#ifndef NDEBUG
#include <iostream>
#endif  // NDEBUG

// local includes
#include "screen_test.h"

Screen::Screen(std::weak_ptr<ScreenStack> stack) : stack(stack) {}

ScreenStack::PendingAction::PendingAction() : screen(), action(Action::NOP) {}

ScreenStack::PendingAction::PendingAction(Action action)
    : screen(), action(action) {
  switch (action) {
    case Action::PUSH_SCREEN:
    case Action::CONSTRUCT_SCREEN:
      // Cannot push non-existant screen.
      this->action = Action::NOP;
#ifndef NDEBUG
      std::clog << "WARNING: Cannot create PendingAction with PUSH_SCREEN or "
                   "CONSTRUCT_SCREEN!\n";
#endif
      break;
    default:
      break;
  }
}

ScreenStack::PendingAction::PendingAction(Screen::Ptr &&screen)
    : screen(std::forward<Screen::Ptr>(screen)), action(Action::PUSH_SCREEN) {}

ScreenStack::PendingAction::PendingAction(
    std::function<Screen::Ptr(ScreenStack::Weak)> &&fn)
    : screen(std::forward<std::function<Screen::Ptr(ScreenStack::Weak)> >(fn)),
      action(Action::CONSTRUCT_SCREEN) {}

ScreenStack::Ptr ScreenStack::new_instance() {
  std::shared_ptr<ScreenStack> ptr =
      std::shared_ptr<ScreenStack>(new ScreenStack{});
  ptr->self_weak = ptr;
  return ptr;
}

void ScreenStack::update(float dt) {
  handle_pending_actions();

  auto idx = stack.size();
  if (idx == 0) {
#ifndef NDEBUG
    std::cerr << "WARNING: Stack is empty, pushing TestScreen...\n";
#endif  // NDEBUG
    push_screen(Screen::new_screen<TestScreen>(self_weak));
    update(dt);
    return;
  }
  while (idx > 0 && stack.at(--idx)->update(dt)) {
  }
}

void ScreenStack::draw() {
  auto idx = stack.size();
  while (idx > 0 && stack.at(--idx)->draw()) {
  }
}

void ScreenStack::push_screen(Screen::Ptr &&screen) {
  actions.push_back(PendingAction(std::forward<Screen::Ptr>(screen)));
}

void ScreenStack::pop_screen() {
  actions.push_back(PendingAction(Action::POP_SCREEN));
}

void ScreenStack::clear_screens() {
  actions.push_back(PendingAction(Action::CLEAR_SCREENS));
}

ScreenStack::ScreenStack() : self_weak(), stack(), actions() {}

void ScreenStack::handle_pending_actions() {
  while (!actions.empty()) {
    switch (actions.front().action) {
      case Action::PUSH_SCREEN:
        stack.emplace_back(
            std::move(std::get<Screen::Ptr>(actions.front().screen)));
        break;
      case Action::POP_SCREEN:
        if (!stack.empty()) {
          stack.pop_back();
        }
#ifndef NDEBUG
        else {
          std::cerr << "WARNING: Tried to pop screen when stack was empty!\n";
        }
#endif  // NDEBUG
        break;
      case Action::CLEAR_SCREENS:
#ifndef NDEBUG
        if (stack.empty()) {
          std::cerr << "WARNING: Clearing an empty screen stack!\n";
        }
#endif
        stack.clear();
        break;
      case Action::CONSTRUCT_SCREEN:
        stack.emplace_back(
            std::get<std::function<Screen::Ptr(ScreenStack::Weak)> >(
                actions.front().screen)(self_weak));
        break;
      case Action::NOP:
        // Intentionally left blank.
        break;
      default:
        assert(!"unreachable");
        break;
    }
    actions.pop_front();
  }
}
