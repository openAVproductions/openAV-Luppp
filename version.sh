#!/bin/bash

# this file writes src/version.h from the git commit
echo "
#ifndef GIT_VERSION
#define GIT_VERSION \"`date +\"%d-%m-%Y\"` `git log --pretty=format:"%h" | head -n1`\"
#endif" > src/version.hxx
