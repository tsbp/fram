//==============================================================================
#include "msp430.h"
//==============================================================================
#define X_COORD          (0)
//#define Y_COORD          (100)
#define LEFT_OFFSET      (30)
#define RIGHT_OFFSET     (3)
#define X_OFFSET         (LEFT_OFFSET + RIGHT_OFFSET)
#define TOP_OFFSET       (10)
#define BOTTOM_OFFSET    (5)
#define Y_OFFSET         (TOP_OFFSET  + BOTTOM_OFFSET)

#define AREA_WIDTH       (240)
#define AREA_HEIGH       (155)
#define PLOT_WIDTH       (AREA_WIDTH - X_OFFSET)
#define PLOT_HEIGH       (AREA_HEIGH - Y_OFFSET)
#define POINTS_CNT       (24)
#define HGRID_SPACING    (PLOT_WIDTH / (POINTS_CNT-1))   // dots between dividings
#define VGRID_CNT        (10)
#define VGRID_SPACING    (PLOT_HEIGH / VGRID_CNT)
//==============================================================================
#define PLOT_INTERVAL   (600)
//==============================================================================
extern signed int tBuffer[POINTS_CNT], tBuffer2[POINTS_CNT];
extern unsigned int plotIntervalCntr;
//==============================================================================
void valueToBuffer(signed int aVal, signed int *aBuf);
void showGraphic(signed int *aBuf, unsigned int aY);