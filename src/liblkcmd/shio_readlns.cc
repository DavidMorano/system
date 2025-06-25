/* shio_readlines SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get a line with possible continuation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	Written a new out of frustration of previous hacks.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	shio_readlns

	Description:
	This is a SHIO subroutine that reads line that may be
	continued onto the next real line using the back-slash
	character.

	Synopsis:
	int shio_readlines(shio *fp,char *lbuf,int llen,int *lcp) noex

	Arguments:
	fp		pointer to object
	lbuf		user buffer to receive data
	llen		length of user supplied buffer
	lcp		pointer to integer to receive number of lines read

	Returns:
	>=0		number of bytes read
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<localmisc.h>

#include	"shio.h"


/* local defines */

#define	ISCONT(b,bl)	\
	(((bl) >= 2) && ((b)[(bl) - 1] == '\n') && ((b)[(bl) - 2] == '\\'))


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int shio_readlns(shio *fp,char *lbuf,int llen,int *lcp) noex {
	int		rs = SR_OK ;
	int		alen ;			/* "add" length */
	int		rl = 0 ;
	bool		f_first = true ;
	bool		f_cont = false ;

	lbuf[0] = '\0' ;
	while (f_first || (f_cont = ISCONT(lbuf,i))) {
	    f_first = false ;
	    if (f_cont) rl -= 2 ;
	    alen = (llen - rl) ;
	    rs = shio_readln(fp,(lbuf + i),alen) ;
	    if (rs <= 0) break ;
	    rl += rs ;
	    if (lcp) *lcp += 1 ;
	} /* end while */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (shio_readlines) */


