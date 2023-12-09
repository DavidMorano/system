/* snfsflags */
/* lang=C20 */

/* make string version of the file-system flags */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */

/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snfsflags

	Description:
	Ths subroutine is similar to |sncpy1(3dam)| but it takes a
	counted string for the source rather than only a NUL-terminated
	string.

	Synopsis:
	int snfsflags(char *dbuf,int dlen,ulong flags) noex

	Arguments:
	dbuf		destination string buffer
	dlen		destination string buffer length
	flags		FS-flags

	Returns:
	>=0		number of bytes in result
	<0		error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/statvfs.h>
#include	<string.h>
#include	<usystem.h>
#include	<storebuf.h>
#include	<localmisc.h>

#include	"snflags.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

struct flagstrs {
	int		f ;
	cchar		*s ;
} ;


/* foward references */


/* local variables */

static const struct flagstrs	fs_vstat[] = {
	{ ST_RDONLY, "RDONLY" },
	{ ST_NOSUID, "NOSUID" },
#ifdef	ST_NOTRUNC
	{ ST_NOTRUNC, "NOTRUNC" },
#endif
	{ 0, NULL }
} ;


/* exported subroutines */

int snfsflags(char *dbuf,int dlen,ulong flags) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (dbuf) {
	    SNFLAGS	ss ;
	    if ((rs = snflags_start(&ss,dbuf,dlen)) >= 0) {
	        for (int i = 0 ; (rs >= 0) && fs_vstat[i].f ; i += 1) {
	            if (flags & fs_vstat[i].f) {
	                rs = snflags_addstr(&ss,fs_vstat[i].s) ;
		    }
	        } /* end for */
	        rs1 = snflags_finish(&ss) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (snflags) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (snfsflags) */


