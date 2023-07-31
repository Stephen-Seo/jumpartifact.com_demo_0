#ifndef JUMPARTIFACT_DOT_COM_DEMO_GAME_H_
#define JUMPARTIFACT_DOT_COM_DEMO_GAME_H_

class Game {
 public:
  Game();

  // No copy.
  Game(const Game&) = delete;
  Game& operator=(const Game&) = delete;

  // Allow move.
  Game(Game&&) = default;
  Game& operator=(Game&&) = default;

  void update();
  void draw();

 private:
};

#endif
