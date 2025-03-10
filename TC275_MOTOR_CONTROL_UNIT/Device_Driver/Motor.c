/**********************************************************************************************************************
 * \file Motor.c
 * \copyright Copyright (C) Infineon Technologies AG 2019
 * 
 * Use of this file is subject to the terms of use agreed between (i) you or the company in which ordinary course of 
 * business you are acting and (ii) Infineon Technologies AG or its licensees. If and as long as no such terms of use
 * are agreed, use of this file is subject to following:
 * 
 * Boost Software License - Version 1.0 - August 17th, 2003
 * 
 * Permission is hereby granted, free of charge, to any person or organization obtaining a copy of the software and 
 * accompanying documentation covered by this license (the "Software") to use, reproduce, display, distribute, execute,
 * and transmit the Software, and to prepare derivative works of the Software, and to permit third-parties to whom the
 * Software is furnished to do so, all subject to the following:
 * 
 * The copyright notices in the Software and this entire statement, including the above license grant, this restriction
 * and the following disclaimer, must be included in all copies of the Software, in whole or in part, and all 
 * derivative works of the Software, unless such copies or derivative works are solely in the form of 
 * machine-executable object code generated by a source language processor.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE 
 * COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN 
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 * IN THE SOFTWARE.
 *********************************************************************************************************************/


/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/
#include "Motor.h"
#include "PID_CON.h"
#include "Ifx_Types.h"
#include "IfxGtm_Atom_Pwm.h"
#include "IfxPort_PinMap.h"
#include "IfxGpt12_IncrEnc.h"
#include "math.h"
#include "Device_Driver/Asclin3.h"
/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/
#define ISR_PRIORITY_INCRENC_ZERO 6
#define ISR_PRIORITY_ATOM  20

#define PORT_PWMA          IfxGtm_ATOM1_1_TOUT1_P02_1_OUT
#define PORT_PWMB          IfxGtm_ATOM4_3_TOUT105_P10_3_OUT

#define PORT_GPT12A_A       IfxGpt120_T2INA_P00_7_IN
#define PORT_GPT12A_B       IfxGpt120_T2EUDA_P00_8_IN

#define PORT_GPT12B_A       IfxGpt120_T3INB_P10_4_IN
#define PORT_GPT12B_B       IfxGpt120_T3EUDB_P10_7_IN

#define PWM_PERIOD         5000
#define CLK_FREQ           1000000.0f

#define PIN_BRAKEA          &MODULE_P02,7
#define PIN_BRAKEB          &MODULE_P02,6

#define PIN_DIRA            &MODULE_P10,1
#define PIN_DIRB            &MODULE_P10,2

#define PULSES_PER_REV      12

#define TIMER_EVENT_MS       10  //PI 제어주기

#define RPM_MAX 9900
#define RPM_MIN -9900

#define WHEEL_RADIUS        0.32705
#define PI                  3.14159265358979
#define WHEEL_BASE          1.825f



#define GPT12_T2_ISR_PRIORITY         7
#define GPT12_T3_ISR_PRIORITY         8
#define GPT12_CLOCK_HZ              12500000
/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/
IfxGtm_Atom_Pwm_Config g_pwm_config_a;                            /* Timer configuration structure                    */
IfxGtm_Atom_Pwm_Config g_pwm_config_b;                            /* Timer configuration structure                    */

IfxGtm_Atom_Pwm_Driver g_pwm_driver_a;                            /* Timer Driver structure                           */
IfxGtm_Atom_Pwm_Driver g_pwm_driver_b;

IfxGpt12_IncrEnc_Config gpt12_config_a;
IfxGpt12_IncrEnc_Config gpt12_config_b;


IfxGpt12_IncrEnc gpt12_a;
IfxGpt12_IncrEnc gpt12_b;

const uint8 CPR = (PULSES_PER_REV * 4);  // 4체배시 한바퀴 펄스 수
const float32 s_T_samp= 0.001 * TIMER_EVENT_MS;
/*********************************************************************************************************************/
/*--------------------------------------------Private Variables/Constants--------------------------------------------*/
/*********************************************************************************************************************/

PIDREG3 speed_pid_a = PIDREG3_DEFAULTS;
PIDREG3 speed_pid_b = PIDREG3_DEFAULTS;

volatile sint32 enc_count_new_a = 0;
volatile sint32 enc_count_old_a = 0;
volatile float32 enc_count_diff_a = 0;
volatile float32 motor_speed_rpm_a = 0;
volatile float32 motor_prev_speed_rpm_a = 0;


volatile sint32 enc_count_new_b = 0;
volatile sint32 enc_count_old_b = 0;
volatile float32 enc_count_diff_b = 0;
volatile float32 motor_speed_rpm_b = 0;
volatile float32 motor_prev_speed_rpm_b = 0;

volatile sint32 rpm_order_right = 0, rpm_order_left = 0;
volatile float32 Kp_a = 3.3, Ki_a = 0.32, Kd_a = 0;
volatile float32 Kp_b = 3.2, Ki_b = 0.3, Kd_b = 0;

volatile float32 motor_speed_rpm_avg;
volatile float32 car_velocity_m_s;
volatile float32 left_motor_velocity_m_s;
volatile float32 right_motor_velocity_m_s;

volatile float32 diff_motor_velocity_m_s;

volatile float32 car_velocity_km_s;
float32 left_right_difference;


volatile uint32 prev_pulse_time_a = 0;
volatile uint32 current_pulse_time_a = 0;

volatile float32 first_pulse_time_a,last_pulse_time_a,last_pulse_interval_a = 0;

static boolean inner_power_brake_flag;
/*********************************************************************************************************************/
/*------------------------------------------------Function Prototypes------------------------------------------------*/
/*********************************************************************************************************************/
void InitPin(void);
void InitGPT(void);
void InitPWM(void);
void CalcRPM(void);
void SetDutyCycle(uint32, MOTOR_CHANNEL);
void SetMotorControl(uint8, uint8, MOTOR_CHANNEL);
void UpdatePID(PIDREG3*, MOTOR_CHANNEL);
void ControlPID(void);
/*********************************************************************************************************************/
/*---------------------------------------------Function Implementations----------------------------------------------*/
/*********************************************************************************************************************/

IFX_INTERRUPT(GPT12_T2_ISR, 0, GPT12_T2_ISR_PRIORITY);
void GPT12_T2_ISR(void)
{
    // 현재 GPT12 타이머 값 읽기
    current_pulse_time_a = IfxGpt12_T2_getTimerValue(&MODULE_GPT120);

    // 최초 펄스(Tm1) 저장
    if (first_pulse_time_a == 0)
    {
        first_pulse_time_a = current_pulse_time_a;
    }

    // 마지막 펄스(Tm2) 저장
    last_pulse_time_a = current_pulse_time_a;

    // 펄스 간 간격 계산 (T 방식)
    if (prev_pulse_time_a != 0)
    {
        last_pulse_interval_a = (float32)(current_pulse_time_a - prev_pulse_time_a) / GPT12_CLOCK_HZ;
    }

    prev_pulse_time_a = current_pulse_time_a;  // 이전 펄스 시간 업데이트


}


void CalcRPM2(void)
{
    // 1. 엔코더 펄스 개수 차이 계산 (M 방식)
    enc_count_new_a = gpt12_config_a.module->T2.U;
    enc_count_diff_a = (float32)(enc_count_new_a - enc_count_old_a);

    // 2. 인터럽트에서 계산된 펄스 간 간격(T 방식) 활용
    float32 T_prime_a = last_pulse_interval_a;  // 펄스 간 시간 (초 단위)

    // 3. RPM 계산 (보정된 주기 기반)
    if (T_prime_a > 0)  // 0으로 나누는 경우 방지
    {
        motor_speed_rpm_a = (60.0f / (float32)CPR*T_prime_a );
    }
    else
    {
        motor_speed_rpm_a = 0;  // 속도 측정 불가능할 때 0으로 설정
    }

    // 4. 기존 값 업데이트
    enc_count_old_a = enc_count_new_a;
}


void InitPin(void){
    IfxPort_setPinMode(PIN_DIRA, IfxPort_Mode_outputPushPullGeneral);
    IfxPort_setPinState(PIN_DIRA, IfxPort_State_low);
    IfxPort_setPinMode(PIN_DIRB, IfxPort_Mode_outputPushPullGeneral);
    IfxPort_setPinState(PIN_DIRB, IfxPort_State_low);
    IfxPort_setPinMode(PIN_BRAKEA, IfxPort_Mode_outputPushPullGeneral);
    IfxPort_setPinState(PIN_BRAKEA, IfxPort_State_low);
    IfxPort_setPinMode(PIN_BRAKEB, IfxPort_Mode_outputPushPullGeneral);
    IfxPort_setPinState(PIN_BRAKEB, IfxPort_State_low);
}

void InitGPT(void){
    IfxGpt12_enableModule(&MODULE_GPT120);
    IfxGpt12_setGpt1BlockPrescaler(&MODULE_GPT120, IfxGpt12_Gpt1BlockPrescaler_8);
    IfxGpt12_setGpt2BlockPrescaler(&MODULE_GPT120, IfxGpt12_Gpt2BlockPrescaler_4);


    // Create module config
    IfxGpt12_IncrEnc_initConfig(&gpt12_config_a, &MODULE_GPT120);

    // Configure encoder parameters
    gpt12_config_a.base.offset               = 100;                    // Initial position offset
    gpt12_config_a.base.reversed             = FALSE;               // Count direction not reversed
    gpt12_config_a.base.resolution           = PULSES_PER_REV;                // Encoder resolution
    gpt12_config_a.base.periodPerRotation    = 1;                   // Number of periods per rotation
    gpt12_config_a.base.resolutionFactor     = IfxStdIf_Pos_ResolutionFactor_fourFold;  // Quadrature mode
    gpt12_config_a.base.updatePeriod         = 0.001;              // 1ms update period
    gpt12_config_a.base.speedModeThreshold   = 100;                // Threshold for speed calculation mode
    gpt12_config_a.base.minSpeed             = 10;                 // Minimum speed in rpm
    gpt12_config_a.base.maxSpeed             = 5000;                // Maximum speed in rpm

    // Configure pins
    gpt12_config_a.pinA = &PORT_GPT12A_A;     // Encoder A signal -> T3IN
    gpt12_config_a.pinB = &PORT_GPT12A_B;    // Encoder B signal -> T3EUD
    gpt12_config_a.pinZ = NULL;                          // No Z signal used
    gpt12_config_a.pinMode = IfxPort_InputMode_pullDown;   // Use internal pullup

    // Configure interrupts
    gpt12_config_a.zeroIsrPriority = ISR_PRIORITY_INCRENC_ZERO;
    //IfxGpt12_T2_setInterruptEnable(&MODULE_GPT120, GPT12_T2_ISR_PRIORITY);
    gpt12_config_a.zeroIsrProvider = IfxSrc_Tos_cpu0;


    /* Initialize the interrupt */
//    volatile Ifx_SRC_SRCR *src = IfxGpt12_T2_getSrc(&MODULE_GPT120);                /* Get the interrupt address    */
//    IfxSrc_init(src, 0, GPT12_T2_ISR_PRIORITY);           /* Initialize service request   */
//    IfxSrc_enable(src);



    // Enable speed filter
    gpt12_config_a.base.speedFilterEnabled = TRUE;
    gpt12_config_a.base.speedFilerCutOffFrequency = gpt12_config_a.base.maxSpeed / 2 * IFX_PI * 2;

    IfxGpt12_IncrEnc_initConfig(&gpt12_config_b, &MODULE_GPT120);

    // Configure encoder parameters
    gpt12_config_b.base.offset               = 100;                    // Initial position offset
    gpt12_config_b.base.reversed             = FALSE;               // Count direction not reversed
    gpt12_config_b.base.resolution           = PULSES_PER_REV;                // Encoder resolution
    gpt12_config_b.base.periodPerRotation    = 1;                   // Number of periods per rotation
    gpt12_config_b.base.resolutionFactor     = IfxStdIf_Pos_ResolutionFactor_fourFold;  // Quadrature mode
    gpt12_config_b.base.updatePeriod         = 0.001;              // 1ms update period
    gpt12_config_b.base.speedModeThreshold   = 100;                // Threshold for speed calculation mode
    gpt12_config_b.base.minSpeed             = 10;                 // Minimum speed in rpm
    gpt12_config_b.base.maxSpeed             = 5000;                // Maximum speed in rpm

    // Configure pins
    gpt12_config_b.pinA = &PORT_GPT12B_A;     // Encoder A signal -> T3IN
    gpt12_config_b.pinB = &PORT_GPT12B_B;    // Encoder B signal -> T3EUD
    gpt12_config_b.pinZ = NULL;                          // No Z signal used
    gpt12_config_b.pinMode = IfxPort_InputMode_pullDown;   // Use internal pullup

    // Configure interrupts
    //gpt12_config_b.zeroIsrPriority = ISR_PRIORITY_INCRENC_ZERO + 1;
    //IfxGpt12_T3_setInterruptEnable(&gpt12_b, GPT12_T2_ISR_PRIORITY);
    gpt12_config_b.zeroIsrProvider = IfxSrc_Tos_cpu0;

    // Enable speed filter
    gpt12_config_b.base.speedFilterEnabled = TRUE;
    gpt12_config_b.base.speedFilerCutOffFrequency = gpt12_config_b.base.maxSpeed / 2 * IFX_PI * 2;

    // Initialize module
    IfxGpt12_IncrEnc_init(&gpt12_a, &gpt12_config_a);
    IfxGpt12_IncrEnc_init(&gpt12_b, &gpt12_config_b);




}

void InitPWM(void){
    IfxGtm_enable(&MODULE_GTM); /* Enable GTM */

    IfxGtm_Cmu_setClkFrequency(&MODULE_GTM, IfxGtm_Cmu_Clk_0, CLK_FREQ);        /* Set the CMU clock 0 frequency    */
    IfxGtm_Cmu_enableClocks(&MODULE_GTM, IFXGTM_CMU_CLKEN_ALL);
    IfxGtm_Atom_Pwm_initConfig(&g_pwm_config_a, &MODULE_GTM);                     /* Initialize default parameters    */
//
    g_pwm_config_a.atom = PORT_PWMA.atom;                                       /* Select the ATOM depending on the PORT_PWMA     */
    g_pwm_config_a.atomChannel = PORT_PWMA.channel;                             /* Select the channel depending on the PORT_PWMA  */
    g_pwm_config_a.dutyCycle = 0;
    g_pwm_config_a.period = PWM_PERIOD;                                   /* Set timer period                         */
    g_pwm_config_a.pin.outputPin = &PORT_PWMA;                                  /* Set LED as output                        */
    g_pwm_config_a.synchronousUpdateEnabled = TRUE;                       /* Enable synchronous update                */

    IfxGtm_Atom_Pwm_init(&g_pwm_driver_a, &g_pwm_config_a);                 /* Initialize the PWM                       */

    IfxGtm_Atom_Pwm_initConfig(&g_pwm_config_b, &MODULE_GTM);                     /* Initialize default parameters    */

    g_pwm_config_b.atom = PORT_PWMB.atom;                                       /* Select the ATOM depending on the PORT_PWMA     */
    g_pwm_config_b.atomChannel = PORT_PWMB.channel;                             /* Select the channel depending on the PORT_PWMA  */
    g_pwm_config_b.dutyCycle = 0;
    g_pwm_config_b.period = PWM_PERIOD;                                   /* Set timer period                         */
    g_pwm_config_b.pin.outputPin = &PORT_PWMB;                                  /* Set LED as output                        */
    g_pwm_config_b.synchronousUpdateEnabled = TRUE;                       /* Enable synchronous update                */

    IfxGtm_Atom_Pwm_init(&g_pwm_driver_b, &g_pwm_config_b);                 /* Initialize the PWM                       */

    IfxGtm_Atom_Pwm_start(&g_pwm_driver_a, TRUE);
    IfxGtm_Atom_Pwm_start(&g_pwm_driver_b, TRUE);
}

void CalcRPM(void)
{
    enc_count_new_a = gpt12_config_a.module->T2.U;
    enc_count_diff_a = (float32)(enc_count_new_a - enc_count_old_a);
    motor_speed_rpm_a = enc_count_diff_a/(float32)CPR/(float32)(TIMER_EVENT_MS*0.001)*60.0f;
    enc_count_old_a = enc_count_new_a;

    enc_count_new_b = gpt12_config_b.module->T3.U;
    enc_count_diff_b = (float32)(enc_count_new_b - enc_count_old_b);
    motor_speed_rpm_b = enc_count_diff_b/(float32)CPR/(float32)(TIMER_EVENT_MS*0.001)*60.0f;
    enc_count_old_b = enc_count_new_b;
}

void SetDutyCycle(uint32 dutyCycle, MOTOR_CHANNEL channel){
    switch(channel){
        case MOTOR_CHANNEL_A:
        {
            g_pwm_config_a.dutyCycle = dutyCycle;
            IfxGtm_Atom_Pwm_init(&g_pwm_driver_a, &g_pwm_config_a);
        }
        break;
        case MOTOR_CHANNEL_B:
        {
            g_pwm_config_b.dutyCycle = dutyCycle;
            IfxGtm_Atom_Pwm_init(&g_pwm_driver_b, &g_pwm_config_b);
        }
        break;
    }
}
void SetMotorControl(uint8 direction, uint8 enable, MOTOR_CHANNEL channel)
{
    switch(channel){
        case MOTOR_CHANNEL_A:
        {
            IfxPort_setPinState(PIN_BRAKEA, ((enable == 0) ? IfxPort_State_high: IfxPort_State_low));
            IfxPort_setPinState(PIN_DIRA, ((direction == 0) ? IfxPort_State_low: IfxPort_State_high));
        }
        break;
        case MOTOR_CHANNEL_B:
        {
            IfxPort_setPinState(PIN_BRAKEB, ((enable == 0) ? IfxPort_State_high: IfxPort_State_low));
            IfxPort_setPinState(PIN_DIRB, ((direction == 0) ? IfxPort_State_low: IfxPort_State_high));
        }
        break;
    }
}
void UpdatePID(PIDREG3* data, MOTOR_CHANNEL channel)
{
    switch(channel){
        case MOTOR_CHANNEL_A:
        {
            data->Kp = Kp_a;
            data->Ki = Ki_a;
            data->Kd = Kd_a;
            data->Kc = 1/Kp_a;
        }
        break;
        case MOTOR_CHANNEL_B:
        {
            data->Kp = Kp_b;
            data->Ki = Ki_b;
            data->Kd = Kd_b;
            data->Kc = 1/Kp_b;
        }
        break;
    }
    data->T_samp = s_T_samp;
//        speed_pid.Current_mode = 1;
    data->OutMax = RPM_MAX;
    data->OutMin = RPM_MIN;

}
void ControlPID(void)
{
//    RPM_Err = RPM_CMD - motor_speed_rpm;
    UpdatePID(&speed_pid_a, MOTOR_CHANNEL_A);
    // Initialze the PID module for Current
    if(rpm_order_right==0)
    {
        speed_pid_a.reset((void *)&speed_pid_a);
        if(!Motor_Get_Power_Brake_Flag()) SetMotorControl(0, 0, MOTOR_CHANNEL_A);

    }
    else
    {
        speed_pid_a.Ref=rpm_order_right;   // speed reference
        speed_pid_a.Fdb= motor_speed_rpm_a; // speed measured by ENC
        speed_pid_a.calc((void *)&speed_pid_a);   // Calculate speed PID Controller
    }

    UpdatePID(&speed_pid_b, MOTOR_CHANNEL_B);

    if(rpm_order_left==0)
    {
        speed_pid_b.reset((void *)&speed_pid_b);
        if(!Motor_Get_Power_Brake_Flag()) SetMotorControl(0, 0, MOTOR_CHANNEL_B);

    }
    else
    {
        speed_pid_b.Ref=rpm_order_left;   // speed reference
        speed_pid_b.Fdb= motor_speed_rpm_b; // speed measured by ENC
        speed_pid_b.calc((void *)&speed_pid_b);   // Calculate speed PID Controller
    }
}

void Motor_Init(void){
    InitPWM();
    InitGPT();
    InitPin();
}

void Motor_SetRPM(sint32 speed, MOTOR_CHANNEL c){
    switch(c){
        case MOTOR_CHANNEL_A:
        {
            rpm_order_right = speed*60*30/(2*PI*WHEEL_RADIUS);;
        }
        break;
        case MOTOR_CHANNEL_B:
        {
            rpm_order_left = speed*60*30/(2*PI*WHEEL_RADIUS);;
        }
        break;
    }
}

void Motor_SetDuty(uint32 v, MOTOR_CHANNEL c){
    if(v >= PWM_PERIOD)
    {
        v = PWM_PERIOD; /* Set the direction of the fade */
    }
    else if(v <= 0)
    {
        v = 0;  /* Set the direction of the fade */
    }
    SetDutyCycle(v, c); /* Call the function which is setting the duty cycle of the PWM */
}

void Motor_Event_10MS(void){
    CalcRPM();
    ControlPID();
    if (speed_pid_a.Out>=0){
        if(!Motor_Get_Power_Brake_Flag()) SetMotorControl(0, 1, MOTOR_CHANNEL_A);
        speed_pid_a.Out = (speed_pid_a.Out/RPM_MAX)*PWM_PERIOD;
        Motor_SetDuty(speed_pid_a.Out, MOTOR_CHANNEL_A);

    }
    else {
        if(!Motor_Get_Power_Brake_Flag()) SetMotorControl(1, 1, MOTOR_CHANNEL_A);
        speed_pid_a.Out = (speed_pid_a.Out/RPM_MIN)*PWM_PERIOD;
        Motor_SetDuty(speed_pid_a.Out * -1, MOTOR_CHANNEL_A);
    }

    if(speed_pid_b.Out >= 0){
        if(!Motor_Get_Power_Brake_Flag()) SetMotorControl(0, 1, MOTOR_CHANNEL_B);
        speed_pid_b.Out = (speed_pid_b.Out/RPM_MAX)*PWM_PERIOD;
        Motor_SetDuty(speed_pid_b.Out, MOTOR_CHANNEL_B);
    }
    else {
        if(!Motor_Get_Power_Brake_Flag()) SetMotorControl(1, 1, MOTOR_CHANNEL_B);
        speed_pid_b.Out = (speed_pid_b.Out/RPM_MIN)*PWM_PERIOD;
        Motor_SetDuty(speed_pid_b.Out*-1, MOTOR_CHANNEL_B);
    }

}

void Motor_Get_Veclocity(uint16* Out_left_motor_velocity_m_s, uint16* Out_right_motor_velocity_m_s, uint16* Out_yaw_rate,boolean* Out_yaw_rate_order)
{
     float32 motor_speed_rpm_avg = (motor_speed_rpm_a + motor_speed_rpm_b)/2;
     motor_speed_rpm_avg/=30;
     car_velocity_m_s = (motor_speed_rpm_avg *2*PI*WHEEL_RADIUS)/60;

     left_motor_velocity_m_s =(motor_speed_rpm_b/30)*PI*WHEEL_RADIUS*2/60;

     right_motor_velocity_m_s =(motor_speed_rpm_a/30)*PI*WHEEL_RADIUS*2/60;


     *Out_yaw_rate = (uint16)(((fabs(left_motor_velocity_m_s)-fabs(right_motor_velocity_m_s))/WHEEL_BASE)*1000);

     if (fabs(left_motor_velocity_m_s - right_motor_velocity_m_s) > 0.0001f)
     {
         *Out_yaw_rate_order = (left_motor_velocity_m_s < right_motor_velocity_m_s) ? FALSE : TRUE;
     }

     *Out_left_motor_velocity_m_s = (uint16)(left_motor_velocity_m_s*100);
     *Out_right_motor_velocity_m_s =(uint16)(right_motor_velocity_m_s*100);
}

void Motor_Move(float32 inner_yaw_rate,float32 inner_target_speed_m_s, Gear_State dir,boolean isPowerBrake)
{
    if(!Motor_Get_Power_Brake_Flag())
    {
        if(dir==D)
        {
            IfxPort_setPinState(PIN_DIRB, IfxPort_State_low);
            IfxPort_setPinState(PIN_DIRA, IfxPort_State_low);
        }
        else if(dir==R)
        {
            IfxPort_setPinState(PIN_DIRB, IfxPort_State_high);
            IfxPort_setPinState(PIN_DIRA, IfxPort_State_high);
        }
    }

     left_right_difference=WHEEL_BASE*inner_yaw_rate;
    if(inner_yaw_rate<0)  //우회전
    {
        left_motor_velocity_m_s=inner_target_speed_m_s-(left_right_difference/2);
        right_motor_velocity_m_s=inner_target_speed_m_s+(left_right_difference/2);

        rpm_order_left = left_motor_velocity_m_s*60*30/(2*PI*WHEEL_RADIUS);
        rpm_order_right = right_motor_velocity_m_s*60*30/(2*PI*WHEEL_RADIUS);
    }
    else if(inner_yaw_rate>0) //좌회전
    {
        left_motor_velocity_m_s=inner_target_speed_m_s-(left_right_difference/2);
        right_motor_velocity_m_s=inner_target_speed_m_s+(left_right_difference/2);

        rpm_order_left = left_motor_velocity_m_s*60*30/(2*PI*WHEEL_RADIUS);
        rpm_order_right = right_motor_velocity_m_s*60*30/(2*PI*WHEEL_RADIUS);
    }
    else
    {
        rpm_order_left = inner_target_speed_m_s*60*30/(2*PI*WHEEL_RADIUS);
        rpm_order_right = inner_target_speed_m_s*60*30/(2*PI*WHEEL_RADIUS);
    }

}

void Motor_PowerBrake(void)
{
    rpm_order_left=0;
    rpm_order_right=0;
}

void Motor_SoftBrake(void)
{
    left_motor_velocity_m_s--;
    right_motor_velocity_m_s--;
    if(left_motor_velocity_m_s<=0) left_motor_velocity_m_s=0;
    if(right_motor_velocity_m_s<=0) right_motor_velocity_m_s=0;
    rpm_order_left = left_motor_velocity_m_s*60*30/(2*PI*WHEEL_RADIUS);
    rpm_order_right = right_motor_velocity_m_s*60*30/(2*PI*WHEEL_RADIUS);
}
void Motor_BackSpin(Gear_State cur_dir)
{
    if(cur_dir==D)
    {
        IfxPort_setPinState(PIN_DIRB, IfxPort_State_high);
        IfxPort_setPinState(PIN_DIRA, IfxPort_State_high);
    }
    else if(cur_dir==R)
    {
        IfxPort_setPinState(PIN_DIRB, IfxPort_State_low);
        IfxPort_setPinState(PIN_DIRA, IfxPort_State_low);
    }

}

void Motor_Set_Power_Brake_Flag(boolean val)
{
    inner_power_brake_flag =val;
}
boolean Motor_Get_Power_Brake_Flag(void)
{
   return inner_power_brake_flag;
}
