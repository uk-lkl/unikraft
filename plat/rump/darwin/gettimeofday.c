#include <errno.h>
#include <time.h>

#include "darwin.h"
#include "syscall.h"


/* XXX: picked from https://github.com/apple/darwin-xnu/blob/0a798f6738bc1db01281fc08ae024145e84df927/libsyscall/wrappers/__commpage_gettimeofday.c */
typedef	volatile struct	commpage_timeofday_data{
	uint64_t	TimeStamp_tick;
	uint64_t	TimeStamp_sec;
	uint64_t	TimeStamp_frac;
	uint64_t	Ticks_scale;
	uint64_t	Ticks_per_sec;
} new_commpage_timeofday_data_t;

#define _COMM_PAGE64_BASE_ADDRESS	( 0x00007fffffe00000 )   /* base address of allocated memory */
#define _COMM_PAGE64_START_ADDRESS	( _COMM_PAGE64_BASE_ADDRESS )	/* address traditional commpage code starts on */
#define _COMM_PAGE_NEWTIMEOFDAY_DATA	(_COMM_PAGE64_START_ADDRESS + 0x0D0)

static __inline uint64_t
multi_overflow(uint64_t a, uint64_t b)
{
	__uint128_t prod;
	prod = (__uint128_t)a * (__uint128_t)b;
	return (uint64_t) (prod >> 64);
}

extern uint64_t mach_absolute_time(void);

int
__commpage_gettimeofday_internal(struct timeval *tp, uint64_t *tbr_out)
{
	uint64_t now, over;
	uint64_t delta,frac;
	uint64_t TimeStamp_tick;
	uint64_t TimeStamp_sec;
	uint64_t TimeStamp_frac;
	uint64_t Tick_scale;
	uint64_t Ticks_per_sec;

	volatile uint64_t *gtod_TimeStamp_tick_p;
	volatile uint64_t *gtod_TimeStamp_sec_p;
	volatile uint64_t *gtod_TimeStamp_frac_p;
	volatile uint64_t *gtod_Ticks_scale_p;
	volatile uint64_t *gtod_Ticks_per_sec_p;

	new_commpage_timeofday_data_t *commpage_timeofday_datap;

	commpage_timeofday_datap =  (new_commpage_timeofday_data_t *)_COMM_PAGE_NEWTIMEOFDAY_DATA;

	gtod_TimeStamp_tick_p = &commpage_timeofday_datap->TimeStamp_tick;
	gtod_TimeStamp_sec_p = &commpage_timeofday_datap->TimeStamp_sec;
	gtod_TimeStamp_frac_p = &commpage_timeofday_datap->TimeStamp_frac;
	gtod_Ticks_scale_p = &commpage_timeofday_datap->Ticks_scale;
	gtod_Ticks_per_sec_p = &commpage_timeofday_datap->Ticks_per_sec;

	do {
		TimeStamp_tick = *gtod_TimeStamp_tick_p;
		TimeStamp_sec = *gtod_TimeStamp_sec_p;
		TimeStamp_frac = *gtod_TimeStamp_frac_p;
		Tick_scale = *gtod_Ticks_scale_p;
		Ticks_per_sec = *gtod_Ticks_per_sec_p;
#if 1
		/*
		 * This call contains an instruction barrier which will ensure that the
		 * second read of the abs time isn't speculated above the reads of the
		 * other values above
		 */
		now = mach_absolute_time();
#endif
		break;
	} while (TimeStamp_tick != *gtod_TimeStamp_tick_p);

	if (TimeStamp_tick == 0)
		return(1);
#if 1
	delta = now - TimeStamp_tick;

	/* If more than one second force a syscall */
	if (delta >= Ticks_per_sec)
		return(1);
#else
	delta = 0;
#endif
	tp->tv_sec = TimeStamp_sec;

	over = multi_overflow(Tick_scale, delta);
	if(over){
		tp->tv_sec += over;
	}

	/* Sum scale*delta to TimeStamp_frac, if it overflows increment sec */
	frac = TimeStamp_frac;
	frac += Tick_scale * delta;
	if( TimeStamp_frac > frac )
		tp->tv_sec++;

	/*
	 * Convert frac (64 bit frac of a sec) to usec
	 * usec = frac * USEC_PER_SEC / 2^64
	 */
	tp->tv_usec = ((uint64_t)1000000 * (uint32_t)(frac >> 32)) >> 32;

	if (tbr_out) {
		*tbr_out = now;
	}

	return(0);
}

int __platform_gettimeofday(struct timeval *tv, void *tz)
{
	if (__commpage_gettimeofday_internal(tv, NULL)) {
		__platform_write(1, "fallback\n", 9);
		return syscall_2(SYS_gettimeofday, (unsigned long)tv, (unsigned long)tz);
	}

	return 0;
}
