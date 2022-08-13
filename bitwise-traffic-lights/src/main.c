#include <stdbool.h>
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

    while (true)
    {
        setLights();

        VIS_visualize();

        sleep(SIM_step());
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
