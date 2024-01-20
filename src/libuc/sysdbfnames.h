/* sysdbfnames HEADER */
/* lang=C20 */

/* standard byte-order serializations */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-03-24, David A­D­ Morano
	This code is newly written but modeled after some code that
	was originally written for the Ethernet-Controller (ETHCON)
	development work.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

#ifndef	SYSDBFNAMES_INCLUDE
#define	SYSDBFNAMES_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


enum sysdbfiles {
	sysdbfile_pw,
	sysdbfile_sp,
	sysdbfile_ua,
	sysdbfile_gr,
	sysdbfile_pj,
	sysdbfile_pr,
	sysdbfile_nw,
	sysdbfile_ho,
	sysdbfile_sv,
	sysdbfile_sh,
	sysdbfile_overlast
} ;

extern cchar	*sysdbfnames[] ;


#endif /* SYSDBFNAMES_INCLUDE */


