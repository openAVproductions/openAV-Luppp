#!/bin/bash

./version.sh
git archive HEAD --format=zip > lupppGit_`date +"%d-%m-%Y"`_`git log --pretty=format:"%h" | head -n1`.zip
