#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include "debug.h"

#define U8 unsigned char
#define S16 signed short

#define PWM_CHIP_PATH "/sys/class/pwm/pwmchip0/"
#define PWM_EXPORT_PATH PWM_CHIP_PATH "export"
#define PWM_UNEXPORT_PATH PWM_CHIP_PATH "unexport"

#define PWM_PIN "pwm0"
#define PWM_PIN_PATH PWM_CHIP_PATH PWM_PIN "/"
#define PWM_PERIOD_PATH PWM_PIN_PATH "period"
#define PWM_DUTY_CYCLE_PATH PWM_PIN_PATH "duty_cycle"
#define PWM_ENABLE_PATH PWM_PIN_PATH "enable"



void write2File(const char *path, const char *value) {
	APP_DDD("path: %s , value: %s", path, value);
    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        perror("Failed to open file for writing");
        //exit(EXIT_FAILURE);
    }
    if (write(fd, value, strlen(value)) < 0) {
        perror("Failed to write to file");
        //exit(EXIT_FAILURE);
    }
    close(fd);
}

static void export_pwm(const char *path, int pwm_no)
{
	char pwm_path[128]={0};
	char pwm_pin[4]={0};
	snprintf(pwm_path, sizeof(pwm_path), "%s/export", path);
	snprintf(pwm_pin, sizeof(pwm_pin), "%d", pwm_no);
	write2File(pwm_path, pwm_pin);
}

static void unexport_pwm(const char *path, int pwm_no)
{
	char pwm_path[128]={0};
	char pwm_pin[4]={0};
	snprintf(pwm_path, sizeof(pwm_path), "%s/unexport", path);
	snprintf(pwm_pin, sizeof(pwm_pin), "%d", pwm_no);
	write2File(pwm_path, pwm_pin);
}

static void set_pwm_period(const char *path, uint32_t period)
{
	char pwm_path[128]={0};
	char cPeriod[16]={0};
	snprintf(pwm_path, sizeof(pwm_path), "%s/period", path);
	snprintf(cPeriod, sizeof(cPeriod), "%u", period);
	
	APP_DDD("set pwm period: %u", period);
	write2File(pwm_path, cPeriod);
}

static void set_pwm_dutyCycle(const char *path, uint32_t duty_cycle)
{
	char pwm_path[128]={0};
	char cDutyCycle[16]={0};
	snprintf(pwm_path, sizeof(pwm_path), "%s/duty_cycle", path);
	snprintf(cDutyCycle, sizeof(cDutyCycle), "%u", duty_cycle);
	
	APP_DDD("set pwm duty cycle: %u", duty_cycle);
	write2File(pwm_path, cDutyCycle);
}

static void enable_pwm(const char *path)
{
	char pwm_path[128]={0};
	snprintf(pwm_path, sizeof(pwm_path), "%s/enable", path);
	write2File(pwm_path, "1");
}

static void disable_pwm(const char *path)
{
	char pwm_path[128]={0};
	snprintf(pwm_path, sizeof(pwm_path), "%s/enable", path);
	write2File(pwm_path, "0");
}


static void init_esc(int pwm_no)
{
	// export
	export_pwm(PWM_CHIP_PATH, pwm_no);
	usleep(1000);
	
	// TBD - check pwm0 be create?
	
	// set period
	set_pwm_period(PWM_PIN_PATH, 2000000);
	
	// set duty_cycle 0
	set_pwm_dutyCycle(PWM_PIN_PATH, 0);
	
	// enable pwm
	enable_pwm(PWM_PIN_PATH);
	
	// set duty_cycle max_duty
	set_pwm_dutyCycle(PWM_PIN_PATH, 2000000);
	
	// delay...
	usleep(2000);
	
	// set duty_cycle min_duty
	set_pwm_dutyCycle(PWM_PIN_PATH, 700000);
	
	// delay...
	usleep(1000);
}

static void exit_esc(int pwm_no)
{
	disable_pwm(PWM_PIN_PATH);
	usleep(1000);
	unexport_pwm(PWM_CHIP_PATH, pwm_no);
	usleep(1000);
}

static void signal_handle(int dunno)
{
	if((SIGINT == dunno) || (SIGKILL == dunno))
	{
		exit_esc(0);
	}
}

int main(int argc, char **argv)
{
	int exit = 0;
	char input;
	const int def_duty=1600000;
	int step=0, cur_duty=0;
	signal(SIGINT, signal_handle);
	
	init_esc(0);
	
	while(!exit){
		scanf("%c",&input);
		printf("input = %c", input);
		switch (input){
			case 'i':
				init_esc(0);
				break;
			case 'q':
				exit_esc(0);
				exit=1;
				break;
			case 'b':
				cur_duty = def_duty;
				step = 2000000/1000;/* step = period/1000 */
				set_pwm_dutyCycle(PWM_PIN_PATH, cur_duty);
				break;
			case 'w':
				cur_duty += step*3;
				set_pwm_dutyCycle(PWM_PIN_PATH, cur_duty);
				break;
			case 's':
				cur_duty -= step*3;
				set_pwm_dutyCycle(PWM_PIN_PATH, cur_duty);
				break;
			case 'd':
				cur_duty += step;
				set_pwm_dutyCycle(PWM_PIN_PATH, cur_duty);
				break;
			case 'a':
				cur_duty -= step;
				set_pwm_dutyCycle(PWM_PIN_PATH, cur_duty);
				break;
			default:
				break;
		}
	};
	
	printf("done");
	

	return 0;
}
