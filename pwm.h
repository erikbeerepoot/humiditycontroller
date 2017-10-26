/* 
 * File:   pwm.h
 * Author: erik
 *
 * Created on August 6, 2017, 5:13 PM
 */

#ifndef PWM_H
#define	PWM_H

#ifdef	__cplusplus
extern "C" {
#endif

int configure_pwm();
int start_pwm();
int stop_pwm();
int set_pwm_setpoint(int setpoint);
int get_pwm_setpoint();

#ifdef	__cplusplus
}
#endif

#endif	/* PWM_H */

