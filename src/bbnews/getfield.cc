/* getfield SUPPORT */
/* lang=C++20 */

/* an old subroutine to get extra message header fields */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-05-01, David A-D- Morano
	This code module was completely rewritten to replace any
	original garbage that was here before.  In other words, the
	same old story: old garbage code needing to be replaced
	in entirety just to get something working with new code
	interfaces.  Cannot we all think "reusuable"?

	= 1998-11-22, David A-D- Morano
        I did some clean-up.

*/

/* Copyright © 1995,1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************
 *									
 * The information contained herein is for the use of AT&T Information	
 * Systems Laboratories and is not for publications.  (See GEI 13.9-3)	
 *									
 *	(c) 1984 AT&T Information Systems				
 *									
 * Authors of the contents of this file:				
 *									
		David A-D- Morano

*									
*	FUNCTIONAL DESCRIPTION:						

*	The 'getfield' reads a file for a key descriptor and returns	
*	the value contained in it.
*									
*	PARAMETERS:							
*	file		filename					
*	key		key name (eg, 'FROM')				
*	valbuf		value for given key
	valbuflen	length of value buffer above
*									
*	RETURNED VALUE:							
		BAD	no header found
		OK	a header was found
*									
*	SUBROUTINES CALLED:						
		+ mm_getfield


	IMPORTANT NOTE:
	This subroutine is NOT RFC-822 compliant !!
	This is because 'mm_getfield()' is NOT RFC-822 compliant.
	We call that to do our work.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<climits>
#include	<ctype.h>
#include	<usystem.h>
#include	<bfile.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */

extern "C" {
    extern int	mm_getfield() noex ;
}


/* external variables */


/* exported variables */


/* exported subroutines */

int getfield(cchar *fname,cchar *key,char *vbuf,int vlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        bfile	mmfile, *fp = &mmfile ;
	        vbuf[0] = '\0' ;
	        if ((rs = bopen(fp,fname,"r",0666)) >= 0) {
	            USTAT	sb ;
	            if ((rs = bcontrol(fp,BC_STAT,&sb)) >= 0) {
		        cint	cfsize = (sb.st_size & INT_MAX) ;
		        rs = mm_getfield(fp,0L,fsize,key,vbuf,vlen) ;
	            } /* end if */
	            rs1 = bclose(fp) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (open-file) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getfield) */


