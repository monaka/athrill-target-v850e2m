#include "../inc/timer32.h"

#include "cpuemu_ops.h"
#include <stdio.h>

typedef enum {
	TIMER_STATE_STOP,
	TIMER_STATE_READY,
	TIMER_STATE_RUNNING
} TimerStateType;

typedef enum {
	TIMER_MODE_FREERUN,
	TIMER_MODE_INTERVAL
} TimerModeType;

typedef struct {
	uint32 				cnt;
	TimerStateType 		state;
	TimerModeType 		mode;
	uint32 				compare0;
	uint32 				compare0_intno;
	uint64 				start_clock;
	uint32 				fd;
} TimerDeviceType;

static Std_ReturnType timer_get_data8(MpuAddressRegionType *region, CoreIdType core_id, uint32 addr, uint8 *data);
static Std_ReturnType timer_get_data16(MpuAddressRegionType *region, CoreIdType core_id, uint32 addr, uint16 *data);
static Std_ReturnType timer_get_data32(MpuAddressRegionType *region, CoreIdType core_id, uint32 addr, uint32 *data);
static Std_ReturnType timer_put_data8(MpuAddressRegionType *region, CoreIdType core_id, uint32 addr, uint8 data);
static Std_ReturnType timer_put_data16(MpuAddressRegionType *region, CoreIdType core_id, uint32 addr, uint16 data);
static Std_ReturnType timer_put_data32(MpuAddressRegionType *region, CoreIdType core_id, uint32 addr, uint32 data);
static Std_ReturnType timer_get_pointer(MpuAddressRegionType *region, CoreIdType core_id, uint32 addr, uint8 **data);

MpuAddressRegionOperationType	timer_memory_operation = {
		.get_data8 		= 	timer_get_data8,
		.get_data16		=	timer_get_data16,
		.get_data32		=	timer_get_data32,

		.put_data8 		= 	timer_put_data8,
		.put_data16		=	timer_put_data16,
		.put_data32		=	timer_put_data32,

		.get_pointer	= timer_get_pointer,
};
static TimerDeviceType TimerDevice[TAAnChannelNum];
static MpuAddressRegionType *timer_region;

void device_init_timer(MpuAddressRegionType *region)
{
	int i = 0;
	uint16 base;
	uint32 value = 100;

	(void)cpuemu_get_devcfg_value("DEVICE_CONFIG_TIMER_FD", &value);
	//printf("timer value=%d\n", value);

	timer_region = region;

	for (i = 0; i < TAAnChannelNum; i++) {
		TimerDevice[i].cnt = 0;
		TimerDevice[i].state = TIMER_STATE_STOP;
		TimerDevice[i].mode = TIMER_MODE_FREERUN;
		TimerDevice[i].compare0 = 0;
		TimerDevice[i].fd = value;
		TimerDevice[i].start_clock = 0;
	}

	for (i = 0; i < 5; i++) {
		base = 15 + (i * 3);
		TimerDevice[i].compare0_intno = base + 1;
	}
	for (i = 5; i < TAAnChannelNum; i++) {
		base = 96 + ((i - 5) * 3) + 1;
		TimerDevice[i].compare0_intno = base + 1;
	}

	return;
}

static void device_timer_do_update(DeviceClockType *device, int ch)
{
	TimerDeviceType *timer = &(TimerDevice[ch]);

	if (timer->state == TIMER_STATE_STOP) {
		 if (timer->mode == TIMER_MODE_FREERUN) {
			if (timer->start_clock != 0U) {
				timer->start_clock += (uint64)timer->fd;
			}
		}
	}
	else if (timer->state == TIMER_STATE_READY) {
		timer->state = TIMER_STATE_RUNNING;
		if ((timer->start_clock == 0U) || (timer->mode == TIMER_MODE_INTERVAL)) {
			timer->start_clock = device->clock;
		}
		//printf("device_timer_do_update:ch=%d compare=%d\n", ch, timer->compare0);
	}

	timer->cnt = (uint32)((device->clock - timer->start_clock) / (uint64)timer->fd);
	if (timer->mode == TIMER_MODE_INTERVAL) {
		if (timer->cnt == timer->compare0) {
			//printf("raise INT:ch=%d cnt=%d\n", ch, timer->cnt);
			device_raise_int(timer->compare0_intno);
			timer->state = TIMER_STATE_READY;
		}
	}
	return;
}


static void device_timer_do_calc_min_interval(DeviceClockType *device, int ch)
{
	TimerDeviceType *timer = &(TimerDevice[ch]);
	uint64 interval;

	if (device->can_skip_clock == FALSE) {
		return;
	}
	if (timer->cnt >= timer->compare0) {
		return;
	}

	interval = (timer->compare0 - timer->cnt) * timer->fd;

	if ((interval > 0) && (interval < device->min_intr_interval)) {
		device->min_intr_interval = interval;
		//printf("TIMER clock=%I64u min=%I64u\n", device->clock, device->min_intr_interval);
	}
	return;
}

#ifndef CPUEMU_CLOCK_BUG_FIX
#define INLINE_device_supply_clock_timer(dev_clock, ch)	\
do {	\
	if ((dev_clock->clock % TimerDevice[ch].fd) == 0) {	\
		device_timer_do_update(dev_clock, ch);	\
		device_timer_do_calc_min_interval(dev_clock, ch);	\
	}	\
	else {	\
		dev_clock->can_skip_clock = FALSE;	\
	}	\
} while(0)
#else
#define INLINE_device_supply_clock_timer(dev_clock, ch)	\
do {	\
	if ((dev_clock->clock % TimerDevice[ch].fd) == 0) {	\
		device_timer_do_update(dev_clock, ch);	\
		device_timer_do_calc_min_interval(dev_clock, ch);	\
	}	\
} while(0)
#endif /* CPUEMU_CLOCK_BUG_FIX */
void device_supply_clock_timer(DeviceClockType *dev_clock)
{
#ifndef MINIMUM_DEVICE_CONFIG
	INLINE_device_supply_clock_timer(dev_clock, 0);
	INLINE_device_supply_clock_timer(dev_clock, 1);
#endif /* MINIMUM_DEVICE_CONFIG */
	INLINE_device_supply_clock_timer(dev_clock, 2);
	INLINE_device_supply_clock_timer(dev_clock, 3);
	return;
}


static Std_ReturnType timer_get_data8(MpuAddressRegionType *region, CoreIdType core_id, uint32 addr, uint8 *data)
{
	uint32 off = (addr - region->start);
	*data = *((uint8*)(&region->data[off]));
	return STD_E_OK;
}
static Std_ReturnType timer_get_data16(MpuAddressRegionType *region, CoreIdType core_id, uint32 addr, uint16 *data)
{
	uint32 off = (addr - region->start);
	*data = *((uint8*)(&region->data[off]));
	return STD_E_SEGV;
}
static Std_ReturnType timer_get_data32(MpuAddressRegionType *region, CoreIdType core_id, uint32 addr, uint32 *data)
{
	uint8 ch;
	for (ch = 0; ch < TAAnChannelNum; ch++) {
		if (addr == (TAAnCNT(ch) & region->mask)) {
			*data = TimerDevice[ch].cnt;
			return STD_E_OK;
		}
	}
	return STD_E_OK;
}
static Std_ReturnType timer_put_data8(MpuAddressRegionType *region, CoreIdType core_id, uint32 addr, uint8 data)
{
	uint8 ch;
	uint32 off = (addr - region->start);
	*((uint8*)(&region->data[off])) = data;

	//printf("timer_put_data8 core=%d addr=0x%x data=0x%x\n", core_id, addr, data);
	for (ch = 0; ch < TAAnChannelNum; ch++) {
		if (addr == (TAAnCTL0(ch) & region->mask)) {
			if ((data & (1 << 7)) == (1 << 7)) {
				TimerDevice[ch].state = TIMER_STATE_READY;
				//printf("timer%d addr=0x%x ready\n", ch, addr);
			}
			else {
				TimerDevice[ch].state = TIMER_STATE_STOP;
				//TimerDevice[ch].cnt = 0;
				//printf("timer%d addr=0x%x stop\n", ch, addr);
			}
			break;
		}
		else if (addr == (TAAnCTL1(ch) & region->mask)) {
			uint8 mode = (data & 0x07);
			if (mode == 0x00) {
				TimerDevice[ch].mode = TIMER_MODE_INTERVAL;
				//printf("timer%d addr=0x%x interval\n", ch, addr);
			}
			else {
				TimerDevice[ch].mode = TIMER_MODE_FREERUN;
				//printf("timer%d addr=0x%x freerun\n", ch, addr);
			}
			break;
		}
	}
	return STD_E_OK;
}
static Std_ReturnType timer_put_data16(MpuAddressRegionType *region, CoreIdType core_id, uint32 addr, uint16 data)
{
	uint32 off = (addr - region->start);
	*((uint16*)(&region->data[off])) = data;
	return STD_E_OK;
}
static Std_ReturnType timer_put_data32(MpuAddressRegionType *region, CoreIdType core_id, uint32 addr, uint32 data)
{
	uint32 off = (addr - region->start);
	*((uint32*)(&region->data[off])) = data;
	uint8 ch;
	for (ch = 0; ch < TAAnChannelNum; ch++) {
		if (addr == (TAAnCCR0(ch) & region->mask)) {
			TimerDevice[ch].compare0 = data;
			TimerDevice[ch].state = TIMER_STATE_READY;
			//printf("addr=0x%x compare[%d]=%u\n", addr, ch, TimerDevice[ch].compare0);
			break;
		}
	}
	return STD_E_OK;
}
static Std_ReturnType timer_get_pointer(MpuAddressRegionType *region, CoreIdType core_id, uint32 addr, uint8 **data)
{
	uint32 off = (addr - region->start);
	uint8 ch;
	uint32 *cntp;

	for (ch = 0; ch < TAAnChannelNum; ch++) {
		if (addr == (TAAnCNT(ch) & region->mask)) {
			cntp = (uint32*)&region->data[off];
			*cntp = TimerDevice[ch].cnt;
			break;
		}
	}
	*data = &region->data[off];
	return STD_E_OK;
}
