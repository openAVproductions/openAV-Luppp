rm FIXME

echo -e "\n===> Samplerate hardcoded:\n" > FIXME 
grep -irn "44100" src/ >> FIXME

echo -e "\n\n\n===> FIXME" >> FIXME 
grep -irn "FIXME" src/ >> FIXME

echo -e "\n\n\n===> TODO" >> FIXME 
grep -irn "TODO" src/ >> FIXME 
