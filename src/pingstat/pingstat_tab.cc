/* progpingtab SUPPORT */
/* charset=ISO8859-1 */
/* version %I% last-modified %G% */

/* add a ping-tab name to the list */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 2001-09-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*****************************************************************************

  	Description:
	This subroutine adds a ping-tab name to the list of them.

*****************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<climits>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<field.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"pingtab.h"
#include	"config.h"
#include	"defs.h"


/* local defines */

#ifndef	PI
#define	PI	proginfo
#endif

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* external subroutines */


/* externals variables */


/* local global variabes */


/* local structures */


/* forward references */


/* local variables */

constexpr char		pterms[32] = {
	0x00, 0x3E, 0x00, 0x00,
	0x01, 0x10, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ; /* end array */

cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int progpingtabadd(PI *pip,cchar *abuf,int alen) noex {
	vecstr		*ptp = &pip->pingtabs ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	DEBUGPRINTF("ent >%r<\n",abuf,alen) ;
#endif

	if (field fsb ; (rs = fsb.start(abuf,alen)) >= 0) {
	    int		fl ;
	    cchar	*fp ;
	    while ((fl = fdb.get(pterms,&fp)) >= 0) {
	        if (fl > 0) {
	            c += 1 ;
	            rs = ptp->adduniq(fp,fl) ;
		}
	        if (rs < 0) break ;
	    } /* e
		 nd while */
	    rs1 = fsb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (field) */

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	DEBUGPRINTF("ret rs=%d c=%u\n",rs,c) ;
#endif

	return (rs >= 0) ? c : rs ;
} /* end subroutine (progpingtabadd) */


