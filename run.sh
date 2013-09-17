#!/bin/bash

set -e
make && xterm -e "sleep 1 && aj-snapshot -r .ajsnapshot" & bin/luppp
