#!/bin/bash

set -e
tup upd
gdb --dir=src/ ./luppp
