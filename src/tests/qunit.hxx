// QUnit.hpp - a simple unit test framework for C++
// 
// Original: svn r26
// Updated to coloured printing by Harry van Haaren <harryhaaren@gmail.com>
// 
// Typical usage:
// 
// #include "QUnit.hpp"
// 
// int main() {
//   QUnit::UnitTest qunit(std::cerr, QUnit::verbose);
// 
//   QUNIT_IS_TRUE(true);
//   QUNIT_IS_FALSE(4!=4);
//   QUNIT_IS_EQUAL(42, 42.0);
//   QUNIT_IS_NOT_EQUAL(42,"43");
// 
//   return qunit.errors();
// }
//

#ifndef _QUNIT_HPP_
#define _QUNIT_HPP_

#include <sstream>
#include <string>
#include <iostream>
using namespace std;

#define QUNIT_COLOUR_PASS  "\033[1;32m"
#define QUNIT_COLOUR_ERROR "\033[1;31m"
#define QUNIT_COLOUR_RESET "\033[0m"

#define QUNIT_IS_EQUAL(expr1,expr2)     QUNIT_COMPARE(true,true,expr1,expr2)
#define QUNIT_IS_NOT_EQUAL(expr1,expr2) QUNIT_COMPARE(true,false,expr1,expr2)
#define QUNIT_IS_TRUE(expr)             QUNIT_COMPARE(false,true,expr,true)
#define QUNIT_IS_FALSE(expr)            QUNIT_COMPARE(false,true,expr,false)

#define QUNIT_COMPARE(compare,result,expr1,expr2) {                 \
        std::stringstream s1, s2;                                   \
        s1 << std::boolalpha << (expr1);                            \
        s2 << std::boolalpha << (expr2);                            \
        qunit.evaluate(                                             \
            compare, result, s1.str(), s2.str(), #expr1, #expr2,    \
            __FILE__, __LINE__, __FUNCTION__ );                     \
    };                                                              \

namespace QUnit {

    enum { silent, quiet, normal, verbose, noisy };
      
    class UnitTest {
        
    public:
        UnitTest( int verboseLevel );
        ~UnitTest();
          
        void verboseLevel(int level);
        int  verboseLevel();

        void printStatus();
        
        int  errors() const;
          
        void evaluate(bool, bool,
                      std::string, std::string, std::string, std::string,
                      const char *, int, const char *);

    private:
        int verboseLevel_;
        int errors_;
        int tests_;
    };
  
    inline UnitTest::UnitTest( int verboseLevel)
        : verboseLevel_(verboseLevel) , errors_(0) , tests_(0) {
    }
  
    inline UnitTest::~UnitTest() {
        if ( verboseLevel_ > quiet ) 
            printStatus();
    }
  
    inline void UnitTest::verboseLevel(int level) {
        verboseLevel_ = level;
    }

    inline int UnitTest::verboseLevel() {
        return verboseLevel_;
    }

    inline void UnitTest::printStatus() {
        if ( errors_ ) {
          cout << QUNIT_COLOUR_ERROR;
        } else {
          cout << QUNIT_COLOUR_PASS;
        }
        cout << "Testing " << ( errors_ ? "FAILED" : "OK" ) << " ("
             << tests_ << " tests, " << ( tests_ - errors_ ) << " ok, "
             << errors_ << " failed)" << QUNIT_COLOUR_RESET << std::endl;
    }
  
    inline int UnitTest::errors() const {
        return errors_;
    }
  
    inline void UnitTest::evaluate(
        bool compare, bool result,
        std::string val1, std::string val2, 
        std::string str1, std::string str2,
        const char * file, int line, const char * func)
    {

        bool ok = result ? (val1 == val2) : (val1 != val2);
        tests_ += 1;
        errors_ += ok ? 0 : 1;

        if( (ok && !(verboseLevel_ > normal)) || verboseLevel_ == silent )
            return;
  
        cout << file << ( ok ? ";" : ":" ) << line << ": ";
        if ( ok ) {
          cout << QUNIT_COLOUR_PASS;
        } else {
          cout << QUNIT_COLOUR_ERROR;
        }
        cout << ( ok ? "OK" : "FAILED" ) << QUNIT_COLOUR_RESET << "/" << func << "(): ";
        if( compare ) {
            const std::string cmp = ( result ? "==" : "!=" );
            cout << "compare {" << str1 << "} " << cmp << " {" <<  str2 << "} "
                 << "got {\"" << val1 << "\"} " << cmp << " {\"" << val2 << "\"}";
        } else {
            cout << "evaluate {" << str1 << "} == " << val1;
        }
        cout << std::endl;
    }
}

#endif // _QUNIT_HPP_
