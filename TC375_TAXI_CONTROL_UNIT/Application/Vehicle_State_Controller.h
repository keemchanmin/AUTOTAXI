
/*
 * Trial License - for use to evaluate programs for possible purchase as
 * an end-user only.
 *
 * File: Vehicle_State_Controller.h
 *
 * Code generated for Simulink model 'Vehicle_State0'.
 *
 * Model version                  : 1.64
 * Simulink Coder version         : 24.2 (R2024b) 21-Jun-2024
 * C/C++ source code generated on : Wed Feb 19 17:47:04 2025
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Infineon->TriCore
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. RAM efficiency
 * Validation result: Not run
 */

#ifndef Vehicle_State_Controller_h_
#define Vehicle_State_Controller_h_
#ifndef Vehicle_State0_COMMON_INCLUDES_
#define Vehicle_State0_COMMON_INCLUDES_
#include <stdbool.h>
#include <stdint.h>
#include "math.h"
#endif                                 /* Vehicle_State0_COMMON_INCLUDES_ */

#include "Vehicle_State0_types.h"

/* Block signals and states (default storage) for system '<Root>/Vehicle_State' */
typedef struct
{
    uint8_t is_active_c2_Vehicle_State0;/* '<Root>/Vehicle_State' */
    uint8_t is_c2_Vehicle_State0;      /* '<Root>/Vehicle_State' */
    uint8_t is_GEAR;                   /* '<Root>/Vehicle_State' */
    uint8_t is_DOOR;                   /* '<Root>/Vehicle_State' */
}
DW_State_Control;

extern void State_Control_Init(bool *rty_isEnd, Vehicle_State *rty_vehicle_state,
    Gear_State *rty_gear_state, bool *rty_obstacle_detected, Door_State
    *rty_door_state);
extern void State_Control(bool rtu_isStart, bool rtu_isArrived, OperationMode
    rtu_operation, uint16_t rtu_left_speed, uint16_t rtu_right_speed, uint32_t
    rtu_r_distance, uint32_t rtu_f_distance, OperationDoorState
    rtu_operationDoorState, bool rtu_isOpenComplete, bool rtu_isCloseComplete,
    bool *rty_isEnd, Vehicle_State *rty_vehicle_state, Gear_State
    *rty_gear_state, bool *rty_obstacle_detected, Door_State *rty_door_state,
    DW_State_Control *localDW);

#endif                                 /* Vehicle_State_Controller_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */


