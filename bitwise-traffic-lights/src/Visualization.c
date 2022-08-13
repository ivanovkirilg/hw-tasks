/* --------------------------------------------------------------------------
 *                               Includes
 * -------------------------------------------------------------------------- */
#include "Visualization.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h> // Beware: this header is POSIX-specific
#include <sys/ioctl.h>

#include "Lights.h"


/* --------------------------------------------------------------------------
 *                               Defines
 * -------------------------------------------------------------------------- */
#define ROWS_PER_VIS 6  // Beware: this is not technically enforced anywhere
#define VIS_ROW_SIZE 64
#define SINGLE_VIS_SIZE (ROWS_PER_VIS) * VIS_ROW_SIZE

#define TERM_RETURN  "\r"
#define TERM_ESC_CHR '\033'
#define TERM_ESC_SEQ "\033["
#define TERM_RESET   TERM_ESC_SEQ "00m" // Beware: these must be the same len
#define TERM_RED     TERM_ESC_SEQ "31m"
#define TERM_GREEN   TERM_ESC_SEQ "32m"
#define TERM_YELLOW  TERM_ESC_SEQ "33m"

#define LIGHTBULB_OFF    TERM_RESET  "o" TERM_RESET
#define LIGHTBULB_RED    TERM_RED    "@" TERM_RESET
#define LIGHTBULB_GREEN  TERM_GREEN  "@" TERM_RESET
#define LIGHTBULB_YELLOW TERM_YELLOW "@" TERM_RESET

#define LIGHTBULB_COLOR_SEQ_LEN 11


/* --------------------------------------------------------------------------
 *                               Declarations
 * -------------------------------------------------------------------------- */

/**
 * @brief An array of ROWS_PER_VIS-line strings for each light.
 *        These are updated with colors and re-printed.
 * 
 * @note Defined at the bottom of the file to avoid clutter.
 */
static char g_visualizations[LIGHTS_COUNT][SINGLE_VIS_SIZE];

static void vis_print(void);
static int  vis_getBulbPositionInLight(enum LightPosition light,
                                       enum LightColor color);
static void vis_setColor(char *light, enum LightColor color, bool on);


/* --------------------------------------------------------------------------
 *                               Definitions
 * -------------------------------------------------------------------------- */

extern int VIS_initialize(void)
{
    const int ROW_COUNT = ROWS_PER_VIS * LIGHTS_COUNT;

    struct winsize w;
    const int result = ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    // Check preconditions
    if (result < 0)
    {
        fprintf(stderr, "Error: you may have piped the visualization program's "
                        "output, but it must go to a terminal. (%s)\n",
                        strerror(errno)); // Get std library error message
        return VISUALIZATION_ERROR;
    }
    else if (w.ws_col < VIS_ROW_SIZE)
    {
        fprintf(stderr, "Error: your terminal is not wide enough to display "
                        "the visualization properly. Minimum width is %d.\n",
                        VIS_ROW_SIZE);
        return VISUALIZATION_ERROR;
    }
    else if (w.ws_row < ROW_COUNT)
    {
        fprintf(stderr, "Error: your terminal is not tall enough to display "
                        "the visualization properly. Minimum height is %d.\n",
                        ROW_COUNT);
        return VISUALIZATION_ERROR;
    }

    vis_print();

    return VISUALIZATION_OK;
}

extern void VIS_visualize(void)
{
    // Move the cursor up -num- lines with "escape[-num-A",
    // then return to the start of the line with carriage-return '\r'
    printf(TERM_ESC_SEQ "%dA" TERM_RETURN,
           ROWS_PER_VIS * LIGHTS_COUNT);

    vis_print();
}

extern int VIS_setLight(enum LightPosition light,
                        enum LightColor color, bool on)
{
    const int bulbPosition = vis_getBulbPositionInLight(light, color);

    if (bulbPosition == VISUALIZATION_ERROR)
    {
        return VISUALIZATION_ERROR;
    }

    char *bulb = g_visualizations[light];
    for (int bulbsFound = 0; bulbsFound < bulbPosition; bulbsFound++)
    {
        bulb = strchr(bulb + LIGHTBULB_COLOR_SEQ_LEN, TERM_ESC_CHR);

        if (bulb == NULL)
        {
            fprintf(stderr, "Error: the requested lightbulb was not found. "
                            "The visualizations buffer is likely malformed.\n");
            return VISUALIZATION_ERROR;
        }
    }

    vis_setColor(bulb, color, on);

    return VISUALIZATION_OK;
}


/* --------------------------------------------------------------------------
 *                               Local Functions
 * -------------------------------------------------------------------------- */

static void vis_print(void)
{
    for (size_t i = 0; i < LIGHTS_COUNT; i++)
    {
        printf("%s", g_visualizations[i]);
    }
}

static int vis_getBulbPositionInLight(enum LightPosition light,
                                      enum LightColor color)
{
    // Green, Yellow, Red (in visualization)
    if (light == TOP_DOWNWARDS || light == LEFT_RIGHTWARDS)
    {
        switch (color)
        {
            case GREEN:  return 1;
            case YELLOW: return 2;
            case RED:    return 3;
            default:     break;
        }
    }
    else // Red, Yellow, Green (in visualization)
    {
        switch (color)
        {
            case RED:    return 1;
            case YELLOW: return 2;
            case GREEN:  return 3;
            default:     break;
        }
    }

    fprintf(stderr, "Error: unexpected color %d received.\n", color);
    return VISUALIZATION_ERROR;
}

static void vis_setColor(char *light, enum LightColor color, bool on)
{
    const char *termColor = LIGHTBULB_OFF;

    if (on)
    {
        switch (color)
        {
            case RED:    termColor = LIGHTBULB_RED;    break;
            case YELLOW: termColor = LIGHTBULB_YELLOW; break;
            case GREEN:  termColor = LIGHTBULB_GREEN;  break;
            default:
                fprintf(stderr, "Warning: unexpected color %d received. "
                                "Turning bulb off.\n", color);
                break;
        }
    }

    for (size_t i = 0; i < LIGHTBULB_COLOR_SEQ_LEN; i++)
    {
        light[i] = termColor[i];
    }
}


/* --------------------------------------------------------------------------
 *                               Data
 * -------------------------------------------------------------------------- */

static char g_visualizations[LIGHTS_COUNT][SINGLE_VIS_SIZE] = {
    "             |\n"
    "            ---\n"
    "           | " LIGHTBULB_OFF " |\n"
    "           | " LIGHTBULB_OFF " |\n"
    "           | " LIGHTBULB_OFF " |\n"
    "            ---\n",

    "                                -------\n"
    "                               | " LIGHTBULB_OFF " " LIGHTBULB_OFF " " LIGHTBULB_OFF " |--\n"
    "                                -------\n"
    "\n"
    "\n"
    "\n",

    "\n"
    "\n"
    "\n"
    "   -------\n"
    "--| " LIGHTBULB_OFF " " LIGHTBULB_OFF " " LIGHTBULB_OFF " |\n"
    "   -------\n",

    "                           ---\n"
    "                          | " LIGHTBULB_OFF " |\n"
    "                          | " LIGHTBULB_OFF " |\n"
    "                          | " LIGHTBULB_OFF " |\n"
    "                           ---\n"
    "                            |\n"
};
