#!/bin/bash
tail -n 9999999 */allstats.txt | grep -f stataccept.txt 

