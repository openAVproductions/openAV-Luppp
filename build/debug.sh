#!/bin/bash

set -e

xterm -e "sleep 3 && aj-snapshot -r lupppDemo" & cd src/ && gdb luppp
