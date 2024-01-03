/* sysdbfnames HEADER */
/* lang=C20 */

/* standard byte-order serializations */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-03-24, David A­D­ Morano
	This code is newly written but modeled after some code that
	was originally written to get some standardized variable
	serialization for the Ethernet-Controller (ETHCON) development
	work.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

#ifndef	SYSDBFNAMES_INCLUDE
#define	SYSDBFNAMES_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<clanguage.h>


#ifndef	STDFNIN
#define	STDFNIN		sysdbfnames[sysdbfile_in]
#endif

#ifndef	STDFNOUT
#define	STDFNOUT	sysdbfnames[sysdbfile_out]
#endif

#ifndef	STDFNERR
#define	STDFNERR	sysdbfnames[sysdbfile_err]
#endif

#ifndef	STDFNLOG
#define	STDFNLOG	sysdbfnames[sysdbfile_log]
#endif

#ifndef	STDFNNUL
#define	STDFNNUL	sysdbfnames[sysdbfile_null]
#endif


enum sysdbfiles {
	sysdbfile_passwd,
	sysdbfile_shadow,
	sysdbfile_userattr,
	sysdbfile_group,
	sysdbfile_project,
	sysdbfile_protocols,
	sysdbfile_networks,
	sysdbfile_hosts,
	sysdbfile_services,
	sysdbfile_shells,
	sysdbfile_overlast
} ;

extern cchar	*sysdbfnames[] ;


#endif /* SYSDBFNAMES_INCLUDE */


