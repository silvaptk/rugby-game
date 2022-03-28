// Standard headers
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// Internal headers
#include "direction.h"
#include "position.h"
#include "spy.h"

// Main header
#include "attacker.h"

// Macros
#define UNUSED(x) (void)(x) // Auxiliary to avoid error of unused parameter
#define UP_INDEX 0
#define UP_RIGHT_INDEX 1
#define RIGHT_INDEX 2
#define DOWN_RIGHT_INDEX 3
#define DOWN_INDEX 4
#define DOWN_LEFT_INDEX 5
#define LEFT_INDEX 6
#define UP_LEFT_INDEX 7

/*----------------------------------------------------------------------------*/
/*                          PRIVATE FUNCTIONS HEADERS                         */
/*----------------------------------------------------------------------------*/

static bool flip_a_coin (float chance);
static int get_direction_index (direction_t direction);
static bool positions_are_equal (position_t first, position_t second);
static bool directions_are_equal (direction_t first, direction_t second);
static void assign_direction (direction_t *first, direction_t second);
static bool should_redirect (int steps);
static direction_t get_constrained_direction (bool *can_go, direction_t direction);
static void update_constraints (bool *can_go, direction_t direction);

/*----------------------------------------------------------------------------*/
/*                              PUBLIC FUNCTIONS                              */
/*----------------------------------------------------------------------------*/

direction_t execute_attacker_strategy (
  position_t attacker_position, 
  Spy defender_spy
) {
  static direction_t direction = DIR_STAY;
  static int steps = 0;
  static bool already_redirected = false;
  static position_t last_position = INVALID_POSITION;
  static bool *can_go = NULL;

  UNUSED(defender_spy);

  if (!can_go) {
    can_go = malloc(8 * sizeof(bool));

    for (int index = 0; index < 8; index++) {
      can_go[index] = true;
    }

    srand(time(0));
  }

  if (directions_are_equal(direction, (direction_t) DIR_STAY)) {
    if (flip_a_coin(0.5)) {
      assign_direction(&direction, (direction_t) DIR_UP_RIGHT);
    } else {
      assign_direction(&direction, (direction_t) DIR_DOWN_RIGHT);
    }
  }

  if (
    directions_are_equal(direction, (direction_t) DIR_UP_RIGHT) || 
    directions_are_equal(direction, (direction_t) DIR_DOWN_RIGHT)
  ) {
    if (!already_redirected && should_redirect(steps)) {
      already_redirected = true;
      
      if (directions_are_equal(direction, (direction_t) DIR_UP_RIGHT)) {
        assign_direction(&direction, (direction_t) DIR_DOWN_RIGHT);
      } else {
        assign_direction(&direction, (direction_t) DIR_UP_RIGHT);
      }
    }
  }

  if (positions_are_equal(last_position, attacker_position)) {
    assign_direction(
      &direction,
      get_constrained_direction(can_go, direction)
    );
  } else {
    update_constraints(can_go, direction);
    steps++;
  }

  last_position = attacker_position;

  return direction;
}

/*----------------------------------------------------------------------------*/
/*                              PRIVATE FUNCTIONS                             */
/*----------------------------------------------------------------------------*/

static bool flip_a_coin (float balance) {
  return (double) rand() / (double) RAND_MAX > balance;
}

static int get_direction_index (direction_t direction) {
  if (directions_are_equal(direction, (direction_t) DIR_UP)) {
    return UP_INDEX;
  } else if (directions_are_equal(direction, (direction_t) DIR_UP_RIGHT)) {
    return UP_RIGHT_INDEX;
  } else if (directions_are_equal(direction, (direction_t) DIR_RIGHT)) {
    return RIGHT_INDEX;
  } else if (directions_are_equal(direction, (direction_t) DIR_DOWN_RIGHT)) {
    return DOWN_RIGHT_INDEX;
  } else if (directions_are_equal(direction, (direction_t) DIR_DOWN)) {
    return DOWN_INDEX;
  } else if (directions_are_equal(direction, (direction_t) DIR_DOWN_LEFT)) {
    return DOWN_LEFT_INDEX;
  } else if (directions_are_equal(direction, (direction_t) DIR_LEFT)) {
    return LEFT_INDEX;
  } else {
    return UP_LEFT_INDEX;
  }
}

static bool positions_are_equal (position_t first, position_t second) {
  return first.i == second.i && first.j == second.j;
}

static bool directions_are_equal (direction_t first, direction_t second) {
  return first.i == second.i && first.j == second.j;
}

static void assign_direction (direction_t *first, direction_t second) {
  (*first).i = second.i;
  (*first).j = second.j;
}

static bool should_redirect (int steps) {
  return flip_a_coin(0.5 - steps * 0.05);
} 

static direction_t get_constrained_direction (bool *can_go, direction_t direction) {
  direction_t new_direction;

  switch (get_direction_index(direction)) {
    case UP_INDEX:
      can_go[UP_INDEX] = false;

      if (can_go[UP_RIGHT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_UP_RIGHT);
      } else if (can_go[UP_LEFT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_UP_LEFT);
      } else if (can_go[RIGHT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_RIGHT);
      } else if (can_go[LEFT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_LEFT);
      } else if (can_go[DOWN_RIGHT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_DOWN_RIGHT);
      } else if (can_go[DOWN_LEFT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_DOWN_LEFT);
      } else {
        assign_direction(&new_direction, (direction_t) DIR_DOWN);
      }
      
      break;
    case UP_RIGHT_INDEX:
      can_go[UP_RIGHT_INDEX] = false;

      if (can_go[RIGHT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_RIGHT);
      } else if (can_go[UP_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_UP);
      } else if (can_go[DOWN_RIGHT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_DOWN_RIGHT);
      } else if (can_go[UP_LEFT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_UP_LEFT);
      } else if (can_go[DOWN_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_DOWN);
      } else if (can_go[LEFT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_LEFT);
      } else {
        assign_direction(&new_direction, (direction_t) DIR_DOWN_LEFT);
      }

      break;
    case RIGHT_INDEX:
      can_go[RIGHT_INDEX] = false;

      if (can_go[DOWN_RIGHT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_DOWN_RIGHT);
      } else if (can_go[UP_RIGHT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_UP_RIGHT);
      } else if (can_go[DOWN_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_DOWN);
      } else if (can_go[UP_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_UP);
      } else if (can_go[DOWN_LEFT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_DOWN_LEFT);
      } else if (can_go[UP_LEFT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_UP_LEFT);
      } else {
        assign_direction(&new_direction, (direction_t) DIR_LEFT);
      }

      break;
    case DOWN_RIGHT_INDEX:
      can_go[DOWN_RIGHT_INDEX] = false;

      if (can_go[RIGHT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_RIGHT);
      } else if (can_go[DOWN_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_DOWN);
      } else if (can_go[UP_RIGHT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_UP_RIGHT);
      } else if (can_go[DOWN_LEFT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_DOWN_LEFT);
      } else if (can_go[UP_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_UP);
      } else if (can_go[LEFT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_LEFT);
      } else {
        assign_direction(&new_direction, (direction_t) DIR_UP_LEFT);
      }

      break;
    case DOWN_INDEX:
      can_go[DOWN_INDEX] = false;

      if (can_go[DOWN_RIGHT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_DOWN_RIGHT);
      } else if (can_go[DOWN_LEFT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_DOWN_LEFT);
      } else if (can_go[RIGHT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_RIGHT);
      } else if (can_go[LEFT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_LEFT);
      } else if (can_go[UP_RIGHT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_UP_RIGHT);
      } else if (can_go[UP_LEFT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_UP_LEFT);
      } else {
        assign_direction(&new_direction, (direction_t) DIR_UP);
      }

      break;
    case DOWN_LEFT_INDEX:
      can_go[DOWN_LEFT_INDEX] = false;

      if (can_go[DOWN_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_DOWN);
      } else if (can_go[LEFT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_LEFT);
      } else if (can_go[DOWN_RIGHT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_DOWN_RIGHT);
      } else if (can_go[UP_LEFT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_UP_LEFT);
      } else if (can_go[RIGHT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_RIGHT);
      } else if (can_go[UP_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_UP);
      } else {
        assign_direction(&new_direction, (direction_t) DIR_UP_RIGHT);
      }

      break;
    case LEFT_INDEX:
      can_go[LEFT_INDEX] = false;

      if (can_go[UP_LEFT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_UP_LEFT);
      } else if (can_go[DOWN_LEFT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_DOWN_LEFT);
      } else if (can_go[UP_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_UP);
      } else if (can_go[DOWN_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_DOWN);
      } else if (can_go[UP_RIGHT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_UP_RIGHT);
      } else if (can_go[DOWN_RIGHT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_DOWN_RIGHT);
      } else {
        assign_direction(&new_direction, (direction_t) DIR_RIGHT);
      }

      break;
    case UP_LEFT_INDEX:
      can_go[UP_LEFT_INDEX] = false;

      if (can_go[UP_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_UP);
      } else if (can_go[LEFT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_LEFT);
      } else if (can_go[UP_RIGHT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_UP_RIGHT);
      } else if (can_go[DOWN_LEFT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_DOWN_LEFT);
      } else if (can_go[RIGHT_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_RIGHT);
      } else if (can_go[DOWN_INDEX]) {
        assign_direction(&new_direction, (direction_t) DIR_DOWN);
      } else {
        assign_direction(&new_direction, (direction_t) DIR_DOWN_RIGHT);
      }

      break;
  }

  return new_direction;
}

static void update_constraints (bool *can_go, direction_t direction) {
  switch (get_direction_index(direction)) {
    case UP_INDEX:
      can_go[UP_LEFT_INDEX] = true;
      can_go[UP_INDEX] = true;
      can_go[UP_RIGHT_INDEX] = true;
      break;
    case UP_RIGHT_INDEX:
      can_go[UP_INDEX] = true;
      can_go[UP_RIGHT_INDEX] = true;
      can_go[RIGHT_INDEX] = true;
      break;
    case RIGHT_INDEX:
      can_go[UP_RIGHT_INDEX] = true;
      can_go[RIGHT_INDEX] = true;
      can_go[DOWN_RIGHT_INDEX] = true;
      break;
    case DOWN_RIGHT_INDEX:
      can_go[RIGHT_INDEX] = true;
      can_go[DOWN_RIGHT_INDEX] = true;
      can_go[DOWN_INDEX] = true;
      break;
    case DOWN_INDEX:
      can_go[DOWN_RIGHT_INDEX] = true;
      can_go[DOWN_INDEX] = true;
      can_go[DOWN_LEFT_INDEX] = true;
      break;
    case DOWN_LEFT_INDEX:
      can_go[DOWN_INDEX] = true;
      can_go[DOWN_LEFT_INDEX] = true;
      can_go[LEFT_INDEX] = true;
      break;
    case LEFT_INDEX:
      can_go[DOWN_LEFT_INDEX] = true;
      can_go[LEFT_INDEX] = true;
      can_go[UP_LEFT_INDEX] = true;
      break;
    case UP_LEFT_INDEX:
      can_go[LEFT_INDEX] = true;
      can_go[UP_LEFT_INDEX] = true;
      can_go[UP_INDEX] = true;
      break;
    default:
      break;
  }
}