#!/bin/bash

set -e
tup upd
<<<<<<< HEAD
xterm -e "sleep 1 && aj-snapshot -r .ajsnapshot" & releaseBuild/luppp
=======
xterm -e "sleep 1 && aj-snapshot -r .ajsnapshot" & ./releaseBuild/luppp
>>>>>>> master
