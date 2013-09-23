rm FIXME

echo -e "\n===> Samplerate hardcoded:\n" > FIXME 
grep -irnI "44100" src/ >> FIXME

echo -e "\n\n\n===> FIXME" >> FIXME 
grep -irnI "FIXME" src/ >> FIXME

echo -e "\n\n\n===> TODO" >> FIXME 
grep -irnI "TODO" src/ >> FIXME 

echo -e "\n\n\n===> Prints" >> FIXME 
grep -irnI "cout" src/ >> FIXME 

geany FIXME
