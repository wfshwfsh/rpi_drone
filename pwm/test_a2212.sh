#!/bin/bash

function init_pwm()
{
    sudo echo 0 > /sys/class/pwm/pwmchip0/export
}

function deinit_pwm()
{
    echo 0 > /sys/class/pwm/pwmchip0/unexport
}

function set_pwm_period()
{
    echo $1 > /sys/class/pwm/pwmchip0/pwm0/period
}

function set_pwm_duty()
{
    local _duty=$1
    echo "duty = $_duty"
    echo $_duty > /sys/class/pwm/pwmchip0/pwm0/duty_cycle
}

function enable_pwm()
{
    echo 1 > /sys/class/pwm/pwmchip0/pwm0/enable
}

function disable_pwm()
{
    echo 0 > /sys/class/pwm/pwmchip0/pwm0/enable
}


#####
init_pwm
set_pwm_period 2000000
enable_pwm
set_pwm_duty 0
sleep 1
set_pwm_duty 2000000
sleep 0.2
set_pwm_duty 700000
sleep 0.1
#set_pwm_duty 0

sleep 10
echo "ready"
set_pwm_duty 1650000

sleep 3
disable_pwm
deinit_pwm
