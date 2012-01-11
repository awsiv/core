#include "cf3.defs.h"
#include "cf3.extern.h"

#include "logging.h"

#include <setjmp.h>
#include <cmockery.h>

static Sockaddr *got_address;

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
               const Sockaddr *dest_addr, socklen_t addrlen)
{
got_address = xmemdup(dest_addr, sizeof(Sockaddr_in));
return len;
}

static void test_set_port(void **state)
{
SetSyslogPort(5678);
RemoteSysLog(LOG_EMERG, "Test string");

if (got_address->sa_family == AF_INET)
   {
   assert_int_equal(ntohs(((Sockaddr_in*)got_address)->sin_port), 5678);
   }
else if (got_address->sa_family == AF_INET6)
   {
   assert_int_equal(ntohs(((Sockaddr_in6*)got_address)->sin6_port), 5678);
   }

free(got_address);
}

static void test_set_host(void **state)
{
SetSyslogHost("127.0.0.55");
RemoteSysLog(LOG_EMERG, "Test string");

assert_int_equal(got_address->sa_family, AF_INET);

assert_int_equal(ntohl(((Sockaddr_in*)got_address)->sin_addr.s_addr), 0x7f000037);
}

int main()
{
const UnitTest tests[] =
   {
   unit_test(test_set_port),
   unit_test(test_set_host),
   };

return run_tests(tests);
}
