#!/bin/bash

set -e
tup upd
xterm -e "sleep 1 && aj-snapshot -r .ajsnapshot" & releaseBuild/luppp
