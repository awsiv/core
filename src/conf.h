/* src/conf.h.  Generated from conf.h.in by configure.  */
/* src/conf.h.in.  Generated from configure.ac by autoheader.  */
/* cfengine for GNU
 
        Copyright (C) 1995/6
        Free Software Foundation, Inc.
 
   This file is part of GNU cfengine - written and maintained 
   by Mark Burgess, Dept of Computing and Engineering, Oslo College,
   Dept. of Theoretical physics, University of Oslo
 
   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.
 
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA

*/
/* The old route entry structure in newer BSDs */
/* #undef HAVE_ORTENTRY */

/* Do we ahve any route entry structure ? */
#define HAVE_RTENTRY 1

/* Whether to use the local REGEX functions */
/* #undef REGEX_MALLOC */

/* Whether to use LCHOWN to change ownerships */
#define HAVE_LCHOWN 1

/* Whether the thread library has setmask */
#define HAVE_PTHREAD_SIGMASK 1

/* Whether the thread library has setstacksize */
#define HAVE_PTHREAD_ATTR_SETSTACKSIZE 1

/* Whether libdb has db_open */
/* #undef HAVE_DB_CREATE */

/* Does the host have /var/run directory? */
/* #undef HAVE_VAR_RUN */


/* Special OS defines */
/* #undef SUN3 */
/* #undef SUN4 */
/* #undef SOLARIS */
/* #undef ULTRIX */
/* #undef HPuUX */
/* #undef AIX */
/* #undef OSF */
/* #undef IRIX */
#define LINUX 1
/* #undef DEBIAN */
/* #undef FREEBSD */
/* #undef NETBSD */
/* #undef NEWS_OS */
/* #undef BSDOS */
/* #undef BSD43 */
/* #undef AOS */
/* #undef SCO */
/* #undef NEXTSTEP */
/* #undef CFCRAY */
/* #undef CFQNX */
/* #undef CFGNU */
/* #undef UNIXWARE */
/* #undef OPENBSD */
/* #undef HAVE_SYS_ACL_H */
/* #undef NOTBROKEN */
/* #undef NT */
/* #undef DARWIN */

/* SVR4 header stuff */

/* #undef __EXTENSIONS__ */
/* #undef _POSIX_C_SOURCE */

/* Solaris 2.6 */

/* #undef __BIT_TYPES_DEFINED__ */

/* LOCK and LOG directories */

#define WORKDIR "/var/cfengine"

/* Special CFEngine symbols */

/* #undef AUTOCONF_HOSTNAME */
/* #undef AUTOCONF_SYSNAME */

/* Define if old Berkeley API */
/* #undef CF_OLD_DB */

/* Define to 1 if you have the <acl.h> header file. */
/* #undef HAVE_ACL_H */

/* Define to 1 if you have the <acl/libacl.h> header file. */
/* #undef HAVE_ACL_LIBACL_H */

/* Define to 1 if you have the `bcopy' function. */
#define HAVE_BCOPY 1

/* Define to 1 if you have the <bson.h> header file. */
/* #undef HAVE_BSON_H */

/* Define to 1 if you have the `chflags' function. */
/* #undef HAVE_CHFLAGS */

/* Define to 1 if the system has the type `clockid_t'. */
#define HAVE_CLOCKID_T 1

/* Define to 1 if you have the <dce/daclif.h> header file. */
/* #undef HAVE_DCE_DACLIF_H */

/* Define to 1 if you have the <depot.h> header file. */
/* #undef HAVE_DEPOT_H */

/* Define to 1 if you have the <dirent.h> header file, and it defines `DIR'.
   */
#define HAVE_DIRENT_H 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the `door' function. */
/* #undef HAVE_DOOR */

/* Define to 1 if you have the `drand48' function. */
#define HAVE_DRAND48 1

/* Define to 1 if you have the <dustat.h> header file. */
/* #undef HAVE_DUSTAT_H */

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Have FIPS OpenSSL cannister */
/* #undef HAVE_FIPS_MODE_SET */

/* Define to 1 if you have the <gd.h> header file. */
/* #undef HAVE_GD_H */

/* Define to 1 if you have the `getaddrinfo' function. */
#define HAVE_GETADDRINFO 1

/* Define to 1 if you have the `getcwd' function. */
#define HAVE_GETCWD 1

/* Define to 1 if you have the `gethostname' function. */
#define HAVE_GETHOSTNAME 1

/* Define to 1 if you have the `getnetgrent' function. */
#define HAVE_GETNETGRENT 1

/* Define to 1 if you have the `getzoneid' function. */
/* #undef HAVE_GETZONEID */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `acl' library (-lacl). */
/* #undef HAVE_LIBACL */

/* Define to 1 if you have the `cfconstellation' library (-lcfconstellation).
   */
/* #undef HAVE_LIBCFCONSTELLATION */

/* Define to 1 if you have the `crypto' library (-lcrypto). */
#define HAVE_LIBCRYPTO 1

/* Define to 1 if you have the `db' library (-ldb). */
#define HAVE_LIBDB 1

/* Define to 1 if you have the `dce' library (-ldce). */
/* #undef HAVE_LIBDCE */

/* Define to 1 if you have the `gd' library (-lgd). */
/* #undef HAVE_LIBGD */

/* Define to 1 if you have the `ldap' library (-lldap). */
/* #undef HAVE_LIBLDAP */

/* Define to 1 if you have the `mongoc' library (-lmongoc). */
/* #undef HAVE_LIBMONGOC */

/* Define to 1 if you have the `mysqlclient' library (-lmysqlclient). */
/* #undef HAVE_LIBMYSQLCLIENT */

/* Define to 1 if you have the `nsl' library (-lnsl). */
/* #undef HAVE_LIBNSL */

/* Define to 1 if you have the `pcre' library (-lpcre). */
#define HAVE_LIBPCRE 1

/* Define to 1 if you have the `pq' library (-lpq). */
/* #undef HAVE_LIBPQ */

/* Define to 1 if you have the <libpq-fe.h> header file. */
/* #undef HAVE_LIBPQ_FE_H */

/* Define to 1 if you have the `pthread' library (-lpthread). */
#define HAVE_LIBPTHREAD 1

/* Define to 1 if you have the `pthreads' library (-lpthreads). */
/* #undef HAVE_LIBPTHREADS */

/* Define to 1 if you have the `PW' library (-lPW). */
/* #undef HAVE_LIBPW */

/* Define to 1 if you have the `qdbm' library (-lqdbm). */
/* #undef HAVE_LIBQDBM */

/* Define to 1 if you have the `rt' library (-lrt). */
#define HAVE_LIBRT 1

/* Define to 1 if you have the `socket' library (-lsocket). */
/* #undef HAVE_LIBSOCKET */

/* Define to 1 if you have the `thread' library (-lthread). */
/* #undef HAVE_LIBTHREAD */

/* Define to 1 if you have the `tokyocabinet' library (-ltokyocabinet). */
/* #undef HAVE_LIBTOKYOCABINET */

/* Define to 1 if you have the `virt' library (-lvirt). */
/* #undef HAVE_LIBVIRT */

/* Define to 1 if you have the <malloc.h> header file. */
#define HAVE_MALLOC_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `mkfifo' function. */
#define HAVE_MKFIFO 1

/* Define to 1 if you have the <mongo_except.h> header file. */
/* #undef HAVE_MONGO_EXCEPT_H */

/* Define to 1 if you have the <mongo.h> header file. */
/* #undef HAVE_MONGO_H */

/* Define to 1 if you have the <mysql/mysql.h> header file. */
/* #undef HAVE_MYSQL_MYSQL_H */

/* Define to 1 if you have the <ndir.h> header file, and it defines `DIR'. */
/* #undef HAVE_NDIR_H */

/* Define to 1 if you have the <pcre.h> header file. */
#define HAVE_PCRE_H 1

/* Define to 1 if you have the <pcre/pcre.h> header file. */
/* #undef HAVE_PCRE_PCRE_H */

/* Define to 1 if you have the <pgsql/libpq-fe.h> header file. */
/* #undef HAVE_PGSQL_LIBPQ_FE_H */

/* Define to 1 if you have the <platform_hacks.h> header file. */
/* #undef HAVE_PLATFORM_HACKS_H */

/* Has NT extended process memory structure */
/* #undef HAVE_PROCESS_MEMORY_COUNTERS_EX */

/* Define if you have POSIX threads libraries and header files. */
#define HAVE_PTHREAD 1

/* Define to 1 if you have the <pthread.h> header file. */
#define HAVE_PTHREAD_H 1

/* Define to 1 if you have the `putenv' function. */
#define HAVE_PUTENV 1

/* Define to 1 if you have the `realpath' function. */
#define HAVE_REALPATH 1

/* Define to 1 if you have the <sched.h> header file. */
#define HAVE_SCHED_H 1

/* Define to 1 if you have the `setegid' function. */
#define HAVE_SETEGID 1

/* Define to 1 if you have the `seteuid' function. */
#define HAVE_SETEUID 1

/* Define to 1 if you have the `setregid' function. */
#define HAVE_SETREGID 1

/* Define to 1 if you have the `setreuid' function. */
#define HAVE_SETREUID 1

/* Define to 1 if you have the `setsid' function. */
#define HAVE_SETSID 1

/* Define to 1 if you have the `snprintf' function. */
#define HAVE_SNPRINTF 1

/* Define to 1 if you have the `srand48' function. */
#define HAVE_SRAND48 1

/* Define to 1 if you have the `statfs' function. */
#define HAVE_STATFS 1

/* Define to 1 if you have the `statvfs' function. */
#define HAVE_STATVFS 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strdup' function. */
#define HAVE_STRDUP 1

/* Define to 1 if you have the `strerror' function. */
#define HAVE_STRERROR 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strrchr' function. */
#define HAVE_STRRCHR 1

/* Define to 1 if you have the `strsep' function. */
#define HAVE_STRSEP 1

/* Define to 1 if you have the `strstr' function. */
#define HAVE_STRSTR 1

/* Define to 1 if `sa_len' is a member of `struct sockaddr'. */
/* #undef HAVE_STRUCT_SOCKADDR_SA_LEN */

/* Define to 1 if you have the `sysconf' function. */
#define HAVE_SYSCONF 1

/* Define to 1 if you have the `sysinfo' function. */
#define HAVE_SYSINFO 1

/* Define to 1 if you have the <sys/acl.h> header file. */
/* #undef HAVE_SYS_ACL_H */

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_DIR_H */

/* Define to 1 if you have the <sys/filesys.h> header file. */
/* #undef HAVE_SYS_FILESYS_H */

/* Define to 1 if you have the <sys/loadavg.h> header file. */
/* #undef HAVE_SYS_LOADAVG_H */

/* Define to 1 if you have the <sys/malloc.h> header file. */
/* #undef HAVE_SYS_MALLOC_H */

/* Define to 1 if you have the <sys/mount.h> header file. */
#define HAVE_SYS_MOUNT_H 1

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_NDIR_H */

/* Define to 1 if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define to 1 if you have the <sys/sockio.h> header file. */
/* #undef HAVE_SYS_SOCKIO_H */

/* Define to 1 if you have the <sys/statfs.h> header file. */
#define HAVE_SYS_STATFS_H 1

/* Define to 1 if you have the <sys/statvfs.h> header file. */
#define HAVE_SYS_STATVFS_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/systeminfo.h> header file. */
/* #undef HAVE_SYS_SYSTEMINFO_H */

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/vfs.h> header file. */
#define HAVE_SYS_VFS_H 1

/* Define to 1 if you have <sys/wait.h> that is POSIX.1 compatible. */
#define HAVE_SYS_WAIT_H 1

/* Define to 1 if you have the <tchdb.h> header file. */
/* #undef HAVE_TCHDB_H */

/* Define to 1 if you have the <tcutil.h> header file. */
/* #undef HAVE_TCUTIL_H */

/* Define to 1 if you have the <thread.h> header file. */
/* #undef HAVE_THREAD_H */

/* Define to 1 if you have the <time.h> header file. */
#define HAVE_TIME_H 1

/* Define to 1 if you have the `uname' function. */
#define HAVE_UNAME 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the <utime.h> header file. */
#define HAVE_UTIME_H 1

/* Define to 1 if you have the <vfs.h> header file. */
/* #undef HAVE_VFS_H */

/* Define to 1 if you have the `waitpid' function. */
#define HAVE_WAITPID 1

/* Define to 1 if you have the <winsock2.h> header file. */
/* #undef HAVE_WINSOCK2_H */

/* Define to 1 if you have the <zone.h> header file. */
/* #undef HAVE_ZONE_H */

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Native NT build. */
/* #undef MINGW */

/* Name of package */
#define PACKAGE "cfnova"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME ""

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION ""

/* Define to the necessary symbol if this constant uses a non-standard name on
   your system. */
/* #undef PTHREAD_CREATE_JOINABLE */

/* Define if QDBM is available. */
/* #undef QDB */

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define if Tokyo Cabinet is available. */
/* #undef TCDB */

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#define TIME_WITH_SYS_TIME 1

/* Define if BerkeleyDB is available. */
#define USE_BERKELEY_DB 1

/* Define if OpenSSL is available. */
#define USE_OPENSSL_LIB 1

/* Define if PCRE is available. */
#define USE_PCRE 1

/* Version number of package */
#define VERSION "2.0.3a1"

/* Define if you want to use SELINUX */
/* #undef WITH_SELINUX */

/* Define if XEN cpuid-based HVM detection is available. */
#define XEN_CPUID_SUPPORT 1

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to `int' if <sys/types.h> doesn't define. */
/* #undef gid_t */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef mode_t */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef pid_t */

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */

/* Define to `int' if <sys/types.h> doesn't define. */
/* #undef uid_t */
