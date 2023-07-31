#include "screen.h"

Screen::Screen(std::weak_ptr<ScreenStack> stack) : stack(stack) {}

ScreenStack::Ptr ScreenStack::new_instance() {
  std::shared_ptr<ScreenStack> ptr =
      std::shared_ptr<ScreenStack>(new ScreenStack{});
  ptr->self_weak = ptr;
  return ptr;
}

void ScreenStack::update(float dt) {
  auto idx = stack.size();
  while (idx > 0 && stack.at(--idx)->update(dt)) {
  }
}

void ScreenStack::draw() {
  auto idx = stack.size();
  while (idx > 0 && stack.at(--idx)->draw()) {
  }
}

void ScreenStack::push_screen(Screen::Ptr &&screen) {
  stack.emplace_back(std::forward<Screen::Ptr>(screen));
}

void ScreenStack::pop_screen() {
  if (!stack.empty()) {
    stack.pop_back();
  }
}

ScreenStack::ScreenStack() : self_weak(), stack() {}
