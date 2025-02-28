
/*
 * Trial License - for use to evaluate programs for possible purchase as
 * an end-user only.
 *
 * File: Vehicle_State0_types.h
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

#ifndef Vehicle_State0_types_h_
#define Vehicle_State0_types_h_
#include <stdint.h>
#ifndef DEFINED_TYPEDEF_FOR_OperationMode_
#define DEFINED_TYPEDEF_FOR_OperationMode_

typedef enum
{
    PARKING = 0,                       /* Default value */
    DRIVING,
    REVERSE,
    BRKING
}
OperationMode;

#endif

#ifndef DEFINED_TYPEDEF_FOR_OperationDoorState_
#define DEFINED_TYPEDEF_FOR_OperationDoorState_

typedef enum
{
    CLOSE = 0,                         /* Default value */
    OPEN
}
OperationDoorState;

#endif

#ifndef DEFINED_TYPEDEF_FOR_Vehicle_State_
#define DEFINED_TYPEDEF_FOR_Vehicle_State_

typedef enum
{
    IDLE = 0,                          /* Default value */
    WORK
}
Vehicle_State;

#endif

#ifndef DEFINED_TYPEDEF_FOR_Gear_State_
#define DEFINED_TYPEDEF_FOR_Gear_State_

typedef enum
{
    P = 0,                             /* Default value */
    D,
    R,
    B
}
Gear_State;

#endif

#ifndef DEFINED_TYPEDEF_FOR_Door_State_
#define DEFINED_TYPEDEF_FOR_Door_State_

typedef enum
{
    CLOSED = 0,                        /* Default value */
    OPENED
}
Door_State;

#endif

/* Forward declaration for rtModel */
typedef struct tag_RTM RT_MODEL;

#endif                                 /* Vehicle_State0_types_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
