
# Tup with CONFIG_LUPPP_BUILD_TESTS=y

# Compile with:
#define BUILD_TESTS
#define BUILD_COVERAGE_TEST
#-lgcov

# Run program, the runtime determines the output!

# Run gcov on each file, it shows output data:

cd testBuild/src/

gcov -r -b *.gcda

# Make lcov update the stats in its info file
lcov --directory . -zerocounters
lcov --directory . --capture --output-file lcov.info

# Generate HTML output
genhtml lcov.info


#rm src/*.gcda
#rm src/*.gcno

#rm src/lcov.info
#rm src/gcov.css
#rm src/*.html
#rm src/*.png

#rm gcov.data


