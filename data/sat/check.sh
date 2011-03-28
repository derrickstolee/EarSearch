#!/bin/bash
tail -n 30 */log*
condor_status -submitters
