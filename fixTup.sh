#!/bin/bash

# sometimes Tup gives errors on FUSE mount or something
# just deleting .tup/mnt/* makes tup re-mount things, fixing it:

rm -rf .tup/mnt/*
rm -rf build/src/*
rm -rf buildTest/src/*
