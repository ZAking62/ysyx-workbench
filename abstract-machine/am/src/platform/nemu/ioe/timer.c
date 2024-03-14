#include <am.h>
#include <nemu.h>

void __am_timer_init() {
	outl(RTC_ADDR, 0);
	outl(RTC_ADDR + 4, 0);
}

//AM_TIMER_UPTIME_T在abstract-machine/am/include/amdev.h
void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
	//inl最后编译成nemu里的read内存
	uint32_t high = inl(RTC_ADDR + 4);
	uint32_t low = inl(RTC_ADDR);
  uptime->us = (uint64_t)low + (((uint64_t)high) << 32);
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 4;
  rtc->month  = 6;
  rtc->year   = 1989;
}
