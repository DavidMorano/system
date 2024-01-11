/* mkmid SUPPORT */
/* lang=C20 */

/* create a mail message ID */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A.D. Morano
	This subroutine is originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkmid

	Description:
	This subroutine is used to create a mail message ID for
	certain PCS programs.

	Synopsis:
	int mkmid(char *rbuf,int rlen,cc *dn,cc *nn,pid_t pid,int serial) noex

	Arguments:
	rbuf		caller-supplied buffer to place result in
	rlen		length of caller supplied buffer
	dn		domain-name
	nn		node-name
	pid		process ID
	serial		serial number

	Returns:
	>=0		length of returned ID
	<0		error in process of creating ID (system-return)

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<time.h>
#include	<usystem.h>
#include	<sbuf.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int mkmid(char *rbuf,int rlen,cchar *dn,cchar *nn,pid_t pid,int serial) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (rbuf && dn && nn) {
	    rs = SR_INVALID ;
	    if (dn[0] && nn[0]) {
	        sbuf	mb ;
	        if ((rs = sbuf_start(&mb,rbuf,rlen)) >= 0) {
	            const time_t	dt = time(NULL) ;
	            uint		uv = (uint) pid ;
	            sbuf_strw(&mb,nn,-1) ;
	            sbuf_decui(&mb,uv) ;
	            sbuf_char(&mb,'.') ;
	            {
		        uv = (uint) dt ;
	                sbuf_hexui(&mb,uv) ;
	            }
	            sbuf_char(&mb,'.') ;
	            sbuf_deci(&mb,serial) ;
	            sbuf_char(&mb,'@') ;
	            sbuf_strw(&mb,dn,-1) ;
	            rs1 = sbuf_finish(&mb) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (sbuf) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mkmid) */


