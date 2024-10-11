/* comsatmsg SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* create and parse COMSAT messages */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright Â© 1999 David AÂ­DÂ­ Morano.  All rights reserved. */

/*******************************************************************************

	This module contains the code to make and parse Comsat
	messages.  A Comsat message looks like:
		<user>@<offset>[:<file>]

	Where:
		<user>		is the user who received the message
		<offset>	is the file-offset to this message
		<file>		associated filename (If present)

	Example:
		dam@251:bobby


	Description:
	This subroutine is used to create or to parse a Comsat message.
		
	Synopsis:
	int comsatmsg_mo(COMSATMSG_MO *msp,int f_read,char *mbuf,int mlen) noex
		
	Arguments:
		msp		pointer to COMSATMSG_MO object (the result)
		f_read		flag to indicate if we are parsing (read)
				or creating (write)
		mbuf		string buffer (input for parseing,
				output for creating
		mlen		length of string buffer

	Returns:
		<0		error
		>=0		for parsing
		>0		lrngth of result when creating

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/param.h>
#include	<unistd.h>
#include	<usystem.h>
#include	<sbuf.h>
#include	<strn.h>
#include	<sncpyx.h>
#include	<mkpathx.h>
#include	<cfdec.h>
#include	<char.h>
#include	<localmisc.h>

#include	"comsatmsg.h"


/* local defines */


/* external subroutines */


/* local structures */


/* local variables */


/* exported variables */


/* exported subroutines */

int comsatmsg_mo(COMSATMSG_MO *msp,int f_read,char *mbuf,int mlen) noex {
	sbuf		msgbuf ;
	ulong		ulv ;
	int		rs ;
	int		rs1 ;
	if (f_read) { /* read */
	    int		cl ;
	    int		sl = mlen ;
	    cchar	*tp, *cp ;
	    cchar	*sp = mbuf ;
	    msp->offset = 0 ;
	    msp->username[0] = '\0' ;
	    msp->fname[0] = '\0' ;
	    while ((sl > 0) && CHAR_ISWHITE(sp[sl - 1])) {
	        sl -= 1 ;
	    }
	    if ((tp = strnchr(sp,sl,'@')) != NULL) {
	        cint	ulen = USERNAMELEN ;
	        char	*ubuf = msp->username ;
	        if ((rs = sncpy1w(ubuf,ulen,sp,(tp-sp))) >= 0) {
	            sl -= ((tp + 1) - mbuf) ;
	            sp = (tp + 1) ;
	            cp = sp ;
	            cl = sl ;
	            if ((tp = strnchr(sp,sl,':')) != NULL) {
	                cl = (tp - sp) ;
	                if (rs >= 0) {
	                    char	*fbuf = msp->fname ;
	                    rs = mkpath1w(fbuf,(tp+1),((sp+sl)-(tp+1))) ;
	                }
	            }
	            if (rs >= 0) {
	                rs = cfdecul(cp,cl,&ulv) ;
	                msp->offset = ulv ;
	            }
	        } /* end if */
	    } else {
	        rs = SR_BADMSG ;
	    }
	} else { /* write */
	    if ((rs = sbuf_start(&msgbuf,mbuf,mlen)) >= 0) {
	        sbuf_strw(&msgbuf,msp->username,-1) ;
	        sbuf_chr(&msgbuf,'@') ;
	        ulv = msp->offset ;
	        sbuf_decul(&msgbuf,ulv) ;
	        if (msp->fname[0] != '\0') {
	            sbuf_chr(&msgbuf,':') ;
	            sbuf_strw(&msgbuf,msp->fname,-1) ;
	        } /* end if */
	        rs1 = sbuf_finish(&msgbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sbuf) */
	} /* end if */
	return rs ;
}
/* end subroutine (comsatmsg_mo) */


