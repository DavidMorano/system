/* sysgroup SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* system group-entry enumeration */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A­D­ Morano
	This object module was morphed from some previous one.  I
	do not remember what the previous one was.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sysgroup

	Description:
	We enumerate (reentrantly and thread safely) group entries
	from the system GROUP database.

*******************************************************************************/


#include	<sys/types.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<ucgrent.h>
#include	<localmisc.h>

#include	"sysgroup.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sysgroup_open(sysgroup *op,cchar *sgfname) noex {
	csize		nmax = INT_MAX ;
	int		rs ;
	cchar	*defgfname = SYSGROUP_FNAME ;

	if (op == NULL) return SR_FAULT ;

	if (sgfname == NULL) sgfname = defgfname ; /* default */

	memclear(op) ;

	if ((rs = filemap_open(&op->b,sgfname,O_RDONLY,nmax)) >= 0) {
	    op->magic = SYSGROUP_MAGIC ;
	}

	return rs ;
}
/* end if (sysgroup_open) */

int sysgroup_close(sysgroup *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == NULL) return SR_FAULT ;
	if (op->magic != SYSGROUP_MAGIC) return SR_NOTOPEN ;

	rs1 = filemap_close(&op->b) ;
	if (rs >= 0) rs = rs1 ;

	op->magic = 0 ;
	return rs ;
} 
/* end subroutine (sysgroup_close) */

int sysgroup_readent(sysgroup *op,ucgrent *grp,char *grbuf,int grlen) noex {
	int		rs ;
	int		ll ;
	cchar	*lp ;

	if (op == NULL) return SR_FAULT ;
	if (grp == NULL) return SR_FAULT ;
	if (grbuf == NULL) return SR_FAULT ;
	if (op->magic != SYSGROUP_MAGIC) return SR_NOTOPEN ;

	while ((rs = filemap_getline(&op->b,&lp)) > 0) {
	    ll = rs ;
	    if (lp[ll-1] == '\n') ll -= 1 ;
	    rs = groupent_parse(grp,grbuf,grlen,lp,ll) ;
	    if (rs != 0) break ;
	} /* end while */

	return rs ;
}
/* end subroutine (sysgroup_readent) */

int sysgroup_reset(sysgroup *op) noex {
	int		rs ;

	if (op == NULL) return SR_FAULT ;
	if (op->magic != SYSGROUP_MAGIC) return SR_NOTOPEN ;

	rs = filemap_rewind(&op->b) ;

	return rs ;
}
/* end subroutine (sysgroup_reset) */


