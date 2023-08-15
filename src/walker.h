#ifndef JUMPARTIFACT_DOT_COM_DEMO_0_WALKER_H_
#define JUMPARTIFACT_DOT_COM_DEMO_0_WALKER_H_

// standard library includes
#include <array>
#ifndef NDEBUG
#include <iostream>
#endif

// third party includes
#include <raylib.h>
#include <raymath.h>

// local includes
#include "3d_helpers.h"
#include "common_constants.h"
#include "ems.h"

constexpr float FEET_RADIUS_PLACEMENT_CHECK_SCALE = 1.0F;
constexpr float FEET_RADIUS_PLACEMENT_SCALE = 0.9F;
constexpr float FEET_TARGET_RATE = 1.0F;
constexpr float BODY_TARGET_SPEED = 2.0F;
constexpr float FEET_LIFT_HEIGHT = 1.0F;
constexpr float FEET_LIFT_SPEED = 5.5F;
constexpr float FEET_HORIZ_MOVE_SPEED = 8.0F;
constexpr float FEET_INIT_POS_VARIANCE_DIV = 3.0F;
constexpr float BODY_ROTATION_SPEED = 1.0F;
constexpr float BODY_IDLE_TIMER_RATE = 1.0F;
constexpr float BODY_IDLE_MOVE_AMOUNT = 0.2F;
constexpr float ROAMING_WAIT_AMOUNT = 2.0F;
constexpr float ROAMING_WAIT_VARIANCE = 7.0F;

class Walker {
 public:
  Walker(float x, float z, bool auto_roaming, float body_height = 2.0F,
         float body_feet_radius = 1.7F, float feet_radius = 1.5F);

  template <std::size_t BBCount>
  void update(float dt, const std::array<BoundingBox, BBCount> &bbs,
              unsigned int width, unsigned int height);

  void draw(const Model &model);

  void set_body_pos(Vector3 pos);

  void set_player_controlled(bool player_controlled);
  void player_idle();
  void player_turn_left();
  void player_turn_right();
  void player_go_forward();

  BoundingBox get_body_bb() const;
  float get_rotation() const;
  Vector3 get_body_pos() const;

 private:
  Vector3 body_pos;
  Vector3 target_body_pos;

  Vector3 leg_nw, leg_ne, leg_sw, leg_se;
  Vector3 target_leg_nw, target_leg_ne, target_leg_sw, target_leg_se;
  // ???? ?000 - initialized
  // ???? ?001 - grounded
  // ???? ?010 - lifting
  // ???? ?011 - lifted, horizontally moving to next pos
  // ???? ?100 - lowering to next pos
  // ???0 0??? - is nw
  // ???0 1??? - is ne
  // ???1 0??? - is sw
  // ???1 1??? - is se
  unsigned int nw_flags, ne_flags, sw_flags, se_flags;
  // ???? ??00 - body stopped
  // ???? ??01 - rotating to move
  // ???? ??10 - moving
  // ???? ?1?? - auto roaming
  // ???? 1??? - player controlled
  // ??00 ???? - player controlled: idle
  // ??01 ???? - player controlled: turn left
  // ??10 ???? - player controlled: turn right
  // ??11 ???? - player controlled: go forward
  unsigned int flags;

  const float body_height;
  const float body_feet_radius;
  const float feet_radius;
  float lift_start_y;
  float rotation;
  float target_rotation;
  float body_idle_move_timer;
  float roaming_time;
  float roaming_timer;
};

template <std::size_t BBCount>
void Walker::update(float dt, const std::array<BoundingBox, BBCount> &bbs,
                    unsigned int width, unsigned int height) {
  if ((flags & 8) == 0 && (flags & 4) != 0 && (flags & 3) == 0) {
    roaming_timer += dt;
    if (roaming_timer > roaming_time) {
      roaming_timer = 0.0F;
      roaming_time =
          call_js_get_random() * ROAMING_WAIT_VARIANCE + ROAMING_WAIT_AMOUNT;
      unsigned int idx = call_js_get_random() * (float)bbs.size();
      float x = (float)(idx % width) - SURFACE_X_OFFSET;
      float y = (bbs[idx].min.y + bbs[idx].max.y) / 2.0F;
      float z = (float)(idx / width) - SURFACE_Y_OFFSET;

      set_body_pos(Vector3{x, y, z});
    }
  }

  const auto initialized_setup_fn = [&bbs](Vector3 &leg, Vector3 &leg_target) {
    Ray downwards{.position = leg, .direction = Vector3{0.0F, -1.0F, 0.0F}};
    for (const auto &bb : bbs) {
      if (GetRayCollisionBox(downwards, bb).hit) {
        leg_target.y = (bb.min.y + bb.max.y) / 2.0F;
        break;
      }
    }
  };

  if ((nw_flags & 7) == 0) {
    initialized_setup_fn(leg_nw, target_leg_nw);
    nw_flags |= 1;
  }
  if ((ne_flags & 7) == 0) {
    initialized_setup_fn(leg_ne, target_leg_ne);
    ne_flags |= 1;
  }
  if ((sw_flags & 7) == 0) {
    initialized_setup_fn(leg_sw, target_leg_sw);
    sw_flags |= 1;
  }
  if ((se_flags & 7) == 0) {
    initialized_setup_fn(leg_se, target_leg_se);
    se_flags |= 1;
  }

  // body rotation
  if ((flags & 8) == 0) {
    if ((flags & 3) == 1) {
      float diff = target_rotation - rotation;
      if (diff > PI) {
        rotation -= dt * BODY_ROTATION_SPEED;
        if (rotation < 0.0F) {
          rotation += PI * 2.0F;
        }
      } else if (diff < -PI) {
        rotation += dt * BODY_ROTATION_SPEED;
        if (rotation > PI * 2.0F) {
          rotation -= PI * 2.0F;
        }
      } else if (diff > 0.0F) {
        rotation += dt * BODY_ROTATION_SPEED;
        if (rotation > PI * 2.0F) {
          rotation -= PI * 2.0F;
        }
      } else {
        rotation -= dt * BODY_ROTATION_SPEED;
        if (rotation < 0.0F) {
          rotation += PI * 2.0F;
        }
      }

      if (std::abs(target_rotation - rotation) < dt * BODY_ROTATION_SPEED) {
        rotation = target_rotation;
        flags &= ~3;
        flags |= 2;
      }
    }
  } else {
    if ((flags & 0x30) == 0x10) {
      rotation += dt * BODY_ROTATION_SPEED;
    } else if ((flags & 0x30) == 0x20) {
      rotation -= dt * BODY_ROTATION_SPEED;
    }
  }

  const Matrix rotationMatrix = get_rotation_matrix_about_y(rotation);

  // body to target pos
  if ((flags & 8) == 0) {
    if ((flags & 3) == 2) {
      float diff = Vector3Distance(target_body_pos, body_pos);
      body_pos = body_pos + Vector3Normalize(target_body_pos - body_pos) *
                                (dt * BODY_TARGET_SPEED);
      if (Vector3Distance(target_body_pos, body_pos) > diff) {
        flags &= ~3;
        body_pos = target_body_pos;
      }
    }
  } else if ((flags & 0x30) == 0x30) {
    Vector3 dir = rotationMatrix * Vector3{1.0F, 0.0F, 0.0F};
    Vector3 prev_body_pos = body_pos;
    body_pos = body_pos + dir * (dt * BODY_TARGET_SPEED);
    if (body_pos.x < SURFACE_X_OFFSET - (float)SURFACE_UNIT_WIDTH + 0.5F ||
        body_pos.x > SURFACE_X_OFFSET + 0.5F ||
        body_pos.z < SURFACE_Y_OFFSET - (float)SURFACE_UNIT_HEIGHT + 0.5F ||
        body_pos.z > SURFACE_Y_OFFSET + 0.5F) {
      body_pos = prev_body_pos;
    }
    target_body_pos = body_pos + dir * 1.0F;

    // Ensure body is at proper height above surface.
    float target_height = body_pos.y;
    Ray downwards{.position = body_pos,
                  .direction = Vector3{0.0F, -1.0F, 0.0F}};
    for (auto &bb : bbs) {
      if (GetRayCollisionBox(downwards, bb).hit) {
        target_height = (bb.min.y + bb.max.y) / 2.0F + body_height;
      }
    }
    body_pos.y += (target_height - body_pos.y) * (dt * BODY_TARGET_SPEED);
  }

  // moving legs
  const auto update_leg_fn = [this, &bbs, dt, &rotationMatrix](
                                 Vector3 &leg_target, Vector3 &leg_pos,
                                 unsigned int &flags,
                                 unsigned int grounded_count) {
    if ((flags & 7) == 1 && grounded_count > 1) {
      // Grounded.
      bool should_lift = false;
      Vector3 body_pos_same_y = this->body_pos;
      body_pos_same_y.y = leg_target.y;
      Vector3 ideal_foot_pos;
      if ((flags & 0x18) == 0) {
        // Is nw.
        ideal_foot_pos =
            rotationMatrix * Vector3Normalize(Vector3{-1.0F, 0.0F, -1.0F});
      } else if ((flags & 0x18) == 0x8) {
        // Is ne.
        ideal_foot_pos =
            rotationMatrix * Vector3Normalize(Vector3{1.0F, 0.0F, -1.0F});
      } else if ((flags & 0x18) == 0x10) {
        // Is sw.
        ideal_foot_pos =
            rotationMatrix * Vector3Normalize(Vector3{-1.0F, 0.0F, 1.0F});
      } else if ((flags & 0x18) == 0x18) {
        // Is se.
        ideal_foot_pos =
            rotationMatrix * Vector3Normalize(Vector3{1.0F, 0.0F, 1.0F});
      }
      ideal_foot_pos =
          body_pos_same_y + (ideal_foot_pos * this->body_feet_radius);
      // Check if body is past threshold.
      if (Vector3Distance(ideal_foot_pos, leg_target) >
          FEET_RADIUS_PLACEMENT_CHECK_SCALE * this->feet_radius) {
        should_lift = true;
        Vector3 diff = this->target_body_pos - this->body_pos;
        if (Vector3Length(diff) > 0.1F) {
          Vector3 dir = Vector3Normalize(diff);
          leg_target =
              ideal_foot_pos +
              (dir * (this->feet_radius * FEET_RADIUS_PLACEMENT_SCALE));
        } else {
          Vector3 dir = Vector3Normalize(ideal_foot_pos - leg_target);
          leg_target =
              ideal_foot_pos +
              (dir * (this->feet_radius * FEET_RADIUS_PLACEMENT_SCALE));
        }
        // Get average .y of ground at target position.
        Ray downwards{.position = Vector3{leg_target.x, leg_target.y + 5.0F,
                                          leg_target.z},
                      .direction = Vector3{0.0F, -1.0F, 0.0F}};
        for (const auto &bb : bbs) {
          if (GetRayCollisionBox(downwards, bb).hit) {
            leg_target.y = (bb.min.y + bb.max.y) / 2.0F;
            break;
          }
        }
      }
      if (should_lift) {
        this->lift_start_y = leg_pos.y;
        flags = (flags & ~7) | 2;
      }
    }
    if ((flags & 7) == 2) {
      // Lifting.
      leg_pos.y += dt * FEET_LIFT_SPEED;
      if (leg_pos.y > this->lift_start_y + FEET_LIFT_HEIGHT) {
        leg_pos.y = this->lift_start_y + FEET_LIFT_HEIGHT;
        flags = (flags & ~7) | 3;
      }
    }
    if ((flags & 7) == 3) {
      // Moving horizontally.
      float prev_dist = Vector3Distance(
          leg_pos, Vector3{leg_target.x, leg_pos.y, leg_target.z});
      Vector3 dir = Vector3Normalize(
          Vector3{leg_target.x, leg_pos.y, leg_target.z} - leg_pos);
      leg_pos = leg_pos + (dir * (dt * FEET_HORIZ_MOVE_SPEED));
      if (Vector3Distance(leg_pos, Vector3{leg_target.x, leg_pos.y,
                                           leg_target.z}) >= prev_dist) {
        leg_pos.x = leg_target.x;
        leg_pos.z = leg_target.z;
        flags = (flags & ~7) | 4;
      }
    }
    if ((flags & 7) == 4) {
      // Lowering leg.
      leg_pos.y -= dt * FEET_LIFT_SPEED;
      if (leg_pos.y < leg_target.y) {
        leg_pos.y = leg_target.y;
        flags = (flags & ~7) | 1;
      }
    }
  };

  update_leg_fn(target_leg_nw, leg_nw, nw_flags,
                ((ne_flags & 7) == 1 ? 1 : 0) + ((sw_flags & 7) == 1 ? 1 : 0) +
                    ((se_flags & 7) == 1 ? 1 : 0));
  update_leg_fn(target_leg_se, leg_se, se_flags,
                ((nw_flags & 7) == 1 ? 1 : 0) + ((ne_flags & 7) == 1 ? 1 : 0) +
                    ((sw_flags & 7) == 1 ? 1 : 0));
  update_leg_fn(target_leg_ne, leg_ne, ne_flags,
                ((nw_flags & 7) == 1 ? 1 : 0) + ((sw_flags & 7) == 1 ? 1 : 0) +
                    ((se_flags & 7) == 1 ? 1 : 0));
  update_leg_fn(target_leg_sw, leg_sw, sw_flags,
                ((nw_flags & 7) == 1 ? 1 : 0) + ((ne_flags & 7) == 1 ? 1 : 0) +
                    ((se_flags & 7) == 1 ? 1 : 0));

  if ((flags & 8) == 0) {
    if ((flags & 3) == 0) {
      body_idle_move_timer += dt * BODY_IDLE_TIMER_RATE;
      if (body_idle_move_timer > PI * 2.0F) {
        body_idle_move_timer -= PI * 2.0F;
      }
    } else if (!FloatEquals(body_idle_move_timer, 0.0F)) {
      if (body_idle_move_timer < PI) {
        body_idle_move_timer += dt * BODY_IDLE_TIMER_RATE;
        if (body_idle_move_timer > PI) {
          body_idle_move_timer = 0;
        }
      } else {
        body_idle_move_timer += dt * BODY_IDLE_TIMER_RATE;
        if (body_idle_move_timer > PI * 2.0F) {
          body_idle_move_timer = 0.0F;
        }
      }
    }
  } else if (!FloatEquals(body_idle_move_timer, 0.0F)) {
    if (body_idle_move_timer < PI) {
      body_idle_move_timer += dt * BODY_IDLE_TIMER_RATE;
      if (body_idle_move_timer > PI) {
        body_idle_move_timer = 0;
      }
    } else {
      body_idle_move_timer += dt * BODY_IDLE_TIMER_RATE;
      if (body_idle_move_timer > PI * 2.0F) {
        body_idle_move_timer = 0.0F;
      }
    }
  }
}

#endif
