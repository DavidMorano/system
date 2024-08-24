/* syspasswd SUPPORT */
/* lang=C++20 */

/* system user-entry enumeration */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A-D- Morano
	This object module was morphed from some previous one. I
	do not remember what the previous one was.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	We enumerate (reentrantly and thread safely) PASSWD entries
	from the system PASSWD database.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<climits>
#include	<usystem.h>
#include	<filemap.h>
#include	<passwdent.h>
#include	<localmisc.h>

#include	"syspasswd.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int syspasswd_open(SYSPASSWD *op,cchar *sufname) noex {
	csize_t		nmax = INT_MAX ;
	int		rs ;
	cchar		*defufname = SYSPASSWD_FNAME ;

	if (op == NULL) return SR_FAULT ;

	if (sufname == NULL) sufname = defufname ; /* default */

	memclear(op) ;

	if ((rs = filemap_open(&op->b,sufname,nmax)) >= 0) {
	    op->magic = SYSPASSWD_MAGIC ;
	}

	return rs ;
}
/* end if (syspasswd_start) */

int syspasswd_close(SYSPASSWD *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == NULL) return SR_FAULT ;
	if (op->magic != SYSPASSWD_MAGIC) return SR_NOTOPEN ;

	rs1 = filemap_close(&op->b) ;
	if (rs >= 0) rs = rs1 ;

	op->magic = 0 ;
	return rs ;
} 
/* end subroutine (syspasswd_close) */

int syspasswd_readent(SYSPASSWD *op,ucentpw *pwp,char *pwbuf,int pwlen) noex {
	int		rs ;
	int		ll ;
	cchar		*lp ;

	if (op == NULL) return SR_FAULT ;
	if (pwp == NULL) return SR_FAULT ;
	if (pwbuf == NULL) return SR_FAULT ;
	if (op->magic != SYSPASSWD_MAGIC) return SR_NOTOPEN ;

	while ((rs = filemap_getln(&op->b,&lp)) > 0) {
	    ll = rs ;
	    if (lp[ll-1] == '\n') ll -= 1 ;
	    rs = passwdent_parse(pwp,pwbuf,pwlen,lp,ll) ;
	    if (rs > 0) break ;
	    if (rs < 0) break ;
	} /* end while */

	return rs ;
}
/* end subroutine (syspasswd_readent) */

int syspasswd_reset(SYSPASSWD *op) noex {
	int		rs ;
	if (op == NULL) return SR_FAULT ;
	if (op->magic != SYSPASSWD_MAGIC) return SR_NOTOPEN ;
	rs = filemap_rewind(&op->b) ;
	return rs ;
}
/* end subroutine (syspasswd_reset) */


