#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>

#include "nrf_drv_saadc.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_timer.h"
#include "nrf_log.h"

typedef enum{
    SAADC_CH_SNORE=0,
    SAADC_CH_HEARTRATE,
    SAADC_CH_BATT,
    SAADC_CH_BREATH,
    SAADC_CH_MAX
}SAADC_CH;

void saadc_init(void);
void saadc_sampling_event_init(void);
void saadc_sampling_event_enable(void);
int16_t get_saadc_value(int i);