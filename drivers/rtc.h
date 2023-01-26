#ifndef _DRIVER_RTC
#define _DRIVER_RTC

typedef struct
{
  unsigned char second;
  unsigned char minute;
  unsigned char hour;
  unsigned char day;
  unsigned char month;
  unsigned int year;
} rtc_time;

rtc_time read_rtc(void);

#endif