
# Tup with CONFIG_LUPPP_BUILD_TESTS=y

# Compile with:
<<<<<<< HEAD
# define BUILD_TESTS
# define BUILD_COVERAGE_TEST
# -lgcov
=======
#define BUILD_TESTS
#define BUILD_COVERAGE_TEST
#-lgcov
>>>>>>> master


<<<<<<< HEAD
# run the tests:
#  Luppp quits after tests are finished
#  gcov scrapes .gcna / .gcdo files, produces .gcov files
#  cp source files into dir: needed for analysis by lcov
#  lcov scrapes .gcov files into lcov.info
#  genhtml produces index.html from lcov.info
=======
# Run gcov on each file, it shows output data:

cd testBuild/src/

gcov -r -b *.gcda
>>>>>>> master

cd testBuild/

<<<<<<< HEAD
./luppp

cd src
=======

#rm src/*.gcda
#rm src/*.gcno

#rm src/lcov.info
#rm src/gcov.css
#rm src/*.html
#rm src/*.png

#rm gcov.data
>>>>>>> master

gcov -r -b *
cp -r ../../src/* ./
lcov --directory . --capture --output-file lcov.info
genhtml lcov.info

firefox index.html
