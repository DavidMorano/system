/* breadlns SUPPORT */
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
	int breadlns(bfile *fp,char *lbuf,int llen,int to,int *lcp) noex

	Arguments:
	fp		pointer to object
	lbuf		user buffer to receive data
	llen		length of user supplied buffer
	to		time-out (to=-1 mean "infinite")
	lcp		pointer to integer to receive number of lines read

	Returns:
	>=0		number bytes returned to caller (not bytes read)
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */

#define	TO_READ		(5*60)

#define	ISCONT(b,bl)	\
	(((bl) >= 2) && ((b)[(bl) - 1] == '\n') && ((b)[(bl) - 2] == '\\'))


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int breadlns(bfile *fp,char *lbuf,int llen,int to,int *lcp) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;		/* result-buffer length */
	int		lines = 0 ;
	if (fp && lbuf) {
	    rs = SR_NOTOPEN ;
	    lbuf[0] = '\0' ;
	    if (fp->magic == BFILE_MAGIC) {
		rs = SR_OK ;
	        bool	f_cont = false ;
	        while ((lines == 0) || (f_cont = ISCONT(lbuf,i))) {
	            if (f_cont) i -= 2 ;
	            rs = breadlnto(fp,(lbuf + i),(llen - i),to) ;
	            if (rs <= 0) break ;
	            i += rs ;
	            lines += 1 ;
	        } /* end while */
	    } /* end if (open) */
	} /* end if (non-null) */
	if (lcp) *lcp  = lines ;
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (breadlns) */


