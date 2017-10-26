#ifndef SHT1x_H
#define	SHT1x_H

#ifdef	__cplusplus
extern "C" {
#endif
    
void configure_sht();
void sht_soft_reset();
void sht_reset();
float sht_read_temp();
float sht_read_humidity();

#ifdef	__cplusplus
}
#endif

#endif	/* HELPER_H */
