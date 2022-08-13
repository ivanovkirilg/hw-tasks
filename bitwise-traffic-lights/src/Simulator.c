/* --------------------------------------------------------------------------
 *                               Includes
 * -------------------------------------------------------------------------- */
#include "Simulator.h"

#include <stdio.h>
#include <unistd.h>

#include "Lights.h"


/* --------------------------------------------------------------------------
 *                               Defines
 * -------------------------------------------------------------------------- */
#define MAX_CYCLE_LEN 20


/* --------------------------------------------------------------------------
 *                               Declarations
 * -------------------------------------------------------------------------- */
typedef unsigned short step_t;

static int sim_read(void);


/* --------------------------------------------------------------------------
 *                               Definitions
 * -------------------------------------------------------------------------- */
static size_t g_iStep = 0;
static step_t g_cycle[MAX_CYCLE_LEN] = {0};


extern int SIM_initialize()
{
    if (isatty(STDIN_FILENO))
    {
        fprintf(stderr, "Error: the simulator expects to read binary data on "
                        "stdin (either from a pipe or file), but it seems to "
                        "be a terminal.\n");
        return SIMULATOR_ERROR;
    }

    return sim_read();
}

bool SIM_getState(enum LightPosition light, enum LightColor color)
{
    return g_cycle[g_iStep] & 1u << ((light * 3) + color);
}

unsigned int SIM_step(void)
{
    unsigned int timeout = g_cycle[g_iStep] >> (16 - 4);

    g_iStep++;
    if (g_cycle[g_iStep] == 0)
    {
        g_iStep = 0;
    }

    return timeout;
}

/* --------------------------------------------------------------------------
 *                               Local Functions
 * -------------------------------------------------------------------------- */
static int sim_read()
{
    int input[2] = {0};
    step_t cycleStep = 1;

    for (size_t i = 0; cycleStep != 0; i++)
    {
        if (i >= MAX_CYCLE_LEN)
        {
            fprintf(stderr, "Error: received too many cycle steps. The maximum "
                            "is %d steps, represented in %zu bytes.\n",
                            MAX_CYCLE_LEN, sizeof(step_t) * MAX_CYCLE_LEN);
            return SIMULATOR_ERROR;
        }

        input[0] = fgetc(stdin);
        input[1] = fgetc(stdin);
        if ((input[0] == EOF) || (input[1] == EOF))
        {
            fprintf(stderr, "Warning: simulator input ended at cycle step %zu "
                            "before it was properly terminated. Continuing.\n",
                            i);
            g_cycle[i] = 0;
            return SIMULATOR_OK;
        }

        cycleStep = (input[0] << 8u) | input[1];
        g_cycle[i] = cycleStep;
    }

    if (g_cycle[0] == 0)
    {
        fprintf(stderr, "Error: simulator received an empty cycle.\n");
        return SIMULATOR_ERROR;
    }

    return SIMULATOR_OK;
}
