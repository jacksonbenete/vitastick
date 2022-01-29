#include <stdio.h>
#include <psp2/ctrl.h>
#include <psp2/power.h>
#include "vitastick_uapi.h"
#include "debugScreen.h"

#define printf(...) psvDebugScreenPrintf(__VA_ARGS__)

static void wait_key_press();
static void set_default_clock();
static void set_lower_clock();

int main(int argc, char *argv[])
{
	int ret;

	psvDebugScreenInit();

	printf("vitastick by xerpi\n");

	ret = vitastick_start();
	if (ret >= 0) {
		printf("vitastick started successfully!\n");
	} else if (ret == VITASTICK_ERROR_DRIVER_ALREADY_ACTIVATED) {
		printf("vitastick is already active!\n");
	} else if (ret < 0) {
		printf("Error vitastick_start(): 0x%08X\n", ret);
		wait_key_press("X", SCE_CTRL_CROSS);
		return -1;
	}

    set_lower_clock();

	wait_key_press("START + SELECT", SCE_CTRL_START | SCE_CTRL_SELECT);

    set_default_clock();

	vitastick_stop();

	return 0;
}

void wait_key_press(const char *key_desc, unsigned int key_mask)
{
	SceCtrlData pad;
    int ret;
    unsigned int lower_clock_mask, default_clock_mask, reload_vitastick_mask;

    lower_clock_mask = SCE_CTRL_SELECT | SCE_CTRL_VOLDOWN;
    default_clock_mask = SCE_CTRL_SELECT | SCE_CTRL_VOLUP;
    reload_vitastick_mask = SCE_CTRL_SELECT | SCE_CTRL_TRIANGLE;

	printf("Press %s to exit.\n", key_desc);

	while (1) {
		sceCtrlReadBufferPositive(0, &pad, 1);

        // lower clock
        if ((pad.buttons & lower_clock_mask) == lower_clock_mask)
        {
            printf("reducing clock...\n");
            set_lower_clock();
            printf("clock reduced [111,111,111,111]\n");
        }
        // default clock
        if ((pad.buttons & default_clock_mask) == default_clock_mask)
        {
            printf("recovering default clock...\n")
            set_default_clock();
            printf("clock recovered [266,166,166,111]\n")
        }
        // reload app
        if ((pad.buttons & reload_vitastick_mask) == reload_vitastick_mask)
        {
            vitastick_stop();
            ret = vitastick_start();
            if (ret >= 0) printf("vitastick started successfully!\n");
        }

		if ((pad.buttons & key_mask) == key_mask)
			break;
		sceKernelDelayThreadCB(200 * 1000);
	}
}

void set_default_clock()
{
    scePowerSetArmClockFrequency(266);
    scePowerSetBusClockFrequency(166);
    scePowerSetGpuClockFrequency(166);
    scePowerSetGpuXbarClockFrequency(111);
}

void set_lower_clock()
{
    scePowerSetArmClockFrequency(111);
    scePowerSetBusClockFrequency(111);
    scePowerSetGpuClockFrequency(111);
    scePowerSetGpuXbarClockFrequency(111);
}
