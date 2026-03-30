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
#include	<usysnative.h>

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
#include	<umem.hh>
#include	<usysop.h>	/* UNIX® system-operations */
#include	<usysdata.h>	/* UNIX® system-data */
#include	<usysutility.hh>
#include	<usysauxinfo.h>
#include	<usysdata.h>
#include	<usysconf.h>
#include	<usupport.h>

#include	<ustd.h>	/* <- converted system calls */
#include	<uatexit.h>
#include	<ucomposite.h>
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
#include	<ulogerror.h>
#include	<umem.hh>
#include	<utimeoutdefs.h>
#include	<utimeout.h>
#include	<utimeouts.h>
#include	<uxti.h>

#include	<varnames.hh>
#include	<syswords.hh>
#include	<sysconfcmds.h>
#include	<ptx.h>
#include	<strnul.hh>
#include	<mailvalues.hh>
#include	<intx.h>
#include	<timewatch.hh>
#include	<timecount.hh>
#include	<timeval.hh>
#include	<timespec.h>
#include	<itimerval.h>
#include	<itimerspec.h>
#include	<stdfnames.h>
#include	<xxtostr.h>
#include	<strtox.h>
#include	<baops.h>

#include	<ccfile.hh>
#include	<readln.hh>


#endif /* USYSCALLS_INCLUDE */


