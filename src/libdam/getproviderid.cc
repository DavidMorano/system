/* getproviderid SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Get Provider (manufacturer) ID */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:

	Description:
	This is a small hack to get a number associated with a
	manufacturing provider.  Prividers are identified (from
	getting this information from the kernel) by a string.  This
	subroutine looks this string up and returns the corresponding
	number.

	Synopsis:
	int getproviderid(char *np,int nl) noex

	Arguments:
	np		pointer to provider string (to lookup)
	nl		length of given provider string

	Returns:
	>0		provider ID was found and this is it
	0		provider ID was not found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<estrings.h>
#include	<nleadstr.h>
#include	<localmisc.h>


/* external subroutines */

extern "C" {
    extern int	getprovider(char *,int) noex ;
}


/* external variables */


/* local structures */

struct provider {
	uint		providerid ;
	char		*codename ;
	char		*realname ;
} ;


/* forward references */


/* local variables */

constexpr provider	providers[] = {
	{ 0, "unknown", "Unknown" }, 
	{ 1, "Sun_Microsystems", "Sun Microsystems" },
	{ 2, "Compaq Computer Corporation", NULL },
	{ 3, "sgi", "Silicon Graphics" },
	{ 4, NULL, NULL }
} ;


/* exported variables */


/* exported subroutines */

int getproviderid(cchar *np,int nl) noex {
	int		i ;
	int		m ;
	int		id = 0 ;
	int		f = FALSE ;
	cchar		*bs ;

	if (nl < 0) nl = strlen(np) ;

	for (i = 0 ; providers[i].codename != NULL ; i += 1) {
	    bs = providers[i].codename ;
	    m = nleadstr(bs,np,nl) ;
	    f = ((m == nl) && (bs[m] == '\0')) ;
	    if (f) break ;
	} /* end for */

	if (f) id = providers[i].providerid ;

	return id ;
}
/* end subroutine (getproviderid) */

int getvendor(char *rbuf,int rlen) noex {
	cint		plen = MAXNAMELEN ;
	int		rs ;
	char		pbuf[MAXNAMELEN+1] ;
	rbuf[0] = '\0' ;
	if ((rs = getprovider(pbuf,plen)) >= 0) {
	    cint	pl = rs ;
	    int		m, i ;
	    int		f = FALSE ;
	    rs = SR_NOTFOUND ;
	    for (i = 0 ; providers[i].codename != NULL ; i += 1) {
	        cchar	*pn = providers[i].codename ;
	        m = nleadstr(pn,pbuf,pl) ;
	        f = ((m == pl) && (pn[m] == '\0')) ;
	        if (f) break ;
	    } /* end for */
	    if (f) {
		rs = sncpy1(rbuf,rlen,providers[i].realname) ;
	    }
	}
	return rs ;
}
/* end subroutine (getvendor) */


