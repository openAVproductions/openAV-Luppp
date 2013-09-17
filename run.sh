#!/bin/bash

set -e
xterm -e "sleep 1 && aj-snapshot -r .ajsnapshot" & bin/luppp
