#!/bin/bash
tail -n 1000 */allsols.txt | grep -v -e-- | grep -v S
#cat */allsols.txt 
# grep : | sort | uniq

