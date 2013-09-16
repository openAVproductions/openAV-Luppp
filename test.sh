
# Tup with CONFIG_LUPPP_BUILD_TESTS=y

# Compile with:
# define BUILD_TESTS
# define BUILD_COVERAGE_TEST
# -lgcov

# run the tests:
#  Luppp quits after tests are finished
#  gcov scrapes .gcna / .gcdo files, produces .gcov files
#  cp source files into dir: needed for analysis by lcov
#  lcov scrapes .gcov files into lcov.info
#  genhtml produces index.html from lcov.info

#set -e


FILE=buildTest/build.success

rm -f $(FILE)

tup upd buildTest/ ;


if [ ! -f $FILE ]; then
    echo "File not found!"
fi

if [ -f $FILE ];
then
   echo "File $FILE exists."
else
   notify-send -t 0 --urgency=critical "Luppp: Compilation FAILURE!"
   exit
fi

sleep 1

cd buildTest/

./luppp

if [ $? -eq 0 ]; then
    notify-send -t 5 "Luppp: Tests passed successfully..."
    echo OK
else
    notify-send -t 5 -u critical "Luppp: Test FAILURE!"
    echo FAIL
fi

cd src

gcov -r -b *
cp -r ../../src/* ./
lcov --directory . --capture --output-file lcov.info
genhtml lcov.info

notify-send -t 5 "Luppp: Test data available..."
#firefox src/index.html
