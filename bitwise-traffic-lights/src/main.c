#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "Visualization.h"
#include "Simulator.h"

void initialize(void);
void setLights(void);

int main()
{
    initialize();

    while (true)
    {
        setLights();

        VIS_visualize();

        sleep(SIM_step());
    }
}

void initialize()
{
    if (SIM_initialize() != SIMULATOR_OK)
    {
        exit(EXIT_FAILURE);
    }

    if (VIS_initialize() != VISUALIZATION_OK)
    {
        exit(EXIT_FAILURE);
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
