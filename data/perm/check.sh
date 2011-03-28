#!/bin/bash
tail -n 20 */log*
condor_status -submitters
