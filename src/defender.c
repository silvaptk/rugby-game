// Standard headers
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Internal headers
#include "direction.h"
#include "position.h"
#include "spy.h"

// Main header
#include "defender.h"

// Macros
#define UNUSED(x) (void)(x) // Auxiliary to avoid error of unused parameter

/*----------------------------------------------------------------------------*/
/*                          PRIVATE FUNCTIONS HEADERS                         */
/*----------------------------------------------------------------------------*/

static bool flip_a_coin (float chance);
static void assign_direction (direction_t *first, direction_t second);

/*----------------------------------------------------------------------------*/
/*                              PUBLIC FUNCTIONS                              */
/*----------------------------------------------------------------------------*/

direction_t execute_defender_strategy(
  position_t defender_position, 
  Spy attacker_spy
) {
  static size_t rounds = 1;
  static size_t field_size = 0; 
  static int upper_bound;
  static int lower_bound;
  static bool reached_attacker_latitude = false;
  static size_t attacker_spied_latitude;
  direction_t direction = DIR_STAY;

  if (rounds == 1) {
    field_size = defender_position.j;
  }

  if (rounds == floor((double) field_size / 2)) {
    position_t attacker_position = get_spy_position(attacker_spy);
    attacker_spied_latitude = attacker_position.i;

    upper_bound = (int) attacker_position.i + (int) field_size / 4;
    lower_bound = (int) attacker_position.i - (int) field_size / 4;

    if ((size_t) upper_bound > field_size) {
      lower_bound = lower_bound - (upper_bound - field_size);
      upper_bound = field_size;
    }

    if (lower_bound < 0) {
      upper_bound = upper_bound - lower_bound;
      lower_bound = 0;
    }
  }

  if (rounds >= floor((double) field_size / 2)) {

    if (!reached_attacker_latitude) {
      reached_attacker_latitude = attacker_spied_latitude == defender_position.j;

      if (attacker_spied_latitude > defender_position.i) {
        assign_direction(&direction, (direction_t) DIR_DOWN);
      } else {
        assign_direction(&direction, (direction_t) DIR_UP);
      }
    }

    if (reached_attacker_latitude) {
      if (flip_a_coin(0.5)) {
        if (defender_position.j == (size_t) upper_bound) {
          assign_direction(&direction, (direction_t) DIR_DOWN);
        } else if (defender_position.j == (size_t) lower_bound) {
          assign_direction(&direction, (direction_t) DIR_UP);
        } else {
          if (flip_a_coin(0.5)) {
            assign_direction(&direction, (direction_t) DIR_UP);
          } else {
            assign_direction(&direction, (direction_t) DIR_DOWN);
          }
        }
      } 
    } 
  }

  rounds++;

  return direction;
}

/*----------------------------------------------------------------------------*/
/*                              PRIVATE FUNCTIONS                             */
/*----------------------------------------------------------------------------*/

static bool flip_a_coin (float balance) {
  return (double) rand() / (double) RAND_MAX > balance;
}

static void assign_direction (direction_t *first, direction_t second) {
  (*first).i = second.i;
  (*first).j = second.j;
}
