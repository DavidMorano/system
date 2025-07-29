/* usyscalls HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* version %I% last-modified %G% */
/* UNIX® system-calls */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in MacOS Darwin

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

#ifndef	USYSCALLS_INCLUDE
#define	USYSCALLS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */

#include	<sys/types.h>
#include	<sys/utsname.h>
#include	<sys/uio.h>
#include	<sys/time.h>		/* |u_adjtime(2u)| */
#include	<sys/resource.h>
#include	<sys/stat.h>
#include	<sys/statvfs.h>
#include	<sys/socket.h>

#include	<poll.h>
#include	<signal.h>
#include	<limits.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<utime.h>		/* for old UNIX® |u_utime(2)| */
#include	<pthread.h>
#include	<termios.h>
#include	<time.h>
#include	<errno.h>
#include	<dirent.h>

#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<ubufdefs.h>

#include	<aflag.hh>
#include	<endian.h>

#include	<usys.h>	/* <- auxilllary OS support */
#include	<usysflag.h>
#include	<usysop.h>	/* UNIX® system-operations */
#include	<usysdata.h>	/* UNIX® system-data */
#include	<usysutility.hh>
#include	<usysauxinfo.h>
#include	<usysdata.h>

#include	<ustd.h>	/* <- converted system calls */

#include	<ugetloadavg.h>	/* load-average retrieval |u_getloadavg| */
#include	<um.h>		/* UNIX® memory-management */
#include	<uipc.h>	/* UNIX® System V IPC */
#include	<ustr.h>	/* UNIX® STREAMS® */
#include	<ufdlock.h>
#include	<ufiledesc.h>	/* file-descriptor */
#include	<ufileop.h>	/* file-operations */
#include	<usig.h>
#include	<uprocess.h>
#include	<uatfork.h>
#include	<uexec.h>
#include	<utimeoutdefs.h>
#include	<utimeout.h>
#include	<utimeouts.h>
#include	<ulogerror.h>
#include	<umemalloc.hh>

#include	<varnames.hh>
#include	<syswords.hh>
#include	<sysconfcmds.h>
#include	<ptx.h>
#include	<strnul.hh>
#include	<mailvalues.hh>
#include	<intx.h>
#include	<timewatch.hh>
#include	<timecount.hh>
#include	<timeval.h>
#include	<timespec.h>
#include	<itimerval.h>
#include	<itimerspec.h>
#include	<stdfnames.h>

#include	<xxtostr.h>
#include	<strtox.h>
#include	<baops.h>


#endif /* USYSCALLS_INCLUDE */


