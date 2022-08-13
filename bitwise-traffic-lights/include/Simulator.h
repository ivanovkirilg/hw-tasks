#ifndef INCLUDE_SIMULATOR
#define INCLUDE_SIMULATOR

#include <stdbool.h>

#include "Lights.h"

#define SIMULATOR_OK     0
#define SIMULATOR_ERROR -1


/* * * * * * * * * * * * * * * * * * * * *
 * Binary Representation of a Cycle Step *
 * * * * * * * * * * * * * * * * * * * * *
 *
 * As a u_int_16 (unsigned short):
 * 00000000 00000000
 * TTTT   G YR   GYR
 *     GYR    GYR
 *
 * Or:
 * 0000 000 000 000 000
 * TTTT GYR GYR GYR GYR
 *      Bot Lft Rgt Top
 * 
 * Where `TTTT' is the unsigned integral length in seconds,
 * then `GYR' is each light's state, in the order:
 * Bottom-Left-Right-Top (as read from left to right)
 *
 * A one-second step of all reds shall look like this:
 * 0001 001 001 001 001 == 00010010 01001001 == 011111
 *
 *
 * The input is expected in BIG ENDIAN.
 * The left (most significant) byte (the one with the length)
 * is read first, then the right (least significant) byte.
 */


/**
 * @brief Reads the cycle definition.
 *
 * @pre stdin is NOT an interactive terminal.
 *
 * @return SIMULATOR_OK or SIMULATOR_ERROR
 */
int SIM_initialize(void);

/**
 * @brief Gets the on/off state of a particular `color' bulb and `light'.
 *
 * @pre The Simulator module has been initialized.
 *
 * @return TRUE if the bulb is on, FALSE otherwise.
 */
bool SIM_getState(enum LightPosition light, enum LightColor color);

/**
 * @brief Advances the Simulator to the next step in the cycle.
 *
 * @pre The Simulator module has been initialized.
 *
 * @return The length in seconds of the current (before advancing) step.
 */
unsigned int SIM_step(void);

#endif // INCLUDE_SIMULATOR
