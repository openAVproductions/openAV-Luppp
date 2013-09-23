#!/bin/bash

# A simple script to invoke cmake with the correct parameters to build the
# self-testing version of Luppp

function err_handle {
  notify-send -t 0 -u critical "Luppp: Self-test FAILED!"
}
trap 'err_handle' ERR

set -e

# setup environment: Copy material for tests to /tmp
cp -r ../src/tests/lupppTestMaterial /tmp

cmake -DBUILD_TESTS=1 ../

make -j 2

lcov --zerocounters --directory .

lcov --capture --initial --directory . --output-file app


./src/luppp -test -stopAfterTest

lcov --base-directory . --directory . -c -o lupppFull.info

lcov --remove lupppFull.info "/usr*" --no-checksum --directory .  --output-file lupppTmp.info

lcov --remove lupppTmp.info "*.hxx" --no-checksum --directory .  --output-file luppp.info

genhtml luppp.info

notify-send -t 5 "Luppp: Test data available..."
firefox index.html
