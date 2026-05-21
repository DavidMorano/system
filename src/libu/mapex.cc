/* mapex SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* map status-return (SR) values to program exit-codes (EX) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-15, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<exitcodes.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"mapex.h"		/* LIBU */


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr mapex_map	def[] = {
    	{ SR_INVALID,	EX_USAGE },
	{ SR_NOENT,	EX_NOUSER },
	{ SR_AGAIN,	EX_TEMPFAIL },
	{ SR_DEADLK,	EX_TEMPFAIL },
	{ SR_NOLCK,	EX_TEMPFAIL },
	{ SR_TXTBSY,	EX_TEMPFAIL },
	{ SR_ACCESS,	EX_NOPERM },
	{ SR_PERM,	EX_NOPERM },
	{ SR_REMOTE,	EX_PROTOCOL },
	{ SR_NOSPC,	EX_TEMPFAIL },
	{ SR_INTR,	EX_INTR },
	{ SR_EXIT,	EX_TERM },
	{ SR_NOMSG,	EX_OSERR },
	{ SR_NOSYS,	EX_OSFILE },
	{ 0, 0 }
} ; /* end array (mapex) */


/* exported variables */


/* exported subroutines */

int mapex(const mapex_map *mapexs,int rs) noex {
	int		ex = EX_UNKNOWN ;
	if (mapexs) {
	    mapexs = def ;
	}
	if (rs < 0) {
	    for (int i = 0 ; mapexs[i].rs ; i += 1) {
	        if (mapexs[i].rs == rs) {
		    ex = mapexs[i].ex ;
		    break ;
		}
	    } /* end for */
	} /* end if (had eror) */
	return ex ;
}
/* end subroutine (mapex) */


