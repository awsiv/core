dnl ##########################################################################
dnl #
dnl # Process this file with autoconf then autoheader to build a configure
dnl # script => aclocal; automake -a -c; autoconf
dnl #
dnl ##########################################################################

AC_PREREQ(2.52)

AC_INIT(src/cfnova.c)
AC_CANONICAL_TARGET

dnl
dnl The version in the next line is the only one to set
dnl

AM_INIT_AUTOMAKE(cfnova,1.0.5)   dnl remember to set version

AM_CONFIG_HEADER(src/conf.h)

#
# Add to the default list of places in LDFLAGS to compensate for
# ... the configure default value of LIBS on some systems
#
for x in /usr/lib64 /usr/local/gnu/lib /usr/local/gnulib /opt/dce/lib /sw/lib
do
  if test -d "$x"; then
    y=`expr " $LDFLAGS " : ".* -L$x "`
    if test $y -eq 0; then
      LDFLAGS="$LDFLAGS -L$x"
    fi
  fi
done

#
# Add to the default list of places in CPPFLAGS to match LDFLAGS above
# 
for x in /usr/local/gnu/include /opt/dce/include /sw/include
do 
  if test -d "$x"; then
    y=`expr " $CPPFLAGS " : ".* -I$x "`
    if test $y -eq 0; then
      CPPFLAGS="$CPPFLAGS -I$x"
    fi
  fi
done

ACX_PTHREAD
CC=$PTHREAD_CC
CFLAGS="$PTHREAD_CFLAGS $CFLAGS"
CPPFLAGS="$PTHREAD_CPPFLAGS $CPPFLAGS"
LDFLAGS="$PTHREAD_LDFLAGS $LDFLAGS"

dnl ######################################################################
dnl Checks for programs.
dnl ######################################################################

AC_PROG_CC
AC_PROG_RANLIB
AC_DISABLE_SHARED
dnl AC_DISABLE_STATIC - maybe use shared libs later
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PATH_PROG(GETCONF, getconf, false, $PATH:$prefix/bin:/usr/bin:/usr/local/bin:/sw/bin)

dnl ######################################################################
dnl Checks for libraries.
dnl ######################################################################

dnl AC_CHECK_LIB(m,main)

AC_CHECK_FUNC(door_create,door)
AC_CHECK_FUNC(setsockopt, , AC_CHECK_LIB(socket, setsockopt))
AC_CHECK_FUNC(gethostent, , AC_CHECK_LIB(nsl, gethostent))
AC_CHECK_FUNC(getaddrinfo, , AC_CHECK_LIB(socket, getaddrinfo))

dnl AC_SEARCH_LIBS(socket, socket, ,
dnl    [AC_CHECK_LIB(nsl, socket, LIBS="$LIBS -lsocket -lnsl", , -lsocket)])

dnl ######################################################################
dnl  Look for SQL connectors.
dnl ######################################################################

AC_ARG_WITH(sql,
    [  --with-sql[=PATH] directory where MySQL/Postgress exists],
    WITH_SQL=1,
    WITH_SQL=0)

dnl ######################################################################
dnl Checks for perl compatible regular expressions
dnl ######################################################################

AC_ARG_WITH(pcre,
    [  --with-pcre[[=PATH]] directory where Perl compatible regex exists (optionally in PATH)],
    [
        AC_MSG_CHECKING(pcre)

        if test "x$withval" != "xno" ; then
            if test "x$withval" != "xyes" ; then
                if test "x$withval" = "xdefault" ; then
                    for d in $prefix /usr/local /usr; do
                        if test -f "$d/include/pcreposix.h" ; then
                            CPPFLAGS="$CPPFLAGS -I$d/include"
                            LDFLAGS="$LDFLAGS -L$d/lib"
                            PCRE_LIB="-lpcreposix"
                            AC_MSG_RESULT($d)
                            break
                        fi
                    done
                    if test x"$PCRE_LIB" = x ; then
                        AC_MSG_ERROR(Cannot find PCRE)
                    fi
                else
                if test -f "${withval}/include/pcreposix.h" ; then
                    CPPFLAGS="$CPPFLAGS -I${withval}/include"
                    LDFLAGS="$LDFLAGS -L${withval}/lib"
                    AC_MSG_RESULT("${withval}")
                else
                    AC_MSG_ERROR(Cannot find PCRE)
                fi
                fi
            else
               AC_MSG_RESULT("yes")
            fi
            LIBS="$LIBS -lpcreposix"
            AC_DEFINE(USE_PCRE, 1, [Define if PCRE is available.])
        fi
    ]
)


dnl ######################################################################
dnl Checks for regular expressions
dnl GNU regex lib if you have regcomp in libc you should NOT have this lib too
dnl ######################################################################

dnl AC_CHECK_LIB(pcre, main)
dnl AC_CHECK_LIB(rx, main) deprecated
dnl AC_CHECK_LIB(rt, main)

dnl Now check for database connectors

dnl if test $WITH_SQL; then
dnl  AC_CHECK_LIB(mysqlclient,main)
dnl  AC_CHECK_LIB(pq,main)
dnl  AC_CHECK_HEADERS(mysql/mysql.h)
dnl  AC_CHECK_HEADERS(pgsql/libpq-fe.h)
dnl  AC_CHECK_HEADERS(libpq-fe.h)
dnl fi

AC_CHECK_HEADERS(ldap.h)
AC_CHECK_LIB(ldap,ldap_get_values_len)

dnl ######################################################################
dnl Checks for header files.
dnl ######################################################################

AC_CHECK_HEADERS(unistd.h stdlib.h sys/loadavg.h)
AC_CHECK_HEADERS(sys/mount.h)
AC_CHECK_HEADERS(utime.h)
AC_CHECK_HEADERS(time.h)
AC_CHECK_HEADERS(sys/time.h)
AC_CHECK_HEADERS(malloc.h sys/malloc.h)
AC_CHECK_HEADERS(vfs.h)
AC_CHECK_HEADERS(sys/vfs.h)
AC_CHECK_HEADERS(sys/sockio.h)
AC_CHECK_HEADERS(sys/statvfs.h)
AC_CHECK_HEADERS(sys/statfs.h)
AC_CHECK_HEADERS(sys/param.h)
AC_CHECK_HEADERS(fcntl.h)
AC_CHECK_HEADERS(sys/filesys.h)
AC_CHECK_HEADERS(dustat.h)
AC_CHECK_HEADERS(regex.h rxposix.h)
AC_CHECK_HEADERS(sys/systeminfo.h)
AC_CHECK_HEADERS(sys/acl.h)
AC_CHECK_HEADERS(zone.h)

AC_HEADER_STDC
AC_HEADER_TIME
AC_HEADER_SYS_WAIT
AC_HEADER_DIRENT

dnl ######################################################################
dnl Checks for graph support
dnl ######################################################################

AC_CHECK_HEADERS(graphviz/gvc.h)
AC_CHECK_LIB(gd,main)
AC_CHECK_HEADERS(gd.h)

dnl ######################################################################
dnl Checks for data types
dnl ######################################################################

AC_TYPE_MODE_T
AC_TYPE_SIZE_T
AC_TYPE_UID_T
AC_TYPE_PID_T
AC_CHECK_TYPES(clockid_t)

dnl ######################################################################
dnl Checks for typedefs, structures, and compiler characteristics.
dnl ######################################################################

AC_C_CONST

dnl ######################################################################
dnl Check for special functions
dnl ######################################################################

AC_CHECK_FUNCS(getcwd getnetgrent waitpid seteuid setegid setreuid setregid)
AC_CHECK_FUNCS(uname gethostname realpath regcomp chflags)
AC_CHECK_FUNCS(strstr strsep putenv drand48 srand48 getaddrinfo)
AC_CHECK_FUNCS(bcopy mkfifo statfs statvfs door)
AC_CHECK_FUNCS(sysinfo setsid strdup strrchr strerror snprintf sysconf)
AC_CHECK_FUNCS(getzoneid,getzonenamebyid)

dnl ######################################################################
dnl Check for sa_len in struct sockaddr
dnl ######################################################################

AC_CHECK_MEMBERS([struct sockaddr.sa_len], , , [
#include <sys/types.h>
#include <sys/socket.h>])
   
dnl #######################################################################
dnl Newer BSD systems don't have a compatible rtentry - use ortentry
dnl #######################################################################
 
rtry=none
AC_MSG_CHECKING(for either struct rtentry or struct ortentry)
AC_EGREP_HEADER(rtentry, net/route.h, rtry=rtentry)
if test "$rtry" = rtentry; then
 AC_DEFINE(HAVE_RTENTRY)
fi
AC_EGREP_HEADER(ortentry, net/route.h, rtry=ortentry)
if test "$rtry" = ortentry; then
 AC_DEFINE(HAVE_ORTENTRY)
fi
AC_MSG_RESULT([$rtry])

dnl #######################################################################
dnl Large File Support
dnl #######################################################################

lfsresult=no
AC_MSG_CHECKING(whether to compile with large file support)
if test "$GETCONF" != "false"; then
	lfscflags=`$GETCONF LFS_CFLAGS 2>/dev/null`
	if test -n "$lfscflags"; then
		CFLAGS="$CFLAGS $lfscflags"
		lfsresult=yes
	fi
	lfsldflags=`$GETCONF LFS_LDFLAGS 2>/dev/null`
	test -n "$lfsldflags" && LDFLAGS="$LDFLAGS $lfsldflags"
fi
AC_MSG_RESULT([$lfsresult])

dnl #######################################################################
dnl Handle DCE + threading stuff
dnl #######################################################################

dnl AC_CHECK_LIB(pthread,main)         dnl POSIX threads

if test "$ac_cv_lib_pthread_main" = "yes"; then
	AC_CHECK_HEADERS(pthread.h)
	AC_CHECK_HEADERS(sched.h)
fi



AC_CHECK_FUNC(lchown, AC_DEFINE(HAVE_LCHOWN))


dnl ######################################################################
dnl Give the chance to enable SELINUX
dnl ######################################################################
dnl 
AC_ARG_ENABLE(selinux, dnl 
               [  --enable-selinux              Enable use of the SELINUX libraries],
               [AC_DEFINE(WITH_SELINUX, 1, [Define if you want to use SELINUX])
               LIB_SELINUX="-lselinux"
               LIBS="$LIBS $LIB_SELINUX"
               AC_SUBST(LIB_SELINUX)])


dnl ######################################################################
dnl OS specific stuff
dnl ######################################################################

dnl To ensure conf.h is picked up via VPATH
if test "$srcdir" != "."; then
  CPPFLAGS="$CPPFLAGS -I`pwd`/src"
fi

case "$target_os" in

   sunos3*)
	AC_DEFINE(SUN3)
        ;;
   sunos4*)
	AC_DEFINE(SUN4)
        ;;
   solaris2.4*)
	AC_DEFINE(SOLARIS)
        AC_DEFINE(HAVE_SYS_ACL_H,)
        CPPFLAGS="-w $CPPFLAGS"
        LIBS="$LIBS -lelf"
        ;;
   solaris2.5*)
	AC_DEFINE(SOLARIS)
        AC_DEFINE(_POSIX_C_SOURCE)
        AC_DEFINE(__EXTENSIONS__)
        LIBS="$LIBS -lelf -lsec"
        ;;
   solaris2*)
	AC_DEFINE(SOLARIS)
        AC_DEFINE(__BIT_TYPES_DEFINED__) # avoid conflict with db.h

       AC_PREPROC_IFELSE([
                         AC_LANG_PROGRAM([[
#define _POSIX_C_SOURCE 1
#include <unistd.h>
                           ]], [])],
                           [AC_DEFINE(_POSIX_C_SOURCE)],
                           [AC_DEFINE(_POSIX_C_SOURCE, 200112loL)])
        AC_DEFINE(__EXTENSIONS__)
        LIBS="$LIBS -lelf -lsec"
        ;;
   ultrix*)
	AC_DEFINE(ULTRIX)
        ;;
   hpux*|hp-ux*) 
	AC_DEFINE(HPuUX)
        if test "$GCC" != "yes"; then
	  AC_DEFINE(REGEX_MALLOC)
        fi

        LIBS="$LIBS -lc"
	AC_CHECK_LIB(PW, main)
        ;;
   aix*)
	AC_DEFINE(AIX)
        CPPFLAGS="$CPPFLAGS -w"
	AC_CHECK_LIB(pthreads, main)
        AC_CHECK_HEADER(pthreads.h)
        ;;
   osf*)
	AC_DEFINE(OSF)
	if test "$GCC" = yes; then
	   AC_MSG_WARN([pthreads may not work with GCC under Tru64])
	   AC_MSG_WARN([If you get build errors mentioning PTHREADDEFAULTS etc.,])
	   AC_MSG_WARN([re-configure with CC=cc.])
	fi
        ;;
   irix6*)
	# rtentry is detected OK on a 6.5.19 system.
        AC_DEFINE(HAVE_ORTENTRY) # Have to hack this for 6.* owing to bug
	AC_DEFINE(IRIX)
        CFLAGS="$CFLAGS -w"
        ;;
   irix4*)
	AC_DEFINE(IRIX)
        CFLAGS="$CFLAGS -w"
        LIBS="$LIBS -lsun"
        ;;
   irix*)
	AC_DEFINE(IRIX)
        CFLAGS="$CFLAGS -w"
        ;;
   linux*)
	AC_DEFINE(LINUX)
dnl	AC_CHECK_LIB(nss_nis, yp_get_default_domain)
        AC_CHECK_LIB(acl,main)
        AC_CHECK_HEADERS(acl.h sys/acl.h acl/libacl.h)
        ;;
   freebsd*|dragonfly*)
	AC_DEFINE(FREEBSD)
        CFLAGS="$CFLAGS -pthread -D_THREAD_SAFE -DBUILDTIN_GCC_THREAD"
        AC_CHECK_HEADERS(pthread.h)
        ;;
   netbsd*)
	AC_DEFINE(NETBSD)
        ;;
   newsos*)
	AC_DEFINE(NEWS_OS)
        ;;
   bsd/os*)
	AC_DEFINE(BSDOS)
        ;;
   bsd*)
	AC_DEFINE(BSD43)
        ;;
   aos*)
	AC_DEFINE(AOS)
        ;;
   nextstep*)
	AC_DEFINE(NEXTSTEP)
        ;;

   unicos*)
	AC_DEFINE(CFCRAY)
        ;;

   cray*)
	AC_DEFINE(CFCRAY)
        ;;

   qnx*)
        AC_DEFINE(CFQNX)
        ;;

   openbsd*|obsd*)
	AC_DEFINE(OPENBSD)
        ;;

   gnu*)
	AC_DEFINE(CFGNU)
        ;;

   sysv4.2MP|unix_sv*)
	AC_DEFINE(UNIXWARE)
        AC_CHECK_LIB(thread,main)         dnl POSIX threads, weird setup
        if test "$ac_cv_lib_thread_main" = "yes"; then
	   AC_CHECK_HEADERS(thread.h)
        fi
        ;;

   cygwin*)
        AC_DEFINE(NT)
        ;;

   sco*)
        AC_DEFINE(SCO)
        ;;

   darwin*)
        AC_DEFINE(DARWIN)
        LDFLAGS="-Xlinker -m $LDFLAGS"
        ;;

   *)
	AC_MSG_ERROR(Unknown system type $target_os)
        ;;
esac

dnl LIBS="$LIBS -L../pub -lcfpub"

dnl #####################################################################
dnl Configure LOCKDIR and LOGDIR
dnl #####################################################################

AC_ARG_WITH(workdir,
	[  --with-workdir=WORKDIR  default internal for trusted cache ],
	[
		if test x$withval != x ; then
			WORKDIR=$withval
		else
			WORKDIR=/var/cfengine
		fi
		AC_DEFINE_UNQUOTED(WORKDIR, "${WORKDIR}")
	],
	[
		WORKDIR=/var/cfengine

		AC_DEFINE_UNQUOTED(WORKDIR, "${WORKDIR}")
	]
)

dnl ####################################################################
dnl Set GCC CFLAGS only if using GCC.
dnl ####################################################################

AC_MSG_CHECKING(Checking for GCC Specific compile flags)
if test x"$GCC" = "xyes"; then
	
dnl # -Wformat - to be considered

	GCC_CFLAG="-g -O2 -Wreturn-type -Wmissing-prototypes -Wuninitialized -fPIC"		
	AC_MSG_RESULT(yes)
	AC_SUBST(GCC_CFLAG)
else 
	GCC_CFLAG=""
	AC_MSG_RESULT(no)
	AC_SUBST(GCC_CFLAG)
fi

dnl #####################################################################
dnl xen cpuid-based hvm detection
dnl #####################################################################

AC_MSG_CHECKING(Checking for Xen cpuid-based HVM detection)
if test x"$GCC" = "xyes"; then
	case $host_cpu in
		i[[3456]]86*|x86_64*)
			AC_DEFINE(XEN_CPUID_SUPPORT, 1, [Define if XEN cpuid-based HVM detection is available.])

			AC_MSG_RESULT(yes)
			;;
		*)
			AC_MSG_RESULT(no)
			;;
	esac
else 
	AC_MSG_RESULT(no)
fi

dnl ######################################################################
dnl Now make the Makefiles
dnl ######################################################################

AC_OUTPUT(Makefile src/Makefile docs/Makefile pub/Makefile knowledge/Makefile)

AC_MSG_RESULT(DONE: Configuration done. Run make/gmake to build cfengine.)

