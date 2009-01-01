/*
* Copyright (c) 2003-2009 Rony Shapiro <ronys@users.sourceforge.net>.
* All rights reserved. Use of the code is allowed under the
* Artistic License 2.0 terms, as specified in the LICENSE file
* distributed with this code, or available from
* http://www.opensource.org/licenses/artistic-license-2.0.php
*/
#define TEST_TWOFISH
#define TEST_SHA256
#define TEST_HMAC_SHA256
#define TEST_STRINGX

#ifdef TEST_TWOFISH
#include "TwoFishTest.h"
#endif
#ifdef TEST_SHA256
#include "SHA256Test.h"
#endif
#ifdef TEST_HMAC_SHA256
#include "HMAC_SHA256Test.h"
#endif
#ifdef TEST_STRINGX
#include "StringXTest.h"
#endif
#include <iostream>
using namespace std;

int main()
{
#ifdef TEST_MYSTRING
  CMyStringTest t1;
  t1.setStream(&cout);
  t1.run();
  t1.report();
#endif
#ifdef TEST_TWOFISH
  CTwoFishTest t2;
  t2.setStream(&cout);
  t2.run();
  t2.report();
#endif
#ifdef TEST_SHA256
  CSHA256Test t3;
  t3.setStream(&cout);
  t3.run();
  t3.report();
#endif
#ifdef TEST_HMAC_SHA256
  CHMAC_SHA256Test t4;
  t4.setStream(&cout);
  t4.run();
  t4.report();
#endif
#ifdef TEST_STRINGX
  StringXTest t5;
  t5.setStream(&cout);
  t5.run();
  t5.report();
#endif
  return 0;
}
