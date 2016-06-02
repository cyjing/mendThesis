#!/bin/bash
cpufreq-set -g performance -c 0
cpufreq-set -g performance -c 1
cpufreq-set -g performance -c 2
cpufreq-set -g performance -c 3
cpufreq-set -g performance -c 4
cpufreq-set -g performance -c 5
cpufreq-set -g performance -c 6
cpufreq-set -g performance -c 7
watch grep \"cpu MHz\" /proc/cpuinfo
