/* 
 * File:   data_logger.h
 * Author: erik
 *
 * Created on October 11, 2017, 8:04 PM
 */

#ifndef DATA_LOGGER_H
#define	DATA_LOGGER_H

#ifdef	__cplusplus
extern "C" {
#endif

void log_entry(float humidity, float temperature);
short get_latest_entry(float *humidity, float *temperature);
short get_n_latest_entries(float *humidity_array, float *temperature_array,int n);

#ifdef	__cplusplus
}
#endif

#endif	/* DATA_LOGGER_H */

