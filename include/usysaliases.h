/* utypealiases INCLUDE */
/* lang=C20 */

/* virtual-system definitions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A�D� Morano
	This module was originally written.

	= 2017-08-01, David A�D� Morano
	Updated for lack of interfaces in MacOS Darwin

*/

/* Copyright � 1998,2017 David A�D� Morano.  All rights reserved. */

#ifndef	UTYPEALIASES_INCLUDE
#define	UTYPEALIASES_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */

#include	<sys/types.h>
#include	<sys/utsname.h>
#include	<sys/uio.h>
#include	<sys/time.h>		/* for 'u_adjtime(3u)' */
#include	<sys/timeb.h>		/* for 'uc_ftime(3uc)' */
#include	<sys/resource.h>
#include	<sys/stat.h>
#include	<sys/statvfs.h>
#include	<sys/socket.h>
#include	<sys/poll.h>
#include	<sys/msg.h>

#if	defined(SYSHAS_ACL) && (SYSHAS_ACL > 0)
#include	<sys/acl.h>
#endif

#include	<ucontext.h>
#include	<signal.h>
#include	<limits.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<utime.h>		/* for 'u_utime(2)' */
#include	<pthread.h>
#include	<termios.h>
#include	<time.h>
#include	<errno.h>
#include	<dirent.h>
#include	<netdb.h>
#include	<pwd.h>
#include	<grp.h>

#if	defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0)
#include	<stropts.h>
#endif

#if	defined(SYSHAS_XTI) && (SYSHAS_XTI > 0)
#include	<xti.h>
#endif

#if	defined(OSNAME_SunOS) && (OSNAME_SunOS > 0)
#include	<shadow.h>
#include	<project.h>
#include	<user_attr.h>
#endif

#include	<usys.h>		/* <- auxillary OS support */

#include	<stdintx.h>
#include	<clanguage.h>
#include	<usysdefs.h>


/* UNIX kernal structures */

#ifndef	UTSNAME
#define	UTSNAME		struct utsname
#endif

#ifndef	SIGACTION
#define	SIGACTION	struct sigaction
#endif

#ifndef	SIGEVENT
#define	SIGEVENT	struct sigevent
#endif

#ifndef	SIGVAL
#define	SIGVAL		union sigval	
#endif

#ifndef	RLIMIT
#define	RLIMIT		struct rlimit
#endif

#ifndef	DIRENT
#define	DIRENT		struct dirent
#endif

#ifndef	USTAT
#define	USTAT		struct ustat
#endif

#ifndef	USTATVFS
#define	USTATVFS	struct ustatvfs
#endif

#ifndef	SOCKADDR
#define	SOCKADDR	struct sockaddr
#endif

#ifndef	MSGHDR
#define	MSGHDR		struct msghdr
#endif

#ifndef	WINSIZE
#define	WINSIZE		struct winsize
#endif

#ifndef	TIMEVAL
#define	TIMEVAL		struct timeval
#endif

#ifndef	TIMESPEC
#define	TIMESPEC	struct timespec
#endif

#ifndef	ITIMERSPEC
#define	ITIMERSPEC	struct itimerspec
#endif

#ifndef	UTIMBUF
#define	UTIMBUF		struct utimbuf
#endif

#ifndef	TIMEB
#define	TIMEB		struct timeb
#endif

#ifndef	ADDRINFO
#define	ADDRINFO	struct addrinfo
#endif

#ifndef	HOSTENT
#define	HOSTENT		struct hostent
#endif

#ifndef	TERMIOS
#define	TERMIOS		struct termios
#endif

#ifndef	STRBUF
#define	STRBUF		struct strbuf
#endif

#ifndef	POLLFD
#define	POLLFD		struct pollfd
#endif

#ifndef	STRRECVFD
#define	STRRECVFD	struct strrecvfd
#endif

#ifndef	IOVCEC
#define	IOVCEC		struct iovec
#endif

#ifndef	MSGIDDS
#define	MSGIDDS		struct msqid_ds		/* for SysV-IPC */
#endif

/* not constant - "Control-Message-Header" */
#ifndef	CMSGHDR
#define	CMSGHDR		struct cmsghdr
#endif

#ifndef	FLOCK
#define	FLOCK		struct flock
#endif

/* constant versions of above */

#ifndef	CSIGVAL
#define	CSIGVAL		const union sigval	
#endif

#ifndef	CTIMESPEC
#define	CTIMESPEC	const struct timespec
#endif

#ifndef	CSOCKADDR
#define	CSOCKADDR	const struct sockaddr
#endif

/* C-language library structures */

#ifndef	PASSWD
#define	PASSWD		struct passwd
#endif

#ifndef	SPWD
#define	SPWD		struct spwd
#endif

#ifndef	GROUP
#define	GROUP		struct group
#endif

#ifndef	PROJECT
#define	PROJECT		struct project
#endif

#ifndef	USERATTR
#define	USERATTR	userattr
#endif

/* constant versions of above */

#ifndef	CPASSWD
#define	CPASSWD		const PASSWD
#endif

#ifndef	CSPWD
#define	CSPWD		const SPWD
#endif

#ifndef	CGROUP
#define	CGROUP		const GROUP
#endif

#ifndef	CPROJECT
#define	CPROJECT	const PROJECT
#endif

#ifndef	CUSERATTR
#define	CUSERATTR	const USERATTR
#endif

/* network related structures */

#ifndef	PROTOENT
#define	PROTOENT	struct protoent
#endif

#ifndef	NETENT
#define	NETENT		struct netent
#endif

#ifndef	HOSTENT
#define	HOSTENT		struct hostent
#endif

#ifndef	SERVENT
#define	SERVENT		struct servent
#endif

#ifndef	CPROTOENT
#define	CPROTOENT	const struct protoent
#endif

#ifndef	CNETENT
#define	CNETENT		const struct netent
#endif

#ifndef	CHOSTENT
#define	CHOSTENT	const struct hostent
#endif

#ifndef	CSERVENT
#define	CSERVENT	const struct servent
#endif


#endif /* UTYPEALIASES_INCLUDE */


