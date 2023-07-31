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
    : screen(), action(action) {}

ScreenStack::PendingAction::PendingAction(Screen::Ptr &&screen)
    : screen(std::forward<Screen::Ptr>(screen)), action(Action::PUSH_SCREEN) {}

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

ScreenStack::ScreenStack() : self_weak(), stack(), actions() {}

void ScreenStack::handle_pending_actions() {
  while (!actions.empty()) {
    switch (actions.front().action) {
      case Action::PUSH_SCREEN:
        stack.push_back(std::move(actions.front().screen));
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
