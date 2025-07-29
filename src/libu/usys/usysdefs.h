/* usysdefs HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* define the various system buffer sizes */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	This file defines various preprocessor variables for use
	in setting up the UNIX®-system default for various buffer
	sizes.  The real run-time system buffer sizes are (indeed)
	determined at runtime where they may exist.

	Usage:
	Among the most important uses for the data contained herein
	is the BUFSIZE management facility. It uses the data below
	to set default buffer sizes for those systems that do not 
	have dynamicly managed buffer size support in the system kernel.

*******************************************************************************/

#ifndef	USYSDEFS_INCLUDE
#define	USYSDEFS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/utsname.h>
#include	<sys/param.h>
#include	<sys/stat.h>		/* |S_IS{x}| + S_IF{x}| */
#include	<limits.h>		/* |{xxx}_MIN| + |{xxx}_MAX| */
#include	<signal.h>		/* |SIG{x}| */
#include	<unistd.h>		/* |_SC_{x}| + |_PC_{x}| */
#include	<fcntl.h>		/* |O_{x}| */
#include	<netdb.h>		/* |NI_MAX{x}| */

#include	<usys_stat.h>		/* |S_{x}| definitions */


/* extra "open" flags */
enum extraopenflags {
	extraopenflag_start = 28,
	extraopenflag_minmode = 28,
	extraopenflag_minfd,
	extraopenflag_network,
	extraopenflag_overlast
} ;

/* missing UNIX® signals */
enum signalmissings {
	signalmissing_start = 1000,
	signalmissing_pwr = 1000,
	signalmissing_cancel,
	signalmissing_lost,
	signalmissing_overlast
} ;

#ifndef	SIGCLD
#define	SIGCLD		SIGCHLD
#endif
#ifndef	SIGCHILD
#define	SIGCHILD	SIGCHLD
#endif
#ifndef	SIGALARM
#define	SIGALARM	SIGALRM
#endif
#ifndef	SIGPOLL
#define	SIGPOLL		SIGIO
#endif
#ifndef	SIGPWR
#define	SIGPWR		signalmissing_pwr
#endif
#ifndef	SIGCANCEL
#define	SIGCANCEL	signalmissing_cancel
#endif
#ifndef	SIGLOST
#define	SIGLOST		signalmissing_lost
#endif

/* missing file open-flags */
#ifndef	OM_SPECIAL
#define	OM_SPECIALMASK	((~ 0) << extraopenflag_start)
#endif
#ifndef	O_LARGEFILE
#define	O_LARGEFILE	0
#endif
#ifndef	O_DIRECTORY
#define	O_DIRECTORY	0
#endif
#ifndef	O_DIRECT
#define	O_DIRECT	0
#endif
#ifndef	O_TMPFILE
#define	O_TMPFILE	0
#endif
#ifndef	O_PRIV
#define	O_PRIV		0
#endif
#ifndef	O_MINMODE
#define	O_MINMODE	(1 << extraopenflag_minmode)
#endif
#ifndef	O_MINFD
#define	O_MINFD		(1 << extraopenflag_minfd)
#endif
#ifndef	O_NETWORK
#define	O_NETWORK	(1 << extraopenflag_network)
#endif

/* extra system flags for |uc_lockfile(3uc)| */
#ifndef	F_UNLOCK
#define	F_UNLOCK	F_ULOCK
#endif
#ifndef	F_WLOCK	
#define	F_WLOCK		F_LOCK
#endif
#ifndef	F_TWLOCK
#define	F_TWLOCK	F_TLOCK
#endif
#ifndef	F_WTEST
#define	F_WTEST		F_TEST
#endif
#ifndef	F_RLOCK
#define	F_RLOCK		10		/* new! (watch UNIX® for changes) */
#endif
#ifndef	F_TRLOCK
#define	F_TRLOCK	11		/* new! (watch UNIX® for changes) */
#endif
#ifndef	F_RTEST
#define	F_RTEST		12		/* new! (watch UNIX® for changes) */
#endif

/* system configuration */

/* PATHCONF preprocessor defines */
#ifndef	_PC_CHOWN_RESTRICTED
#define	_PC_CHOWN_RESTRICTED	-1
#endif

/* various limits (that might be missing) */

#ifndef	SHORT_MIN
#ifdef	SHRT_MIN
#define	SHORT_MIN	SHRT_MIN
#else
#define	SHORT_MIN	(-32768)	/* min value of a "short int" */
#endif
#endif

#ifndef	SHORT_MAX
#ifdef	SHRT_MAX
#define	SHORT_MAX	SHRT_MAX
#else
#define	SHORT_MAX	32767		/* max value of a "short int" */
#endif
#endif

#ifndef	USHORT_MAX
#ifdef	USHRT_MAX
#define	USHORT_MAX	USHRT_MAX
#else
#define	USHORT_MAX	65535		/* max value of "unsigned short int" */
#endif
#endif

#ifndef	SIZE_MAX
#define	SIZE_MAX	ULONG_MAX
#endif

#ifndef	SSIZE_MAX
#define	SSIZE_MAX	LONG_MAX
#endif

/* the following could be determined dynamically, but we choose not to */
#ifndef	TIME_MIN
#define	TIME_MIN	LONG_MIN
#endif
#ifndef	TIME_MAX
#define	TIME_MAX	LONG_MAX
#endif

/* some stuff that not all systems (like GNU-Linux) have */

/* max-namelen */
#ifndef	MAXNAMELEN
#ifdef	NAME_MAX
#define	MAXNAMELEN	NAME_MAX
#else
#define	MAXNAMELEN	256		/* common value */
#endif
#endif /* MAXNAMELEN */

/* max-pathlen */
#ifndef	MAXPATHLEN
#ifdef	PATH_MAX
#define	MAXPATHLEN	PATH_MAX
#else
#define	MAXPATHLEN	2048		/* common value */
#endif
#endif /* MAXPATHLEN */

/* max-linelen */
#ifndef	MAXLINELEN
#ifdef	LINE_MAX
#define	MAXLINELEN	LINE_MAX
#else
#define	MAXLINELEN	2048		/* common value */
#endif
#endif /* MAXLINELEN */

#ifndef	NODENAMELEN
#ifdef	SYS_NMLN
#define	NODENAMELEN	(SYS_NMLN-1)	/* usually 256 for SVR4! */
#else
#define	NODENAMELEN	256		/* should be at least 256 for SVR4! */
#endif
#endif

#ifndef	USERNAMELEN
#ifdef	LOGNAME_MAX
#define	USERNAMELEN	LOGNAME_MAX
#else
#define	USERNAMELEN	32
#endif
#endif

#ifndef	GROUPNAMELEN
#ifdef	LOGNAME_MAX
#define	GROUPNAMELEN	LOGNAME_MAX
#else
#define	GROUPNAMELEN	32
#endif
#endif

/* Solaris® project name */
#ifndef	PROJNAMELEN
#ifdef	LOGNAME_MAX
#define	PROJNAMELEN	LOGNAME_MAX
#else
#define	PROJNAMELEN	32
#endif
#endif

#ifndef	LOGNAMELEN
#ifdef	LOGNAME_MAX
#define	LOGNAMELEN	LOGNAME_MAX
#else
#define	LOGNAMELEN	32
#endif
#endif

/* symbol name (this is really 255 on most platforms) */
#ifndef	SYMNAMELEN
#define	SYMNAMELEN	100		/* symbol-name-length (really 255) */
#endif

#ifndef	ARGBUFLEN
#define	ARGBUFLEN	MAXARGLEN
#endif

#ifndef	LINEBUFLEN
#define	LINEBUFLEN	MAXLINELEN
#endif

#ifndef	NAMEBUFLEN
#define	NAMEBUFLEN	MAXNAMELEN
#endif

#ifndef	PATHBUFLEN
#define	PATHBUFLEN	MAXPATHLEN
#endif

#ifndef	MSGBUFLEN
#define	MSGBUFLEN	(16 * 1024)
#endif /* MSGBUFLEN */

#ifndef	PASSWORDLEN
#define	PASSWORDLEN	8
#endif

/* service name */
#ifndef	SVCNAMELEN
#define	SVCNAMELEN	32
#endif

/* maximum SHM name (as per POSIX®) */
#ifndef	SHMNAME_MAX
#define	SHMNAME_MAX	14		/* historic value (in decimal) */
#endif

/* maximum PID on the system (could be wrong) */
#ifndef	PID_MAX
#define	PID_MAX		999999		/* historic value (in decimal) */
#endif

/* Network-Interface (NI) */
#ifndef	NI_MAXHOST
#define	NI_MAXHOST	MAXHOSTNAMELEN
#endif
#ifndef	NI_MAXSERV
#define	NI_MAXSERV	SVCNAMELEN
#endif

/* network-related name resolving service (some system do not have) */
#ifndef	NETDB_INTERAL
#define	NETDB_INTERAL	-1
#endif

/* for |poll(2)| */
#ifndef	POLL_INTMULT
#define	POLL_INTMULT	1000		/* poll-interval-multiplier (to secs) */
#endif

#ifndef	MAXBASE
#define	MAXBASE		64		/* standard (common) value */
#endif

/* C-C++ memory order */
#ifndef	memord_relaxed
#define	memord_relaxed	std::memory_order_relaxed
#define	memord_consume	std::memory_order_consume
#define	memord_acquire	std::memory_order_acquire
#define	memord_release	std::memory_order_release
#define	memord_acqrel	std::memory_order_acq_rel
#define	memord_seq	std::memory_order_seq_cst
#endif /* memord_relaxed */


#endif /* USYSDEFS_INCLUDE */


