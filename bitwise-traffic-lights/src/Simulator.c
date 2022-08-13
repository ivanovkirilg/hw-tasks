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
#define MAX_CYCLE_LEN   20

#define BITS_PER_BYTE   8
#define BITS_PER_LIGHT  3

#define TIMEOUT_OFFSET  (sizeof(step_t) * BITS_PER_BYTE - 4) // Last 4 bits
#define TIMEOUT_MASK    (0b1111 << TIMEOUT_OFFSET)

#define PLACEHOLDER_STEP ~0
#define TERMINATING_STEP 0


/* --------------------------------------------------------------------------
 *                               Declarations
 * -------------------------------------------------------------------------- */
typedef unsigned short step_t;

static int sim_read(void);
static step_t sim_deserializeStep(const size_t stepIdx);


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
    return g_cycle[g_iStep] & 1u << ((light * BITS_PER_LIGHT) + color);
}

unsigned int SIM_step(void)
{
    unsigned int timeout = g_cycle[g_iStep] >> TIMEOUT_OFFSET;

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
    for (size_t i = 0; ; i++)
    {
        if (i >= MAX_CYCLE_LEN)
        {
            fprintf(stderr, "Error: received too many cycle steps. The maximum "
                            "is %d steps, represented in %zu bytes.\n",
                            MAX_CYCLE_LEN, sizeof(step_t) * MAX_CYCLE_LEN);
            return SIMULATOR_ERROR;
        }

        g_cycle[i] = sim_deserializeStep(i);

        if (g_cycle[i] == TERMINATING_STEP)
        {
            break;
        }
    }

    if (g_cycle[0] == TERMINATING_STEP)
    {
        fprintf(stderr, "Error: simulator received an empty cycle.\n");
        return SIMULATOR_ERROR;
    }

    return SIMULATOR_OK;
}

static step_t sim_deserializeStep(const size_t stepIdx)
{
    int input[2] = {
        fgetc(stdin),
        fgetc(stdin)
    };

    if ((input[0] == EOF) || (input[1] == EOF))
    {
        fprintf(stderr, "Warning: simulator input ended at cycle step %zu "
                        "before it was properly terminated. Continuing.\n",
                        stepIdx);
        return TERMINATING_STEP;
    }

    const step_t step = (input[0] << 8u) | input[1];

    if (   (step != TERMINATING_STEP)
        && ((step & TIMEOUT_MASK) == 0) )
    {
        fprintf(stderr, "Warning: received non-terminating step with zero "
                        "timeout at position %zu. Continuing.\n",
                        stepIdx);
        return TERMINATING_STEP;
    }

    return step;
}
