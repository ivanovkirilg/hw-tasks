#ifndef INCLUDE_VISUALIZATION
#define INCLUDE_VISUALIZATION

#include <stdbool.h>

#include "Lights.h"

#define VISUALIZATION_OK     0
#define VISUALIZATION_ERROR -1

/**
 * @brief Prints the initial off-state visualization.
 *
 * @pre stdout is a terminal with sufficient size.
 *
 * @return VISUALIZATION_ERROR or VISUALIZATION_OK
 */
int VIS_initialize(void);

/**
 * @brief Re-prints the visualization in-place.
 *
 * @pre The Visualization module has been initialized.
 */
void VIS_visualize(void);

/**
 * @brief Sets only the given `color' bulb of the specified `light' to on/off.
 *
 * @pre The Visualization module has been initialized.
 *
 * @return VISUALIZATION_ERROR or VISUALIZATION_OK
 *
 * @note The other bulbs of the light are unaffected.
 */
int VIS_setLight(enum LightPosition light, enum LightColor color, bool on);


#endif // INCLUDE_VISUALIZATION
