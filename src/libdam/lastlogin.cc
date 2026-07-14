/* lastlogin SUPPORT */
/* charset=ISO8859-1 */
/* version %I% last-modified %G% */

/* retrieve the time of the last login given specified information */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* non-switchable debug print-outs */

/* revision history:

	= 1995-01-22, David A­D­ Morano
	This subroutine module was adopted for use from some previous
	code that performed the similar sorts of functions.

*/

/* Copyright © 1995 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	lastlogin

	Description:
	This subroutine is used to get the last login information
	for the specified UID.  The last login information is
	retrieved from the caller specified 'lastlog' file, which
	is by default '/var/adm/lastlog'.

	Synopsis:
	int lastlogin(cc *fn,uid_t uid,time_t *tp,char *lbuf,char *hbuf) noex

	Arguments:
	fn		the 'lastlog' file
	uid		the UID to lookup
	tp		pointer to the 'lastlog' timestamp
	lbuf		buffer to receive the line-string
	hbuf		buffer to receive hostname-string

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<lastlogfile.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG */

#include	"lastlogin.h"


/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0
#endif


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr uid_t		uidend		= uid_t(-1) ;
cbool			f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int lastlogin(cc *fn,uid_t uid,time_t *tp,char *lbuf,char *hbuf) noex {
    	cint		of = O_RDONLY ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	DEBUGPRINTF("ent\n") ;
	if (uid == uidend) uid = getuid() ;
	if (lastlogfile lf ; (rs = lastlogfile_open(&lf,fn,of)) >= 0) {
	    {
	        rs = lastlogfile_rdinfo(&lf,uid,tp,lbuf,hbuf) ;
	        len = rs ;
	    }
	    rs1 = lastlogfile_close(&lf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (lastlogfile) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return (rs >= 0) ? len : rs ;
} /* end subroutine (lastlogin) */


