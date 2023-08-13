#include "dcmotor.h"
#include "dcmotor_tim.h"
extern TIM_HandleTypeDef g_atimx_cplm_pwm_handle;                              /* 定时器x句柄 */


static uint16_t PWM_Period;

void dcmotor_init(uint16_t period)
{
    // atim_timx_cplm_pwm_init(8400 - 1, 0);   /* 168 000 000 / 1 = 168 000 000 168Mhz的计数频率，计数8400次为50us, 20KHz PWM */
    //atim_timx_cplm_pwm_init(4200 - 1, 0);   /* 168 000 000 / 1 = 168 000 000 168Mhz的计数频率，计数4200次为25us, 40KHz PWM */ 
    atim_timx_cplm_pwm_init(period - 1, 0);   /* 168 000 000 / 1 = 168 000 000 168Mhz的计数频率，计数4200次为25us, 40KHz PWM */ 
    ShutDonwPinInit;
    dcmotor_stop();                 /* 停止电机 */
    dcmotor_dir(0);                 /* 设置正转 */
    dcmotor_speed(0);               /* 速度设置为0 */
    dcmotor_start();                /* 开启电机 */

    PWM_Period = period;

}
void dcmotor_start(void)
{
    ENABLE_MOTOR;                                                       /* 拉高SD引脚，开启电机 */
}

void dcmotor_stop(void)
{
    HAL_TIM_PWM_Stop(&g_atimx_cplm_pwm_handle, TIM_CHANNEL_1);          /* 关闭主通道输出 */
    HAL_TIMEx_PWMN_Stop(&g_atimx_cplm_pwm_handle, TIM_CHANNEL_1);       /* 关闭互补通道输出 */
    DISABLE_MOTOR;                                                      /* 拉低SD引脚，停止电机 */
}

void dcmotor_dir(uint8_t para)
{
    HAL_TIM_PWM_Stop(&g_atimx_cplm_pwm_handle, TIM_CHANNEL_1);          /* 关闭主通道输出 */
    HAL_TIMEx_PWMN_Stop(&g_atimx_cplm_pwm_handle, TIM_CHANNEL_1);       /* 关闭互补通道输出 */

    if (para == 0)                /* 正转 */
    {
        HAL_TIM_PWM_Start(&g_atimx_cplm_pwm_handle, TIM_CHANNEL_1);     /* 开启主通道输出 */
    } 
    else if (para == 1)           /* 反转 */
    {
        HAL_TIMEx_PWMN_Start(&g_atimx_cplm_pwm_handle, TIM_CHANNEL_1);  /* 开启互补通道输出 */
    }
}

void dcmotor_speed(uint16_t para)
{
    if (para < (__HAL_TIM_GetAutoreload(&g_atimx_cplm_pwm_handle) - 0x0F))  /* 限速 */
    {  
        __HAL_TIM_SetCompare(&g_atimx_cplm_pwm_handle, TIM_CHANNEL_1, para);
    }
}

void motor_pwm_set(float para)
{
    int val = (int)para;

    if (val >= 0) 
    {
        dcmotor_dir(0);           /* 正转 */
        dcmotor_speed(val);
    } 
    else 
    {
        dcmotor_dir(1);           /* 反转 */
        dcmotor_speed(-val);
    }
}


static void motor_test( int argc, char**argv )
{
    if ( argc < 2 )
    {
        rt_kprintf( "Please input'motor_test <init|cmd2>'nn" );
        return;
    }
    if ( !rt_strcmp( argv[1], "init" ) )
    {
        rt_kprintf( "motor init 40K\n" );
        dcmotor_init(FREQ40K_Period);
    }
    if ( !rt_strcmp( argv[1], "init100K" ) )
    {
        rt_kprintf( "motor init 100K\n" );
        dcmotor_init(FREQ100K_Period);
    }
    if ( !rt_strcmp( argv[1], "init200K" ) )
    {
        rt_kprintf( "motor init 200K\n" );
        dcmotor_init(FREQ200K_Period);
    }
    if ( !rt_strcmp( argv[1], "stop" ) )
    {
        rt_kprintf( "motor stop \n" );
        dcmotor_stop();
    }
    
    if ( argc == 3 &&  !rt_strcmp( argv[1], "go" )) 
    {
        int x = atoi(argv[2]);
        rt_kprintf( "motor go %d[%d]\n", x, PWM_Period);
        dcmotor_start();
        motor_pwm_set(x);

    }

}

MSH_CMD_EXPORT(motor_test, motor test);
