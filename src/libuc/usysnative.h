/* usysnative HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* version %I% last-modified %G% */
/* virtual-system definitions */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in MacOS Darwin

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

#ifndef	USYSNATIVE_INCLUDE
#define	USYSNATIVE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */

#include	<sys/mount.h>		/* for |USTATFS| */
#include	<sys/resource.h>	/* POSIX® */
#include	<sys/socket.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<sys/statvfs.h>		/* POSIX® */
#include	<sys/time.h>		/* for |u_adjtime(3u)| */
#include	<sys/timeb.h>		/* for |uc_ftime(3uc)| */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/uio.h>		/* POSIX® */
#include	<sys/utsname.h>		/* POSIX® */
#include	<sys/select.h>		/* |u_select(3u)| */

#include	<sys/shm.h>		/* UNIX® Sys-V IPC - Shared-Memory */
#include	<sys/msg.h>		/* UNIX® Sys-V IPC - Message-Queues */
#include	<sys/sem.h>		/* UNIX® Sys-V IPC - Semaphores */

#include	<arpa/inet.h>		/* <- |in_addr_t| */

#include	<ucontext.h>		/* POSIX® */
#include	<dirent.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<netdb.h>		/* POSIX® */
#include	<poll.h>		/* POSIX® */
#include	<pthread.h>		/* POSIX® */
#include	<termios.h>		/* POSIX® */
#include	<utime.h>		/* POSIX® |u_utime(2)| */
#include	<pwd.h>			/* POSIX® */
#include	<grp.h>			/* POSIX® */
#include	<time.h>		/* CSTD */
#include	<errno.h>		/* CSTD */
#include	<limits.h>		/* CSTD */
#include	<signal.h>		/* CSTD */
#include	<stddef.h>		/* CSTD */
#include	<stdlib.h>		/* CSTD */
#include	<stdarg.h>		/* CSTD */
#include	<stdint.h>		/* CSTD */

/* synthetics */
#include	<usysacl.h>		/* need for stupid Apple-Darwin */


#endif /* USYSNATIVE_INCLUDE */


