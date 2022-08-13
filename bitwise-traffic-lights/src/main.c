#include <stdlib.h>
#include <unistd.h>

#include "Visualization.h"
#include "Simulator.h"

void setLights(void);

int main()
{
    if (SIM_initialize() != SIMULATOR_OK)
    {
        return EXIT_FAILURE;
    }

    if (VIS_initialize() != VISUALIZATION_OK)
    {
        return EXIT_FAILURE;
    }

    unsigned int timeout = 1;
    while (timeout > 0)
    {
        setLights();

        VIS_visualize();

        timeout = SIM_step();
        sleep(timeout);
    }
}

void setLights(void)
{
    for (int light = 0; light < LIGHTS_COUNT; light++)
    {
        for (int color = 0; color < COLORS_COUNT; color++)
        {
            const bool on = SIM_getState(light, color);

            VIS_setLight(light, color, on);
        }
    }
}
