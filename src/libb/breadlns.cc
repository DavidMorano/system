/* breadlns SUPPORT */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get a line with possible continuation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	Written a new out of frustration of previous hacks.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	breadlns

	Description:
	This is a BFILE subroutine that reads line that may be
	continued onto the next real line using the back-slash
	character.

	Synopsis:
	int breadlns(bfile *op,char *lbuf,int llen,int to,int *lcp) noex

	Arguments:
	op		pointer to object
	lbuf		user buffer to receive data
	llen		length of user supplied buffer
	to		time-out (to=-1 mean "infinite")
	lcp		pointer to integer to receive number of lines read

	Returns:
	>=0		number bytes returned to caller (not bytes read)
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */

#define	ISCONT(b,bl)	iscont(b,bl)


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static bool iscont(cchar *lp,int ll) noex {
	bool		f = true ;
	f = f && (ll >= 2) ;
	f = f && (lp[ll - 1] == '\n') ;
	f = f && (lp[ll - 2] == '\\') ;
	return f ;
}


/* local variables */


/* exported variables */


/* exported subroutines */

int breadlns(bfile *op,char *lbuf,int llen,int to,int *lcp) noex {
	int		rs ;
	int		rl = 0 ;	/* result-buffer length */
	int		lines = 0 ;
	if ((rs = bfile_magic(op,lbuf)) > 0) {
	    if ((rs = bfile_ckrd(op)) >= 0) {
	        bool	f_cont = false ;
	        while ((lines == 0) || ((f_cont = ISCONT(lbuf,rl)))) {
	            if (f_cont) rl -= 2 ;
	            rs = breadlnto(op,(lbuf + rl),(llen - rl),to) ;
	            if (rs <= 0) break ;
	            rl += rs ;
	            lines += 1 ;
	        } /* end while */
	    } /* end if (readling) */
	} /* end if (magic) */
	if (lcp) *lcp  = lines ;
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (breadlns) */


