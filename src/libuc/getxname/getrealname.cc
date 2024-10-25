/* getrealname SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* try to find the real-name of a user (given a username) */
/* version %I% last-modified %G% */

#define	CF_PWCACHE	1		/* use |ugetpw(3uc)| */

/* revision history:

	= 2003-10-01, David A­D­ Morano
	This was written to consolidate this type of code that was
	previously done separately in different places.  This code
	only makes sense if access to the user ucentpw record is
	*only* needed to get the GECOS "name" field.  If more general
	access of the user ucentpw record is needed, something other
	than this should be used.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getrealname

	Description:
	This subroutine will try to find a real-name given a username.  If just
	queries the ucentpw-GECOS information.

	Synopsis:
	int getrealname(char *rbuf,int rlen,cchar *un) noex

	Arguments:
	rbuf		supplied buffer to receive the program-root
	rlen		length of supplied buffer
	un		username

	Returns:
	>=0		length of result
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<ucpwcache.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<getusername.h>
#include	<getax.h>
#include	<mkx.h>			/* |getgecosname(3uc)| */
#include	<localmisc.h>

#include	"getrealname.h"


/* local defines */

#if	CF_PWCACHE
#define	GETPW_NAME	ucpwcache_name
#else
#define	GETPW_NAME	getpw_name
#endif /* CF_PWCACHE */
#undef	COMMENT

#ifndef	REALNAMELEN
#define	REALNAMELEN	100		/* real name length */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int getpwname(ucentpw *,char *,int,cchar *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int getrealname(char *rbuf,int rlen,cchar *un) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rl = 0 ;
	if (rlen < 0) rlen = REALNAMELEN ;
	if (rbuf) {
	    char	*pwbuf{} ;
	    if ((rs = malloc_pw(&pwbuf)) >= 0) {
	        ucentpw	pw ;
	        cint	pwlen = rs ;
	        if ((rs = getpwname(&pw,pwbuf,pwlen,un)) >= 0) {
	            cchar	*gp{} ;
	            if ((rs = getgecosname(pw.pw_gecos,-1,&gp)) > 0) {
		        rs = mkrealname(rbuf,rlen,gp,rs) ;
			rl = rs ;
	            } /* end if (getgecosname) */
	        } /* end if (getpwname) */
	        rs1 = uc_free(pwbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (getrealname) */


/* local subroutines */

static int getpwname(ucentpw *pwp,char *pwbuf,int pwlen,cchar *un) noex {
	int		rs ;
	if (un && (un[0] != '\0') && (un[0] != '-')) {
	    rs = GETPW_NAME(pwp,pwbuf,pwlen,un) ;
	} else {
	    rs = getpwusername(pwp,pwbuf,pwlen,-1) ;
	}
	return rs ;
}
/* end if (getpwname) */


