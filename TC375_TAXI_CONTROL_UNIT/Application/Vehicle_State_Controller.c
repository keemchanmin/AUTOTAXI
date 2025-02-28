
/*
 * Trial License - for use to evaluate programs for possible purchase as
 * an end-user only.
 *
 * File: Vehicle_State_Controller.c
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

#include <stdbool.h>
#include "Vehicle_State0_types.h"
#include <stdint.h>
#include "Vehicle_State_Controller.h"

/* Named constants for Chart: '<Root>/Vehicle_State' */
#define IN_BRAKE                       ((uint8_t)1U)
#define IN_DOOR_CLOSE                  ((uint8_t)1U)
#define IN_DOOR_IDLE                   ((uint8_t)2U)
#define IN_DOOR_OPEN                   ((uint8_t)3U)
#define IN_DRIVING                     ((uint8_t)2U)
#define IN_IDLE                        ((uint8_t)1U)
#define IN_NO_ACTIVE_CHILD             ((uint8_t)0U)
#define IN_OBSTACLE_DETECTED           ((uint8_t)3U)
#define IN_PARKING                     ((uint8_t)4U)
#define IN_REVERSE                     ((uint8_t)5U)
#define IN_WORK                        ((uint8_t)2U)

/* System initialize for atomic system: '<Root>/Vehicle_State' */
void State_Control_Init(bool *rty_isEnd, Vehicle_State *rty_vehicle_state,
                        Gear_State *rty_gear_state, bool *rty_obstacle_detected,
                        Door_State *rty_door_state)
{
    *rty_isEnd = false;
    *rty_vehicle_state = IDLE;
    *rty_gear_state = P;
    *rty_obstacle_detected = false;
    *rty_door_state = CLOSED;
}

/* Output and update for atomic system: '<Root>/Vehicle_State' */
void State_Control(bool rtu_isStart, bool rtu_isArrived, OperationMode
                   rtu_operation, uint16_t rtu_left_speed, uint16_t
                   rtu_right_speed, uint32_t rtu_r_distance, uint32_t
                   rtu_f_distance, OperationDoorState rtu_operationDoorState,
                   bool rtu_isOpenComplete, bool rtu_isCloseComplete, bool
                   *rty_isEnd, Vehicle_State *rty_vehicle_state, Gear_State
                   *rty_gear_state, bool *rty_obstacle_detected, Door_State
                   *rty_door_state, DW_State_Control *localDW)
{
    /* Chart: '<Root>/Vehicle_State' */
    if (localDW->is_active_c2_Vehicle_State0 == 0)
    {
        localDW->is_active_c2_Vehicle_State0 = 1U;
        localDW->is_c2_Vehicle_State0 = IN_IDLE;
        *rty_isEnd = false;
        *rty_vehicle_state = IDLE;
    }
    else if (localDW->is_c2_Vehicle_State0 == IN_IDLE)
    {
        *rty_vehicle_state = IDLE;
        if (rtu_isStart)
        {
            localDW->is_c2_Vehicle_State0 = IN_WORK;
            *rty_vehicle_state = WORK;
            localDW->is_GEAR = IN_PARKING;
            *rty_obstacle_detected = false;
            *rty_gear_state = P;
            localDW->is_DOOR = IN_DOOR_IDLE;
            *rty_door_state = CLOSED;
        }
    }
    else
    {
        /* case IN_WORK: */
        *rty_vehicle_state = WORK;
        if (*rty_isEnd)
        {
            localDW->is_DOOR = IN_NO_ACTIVE_CHILD;
            localDW->is_GEAR = IN_NO_ACTIVE_CHILD;
            localDW->is_c2_Vehicle_State0 = IN_IDLE;
            *rty_isEnd = false;
            *rty_vehicle_state = IDLE;
        }
        else
        {
            switch (localDW->is_GEAR)
            {
              case IN_BRAKE:
                *rty_gear_state = B;
                if ((rtu_operation == PARKING) && ((rtu_left_speed == 0) &&
                        (rtu_right_speed == 0)))
                {
                    localDW->is_GEAR = IN_PARKING;
                    *rty_obstacle_detected = false;
                    *rty_gear_state = P;
                }
                else if ((rtu_f_distance < 10U) || (rtu_r_distance < 10U))
                {
                    localDW->is_GEAR = IN_OBSTACLE_DETECTED;
                    *rty_obstacle_detected = true;
                }
                break;

              case IN_DRIVING:
                *rty_gear_state = D;
                if (rtu_operation == BRKING)
                {
                    localDW->is_GEAR = IN_BRAKE;
                    *rty_gear_state = B;
                }
                else if ((rtu_f_distance <= 100U) && (rtu_f_distance > 10U))
                {
                    localDW->is_GEAR = IN_OBSTACLE_DETECTED;
                    *rty_obstacle_detected = true;
                }
                break;

              case IN_OBSTACLE_DETECTED:
                *rty_obstacle_detected = true;
                if ((rtu_f_distance > 100U) && (rtu_r_distance > 100U) &&
                        ((rtu_left_speed == 0) && (rtu_right_speed == 0)))
                {
                    localDW->is_GEAR = IN_PARKING;
                    *rty_obstacle_detected = false;
                    *rty_gear_state = P;
                }
                break;

              case IN_PARKING:
                *rty_obstacle_detected = false;
                *rty_gear_state = P;
                switch (rtu_operation)
                {
                  case REVERSE:
                    localDW->is_GEAR = IN_REVERSE;
                    *rty_gear_state = R;
                    break;

                  case DRIVING:
                    localDW->is_GEAR = IN_DRIVING;
                    *rty_gear_state = D;
                    break;
                }
                break;

              default:
                /* case IN_REVERSE: */
                *rty_gear_state = R;
                if (rtu_operation == BRKING)
                {
                    localDW->is_GEAR = IN_BRAKE;
                    *rty_gear_state = B;
                }
                else if ((rtu_r_distance <= 100U) && (rtu_r_distance > 10U))
                {
                    localDW->is_GEAR = IN_OBSTACLE_DETECTED;
                    *rty_obstacle_detected = true;
                }
                break;
            }

            switch (localDW->is_DOOR)
            {
              case IN_DOOR_CLOSE:
                *rty_isEnd = (rtu_isCloseComplete || (*rty_isEnd));
                break;

              case IN_DOOR_IDLE:
                if ((localDW->is_GEAR == IN_PARKING) && rtu_isArrived &&
                        (rtu_operationDoorState == OPEN))
                {
                    localDW->is_DOOR = IN_DOOR_OPEN;
                }
                break;

              default:
                /* case IN_DOOR_OPEN: */
                if (rtu_isOpenComplete && (localDW->is_GEAR == IN_PARKING))
                {
                    *rty_door_state = OPENED;
                    if (rtu_operationDoorState == CLOSE)
                    {
                        localDW->is_DOOR = IN_DOOR_CLOSE;
                        *rty_isEnd = (rtu_isCloseComplete || (*rty_isEnd));
                    }
                }
                break;
            }
        }
    }

    /* End of Chart: '<Root>/Vehicle_State' */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
