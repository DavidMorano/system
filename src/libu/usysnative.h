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
#include	<sys/resource.h>
#include	<sys/socket.h>
#include	<sys/stat.h>
#include	<sys/statvfs.h>
#include	<sys/time.h>		/* for |u_adjtime(3u)| */
#include	<sys/timeb.h>		/* for |uc_ftime(3uc)| */
#include	<sys/types.h>
#include	<sys/uio.h>
#include	<sys/utsname.h>
#include	<sys/select.h>		/* |u_select(3u)| */

#include	<sys/shm.h>		/* UNIX® Sys-V IPC - Shared-Memory */
#include	<sys/msg.h>		/* UNIX® Sys-V IPC - Message-Queues */
#include	<sys/sem.h>		/* UNIX® Sys-V IPC - Semaphores */

#include	<arpa/inet.h>		/* <- |in_addr_t| */

#include	<dirent.h>
#include	<errno.h>
#include	<fcntl.h>
#include	<grp.h>
#include	<limits.h>
#include	<netdb.h>
#include	<poll.h>
#include	<pthread.h>
#include	<pwd.h>
#include	<signal.h>
#include	<stddef.h>
#include	<stdlib.h>
#include	<stdarg.h>
#include	<stdint.h>
#include	<termios.h>
#include	<time.h>
#include	<ucontext.h>
#include	<unistd.h>
#include	<utime.h>		/* |u_utime(2)| */

/* synthetics */
#include	<usysacl.h>		/* need for stupid Apple-Darwin */

#include	<clanguage.h>

#endif /* USYSNATIVE_INCLUDE */


