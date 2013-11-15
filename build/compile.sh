#!/bin/bash

# A simple script to invoke cmake with the correct parameters to build the
# release version of Luppp

function err_handle {
  notify-send -t 0 -u critical "Luppp: Release build FAILED!"
}
trap 'err_handle' ERR

set -e

cd ../
./version.sh
cd build

cmake -DRELEASE_BUILD=1 ../

make -j 2

