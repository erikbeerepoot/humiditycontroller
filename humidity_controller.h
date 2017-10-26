/* 
 * File:   humidity_controller.h
 * Author: erik
 *
 * Created on October 15, 2017, 3:01 PM
 */

#ifndef HUMIDITY_CONTROLLER_H
#define	HUMIDITY_CONTROLLER_H

#ifdef	__cplusplus
extern "C" {
#endif

int run_loop();
void set_humidity_setpoint(float setpoint);
void set_pid_gains(float k_p_new, float k_i_new, float k_d_new);
void get_pid_gains(float *k_p_out, float *k_i_out, float *k_d_out);
float get_humidity_setpoint();
int get_fan_on_time();
void set_fan_on_time(int on_time);

#ifdef	__cplusplus
}
#endif

#endif	/* HUMIDITY_CONTROLLER_H */

