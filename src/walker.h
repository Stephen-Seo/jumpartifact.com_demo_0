#ifndef JUMPARTIFACT_DOT_COM_DEMO_0_WALKER_H_
#define JUMPARTIFACT_DOT_COM_DEMO_0_WALKER_H_

// standard library includes
#include <array>

// third party includes
#include <raylib.h>
#include <raymath.h>

constexpr float FEET_RADIUS_PLACEMENT_CHECK_SCALE = 1.0F;
constexpr float FEET_RADIUS_PLACEMENT_SCALE = 0.9F;
constexpr float FEET_TARGET_RATE = 1.0F;
constexpr float BODY_TARGET_SPEED = 1.5F;
constexpr float FEET_LIFT_HEIGHT = 1.0F;
constexpr float FEET_LIFT_SPEED = 5.0F;
constexpr float FEET_HORIZ_MOVE_SPEED = 7.0F;
constexpr float FEET_INIT_POS_VARIANCE_DIV = 4.0F;

class Walker {
 public:
  Walker(float body_height = 2.0F, float body_feet_radius = 1.7F,
         float feet_radius = 1.5F);

  template <typename TBBS>
  void update(float dt, const TBBS &bbs, unsigned int width,
              unsigned int height);

  void draw(const Model &model);

  void set_body_pos(Vector3 pos);

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
  // ???? ???1 - body stopped
  unsigned int flags;

  const float body_height;
  const float body_feet_radius;
  const float feet_radius;
  float lift_start_y;
};

template <typename TBBS>
void Walker::update(float dt, const TBBS &bbs, unsigned int width,
                    unsigned int height) {
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

  // body to target pos
  if ((flags & 1) == 0) {
    float diff = Vector3Distance(target_body_pos, body_pos);
    body_pos =
        Vector3Add(body_pos, Vector3Scale(Vector3Normalize(Vector3Subtract(
                                              target_body_pos, body_pos)),
                                          dt * BODY_TARGET_SPEED));
    if (Vector3Distance(target_body_pos, body_pos) > diff) {
      flags |= 1;
      body_pos = target_body_pos;
    }
  }

  // moving legs
  const auto update_leg_fn = [this, &bbs, dt](Vector3 &leg_target,
                                              Vector3 &leg_pos,
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
        ideal_foot_pos = Vector3Normalize(Vector3{-1.0F, 0.0F, -1.0F});
      } else if ((flags & 0x18) == 0x8) {
        // Is ne.
        ideal_foot_pos = Vector3Normalize(Vector3{1.0F, 0.0F, -1.0F});
      } else if ((flags & 0x18) == 0x10) {
        // Is sw.
        ideal_foot_pos = Vector3Normalize(Vector3{-1.0F, 0.0F, 1.0F});
      } else if ((flags & 0x18) == 0x18) {
        // Is se.
        ideal_foot_pos = Vector3Normalize(Vector3{1.0F, 0.0F, 1.0F});
      }
      ideal_foot_pos =
          Vector3Add(body_pos_same_y,
                     Vector3Scale(ideal_foot_pos, this->body_feet_radius));
      // Check if body is past threshold.
      if (Vector3Distance(ideal_foot_pos, leg_target) >
          FEET_RADIUS_PLACEMENT_CHECK_SCALE * this->feet_radius) {
        should_lift = true;
        Vector3 diff = Vector3Subtract(this->target_body_pos, this->body_pos);
        if (Vector3Length(diff) > 0.1F) {
          Vector3 dir = Vector3Normalize(diff);
          leg_target =
              Vector3Add(ideal_foot_pos,
                         Vector3Scale(dir, this->feet_radius *
                                               FEET_RADIUS_PLACEMENT_SCALE));
        } else {
          Vector3 dir =
              Vector3Normalize(Vector3Subtract(ideal_foot_pos, leg_target));
          leg_target =
              Vector3Add(ideal_foot_pos,
                         Vector3Scale(dir, this->feet_radius *
                                               FEET_RADIUS_PLACEMENT_SCALE));
        }
        // Get average .y of ground at target position.
        Ray downwards{.position = Vector3{leg_target.x, leg_target.y + 5.0F,
                                          leg_target.z},
                      .direction = Vector3{0.0F, -1.0F, 0.0F}};
        for (const auto &bb : bbs) {
          if (GetRayCollisionBox(downwards, bb).hit) {
            leg_target.y = (bb.min.y + bb.max.y) / 2.0F;
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
      Vector3 dir = Vector3Normalize(Vector3Subtract(
          Vector3{leg_target.x, leg_pos.y, leg_target.z}, leg_pos));
      leg_pos =
          Vector3Add(leg_pos, Vector3Scale(dir, dt * FEET_HORIZ_MOVE_SPEED));
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
  update_leg_fn(target_leg_sw, leg_sw, sw_flags,
                ((nw_flags & 7) == 1 ? 1 : 0) + ((ne_flags & 7) == 1 ? 1 : 0) +
                    ((se_flags & 7) == 1 ? 1 : 0));
  update_leg_fn(target_leg_ne, leg_ne, ne_flags,
                ((nw_flags & 7) == 1 ? 1 : 0) + ((sw_flags & 7) == 1 ? 1 : 0) +
                    ((se_flags & 7) == 1 ? 1 : 0));
  update_leg_fn(target_leg_se, leg_se, se_flags,
                ((nw_flags & 7) == 1 ? 1 : 0) + ((ne_flags & 7) == 1 ? 1 : 0) +
                    ((sw_flags & 7) == 1 ? 1 : 0));

  // legs to target pos
  //  leg_nw =
  //      Vector3Add(leg_nw, Vector3Scale(Vector3Subtract(target_leg_nw,
  //      leg_nw),
  //                                      dt * FEET_TARGET_RATE));
  //  leg_ne =
  //      Vector3Add(leg_ne, Vector3Scale(Vector3Subtract(target_leg_ne,
  //      leg_ne),
  //                                      dt * FEET_TARGET_RATE));
  //  leg_sw =
  //      Vector3Add(leg_sw, Vector3Scale(Vector3Subtract(target_leg_sw,
  //      leg_sw),
  //                                      dt * FEET_TARGET_RATE));
  //  leg_se =
  //      Vector3Add(leg_se, Vector3Scale(Vector3Subtract(target_leg_se,
  //      leg_se),
  //                                      dt * FEET_TARGET_RATE));
}

#endif
