

git archive HEAD --format=zip > lupppGit_`date +"%m-%d-%Y"`_`git log --pretty=format:"%h" | head -n1`.zip
