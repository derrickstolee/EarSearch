#!/bin/bash
tail -n 15 */log*
condor_status -submitters
