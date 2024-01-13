/* mkaltext SUPPORT */
/* lang=C++20 */

/* make a file-name w/ an alternate extension */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	The subroutine was written from scratch.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkalttext

	Description:
	We take a file-name and create a new file-name replacing
	the existing extension with the new (alternative) extension
	supplied.

	Synopsis:
	int mkaltext(char *dbuf,cchar *name,cchar *ext) noex

	Arguments:
	dbuf		result buffer pointer
	name		source base-file-name
	ext		alternative extension

	Returns:
	>=0		length of result string
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstdlib>
#include	<cstring>		/* for |strrchr(3c)| */
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<sbuf.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp) ;


/* exported variables */


/* exported subroutines */

int mkaltext(char *dbuf,cchar *name,cchar *ext) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (dbuf && name && ext) {
	    rs = SR_INVALID ;
	    if (name[0]) {
	        cchar	*tp ;
	        if ((tp = strrchr(name,'.')) != nullptr) {
	            if (tp[1] != '\0') {
			if ((rs = maxpathlen) >= 0) {
	                    sbuf	alt ;
	                    cint	dlen = rs ;
	                    if ((rs = sbuf_start(&alt,dbuf,dlen)) >= 0) {
		                {
	                            sbuf_strw(&alt,name,(tp - name)) ;
	                            sbuf_char(&alt,'.') ;
	                            sbuf_strw(&alt,ext,-1) ;
		                }
	                        len = sbuf_finish(&alt) ;
	                        if (rs >= 0) rs = len ;
	                    } /* end if (sbuf) */
			} /* end if (maxpathlen) */
	            } /* end if (had extension) */
	        } /* end if */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mkaltext) */


