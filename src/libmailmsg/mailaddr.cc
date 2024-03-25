/* mailaddr SUPPORT */
/* lang=C++20 */

/* parse email route addresses into host and local parts */
/* version %I% last-modified %G% */


/* revision history:

	= 2002-05-30, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2002 David A­D­ Morano.  All rights reserved. */

/*****************************************************************************

	This subroutine will parse email route addresses into
	hostname and localname parts.  The assumption is that only
	route addresses are given to us.  If this is wrong, the
	results are indeterminate.  The hostname part is just the
	first host in the address as if the "focus" (using SENDMAIL
	language) was on the first host.

	Synopsis:

	int mailaddrparse(dp,dl,mahost,malocal)
	cchar	buf[] ;
	int		buflen ;
	char		mahost[], malocal[] ;

	Synopsis:
	int mailaddrjoin(dp,dl,mahost,malocal,type)
	char		buf[] ;
	int		buflen ;
	cchar	mahost[], malocal[] ;
	int		type ;

	Synopsis:
	int mailaddrarpa(dp,dl,mahost,malocal,type)
	char		buf[] ;
	int		buflen ;
	cchar	mahost[], malocal[] ;
	int		type ;

	Arguments:
	buf		string buffer containing route address
	buflen		length of string buffer
	mahost	supplied buffer to receive mahost
	malocal	supplied buffer to receive malocal
	type		type of address desired

	Returns:
	0		local address
	1		UUCP
	2		ARPAnet normal
	3		ARPAnet route address
	<0		bad address of some kind

******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<storebuf.h>
#include	<strn.h>
#include	<sncpyx.h>
#include	<snwcpy.h>
#include	<localmisc.h>

#include	"mailaddr.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* local variables */


/* exported variables */


/* exported subroutines */

int mailaddrparse(cc *sp,int sl,char *mahost,char *malocal) noex {
	int		rs = SR_OK ;
	int		t ;
	int		mal = MAILADDRLEN ;
	cchar	*localhostpart = MAILADDR_LOCALHOST ;
	cchar	*cp ;
	cchar	*cp1, *cp2 ;

	if (sp == nullptr) return SR_FAULT ;
	if (mahost == nullptr) return SR_FAULT ;
	if (malocal == nullptr) return SR_FAULT ;

	mahost[0] = '\0' ;
	malocal[0] = '\0' ;
	if (sl < 0) sl = strlen(sp) ;

/* what kind of address do we have? */

	if ((cp1 = strnchr(sp,sl,'@')) != nullptr) {
	    if ((cp2 = strnchr(sp,sl,':')) != nullptr) {
/* ARPAnet route address */
	        t = MAILADDRTYPE_ARPAROUTE ;
	        if ((cp = strnchr(sp,sl,',')) != nullptr) {
	            if (rs >= 0) {
	                rs = snwcpy(mahost,mal,(cp1 + 1),(cp - (cp1 + 1))) ;
		    }
	            if (rs >= 0) {
	                rs = sncpy1(malocal,mal,(cp + 1)) ;
		    }
	        } else {
	            if (rs >= 0) {
	                rs = snwcpy(mahost,mal,(cp1 + 1),cp2 - (cp1 + 1)) ;
		    }
	            if (rs >= 0) {
	                rs = sncpy1(malocal,mal,(cp2 + 1)) ;
		    }
	        } /* end if */
	    } else {
/* normal ARPAnet address */
	        t = MAILADDRTYPE_ARPA ;
	        if (rs >= 0) {
	            rs = sncpy1(mahost,mal,(cp1 + 1)) ;
		}
	        if (rs >= 0) {
	            rs = snwcpy(malocal,mal,sp,(cp1 - sp)) ;
		}
	    } /* end if */
	} else if ((cp = strnrchr(sp,sl,'!')) != nullptr) {
	    t = MAILADDRTYPE_UUCP ;
	    if (rs >= 0) {
	        rs = snwcpy(mahost,mal,sp,(cp - sp)) ;
	    }
	    if (rs >= 0) {
	        rs = sncpy1(malocal,mal,(cp + 1)) ;
	    }
	} else {
/* local */
	    t = MAILADDRTYPE_LOCAL ;
	    if (rs >= 0) {
	        rs = sncpy1(mahost,mal,localhostpart) ;
	    }
	    if (rs >= 0) {
	        rs = snwcpy(malocal,mal,sp,sl) ;
	    }
	} /* end if */
/* perform some cleanup on host-domain part (remove stupid trailing dots) */
	if (rs >= 0) {
	    int		cl = strlen(mahost) ;
	    while ((cl > 0) && (mahost[cl - 1] == '.')) {
	        cl -= 1 ;
	    }
	    mahost[cl] = '\0' ;
	} /* end if (ok) */

	return (rs >= 0) ? t : rs ;
}
/* end subroutine (mailaddrparse) */

/* put an address back together as it was ORIGINALLY */
int mailaddrjoin(char *dp,int dl,cc *mahost,cc *malocal,int type) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (dp && mahost && malocal) {
	    rs = SR_INVALID ;
	    if (type >= 0) {
		rs = SR_OK ;
	        switch (type) {
	        case MAILADDRTYPE_LOCAL:
	  	    {
	                rs = storebuf_strw(dp,dl,i,malocal,-1) ;
	                i += rs ;
		    }
	            break ;
	        case MAILADDRTYPE_UUCP:
		    {
	                rs = storebuf_strw(dp,dl,i,mahost,-1) ;
	                i += rs ;
		    }
	            if (rs >= 0) {
	                rs = storebuf_strw(dp,dl,i,"!",1) ;
	                i += rs ;
		    }
	            if (rs >= 0) {
	                rs = storebuf_strw(dp,dl,i,malocal,-1) ;
	                i += rs ;
		    }
	            break ;
	        case MAILADDRTYPE_ARPA:
		    {
	                rs = storebuf_strw(dp,dl,i,malocal,-1) ;
	                i += rs ;
		    }
	            if (rs >= 0) {
	                rs = storebuf_strw(dp,dl,i,"@",1) ;
	                i += rs ;
		    }
	            if (rs >= 0) {
	                rs = storebuf_strw(dp,dl,i,mahost,-1) ;
	                i += rs ;
		    }
	            break ;
	        case MAILADDRTYPE_ARPAROUTE:
		    {
	                rs = storebuf_strw(dp,dl,0, "@",1) ;
	                i += rs ;
		    }
	            if (rs >= 0) {
	                rs = storebuf_strw(dp,dl,i,mahost,-1) ;
	                i += rs ;
		    }
	            if (rs >= 0) {
	                if (strchr(malocal,':') != nullptr) {
	                    rs = storebuf_strw(dp,dl,i,",",1) ;
	                } else {
	                    rs = storebuf_strw(dp,dl,i,":",1) ;
	                    i += rs ;
		        }
	            }
	            if (rs >= 0) {
	                rs = storebuf_strw(dp,dl,i,malocal,-1) ;
	                i += rs ;
		    }
	            break ;
	        } /* end switch */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (mailaddjoin) */

/* put an address back together as its ARPA form only */
int mailaddrarpa(char *dp,int dl,cc *mahost,cc *malocal,int type) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (dp && mahost && malocal) {
	    rs = SR_INVALID ;
	    if (type >= 0) {
	        rs = SR_OK ;
	        switch (type) {
	        case MAILADDRTYPE_LOCAL:
	            if (rs >= 0) {
	                rs = storebuf_strw(dp,dl,i,malocal,-1) ;
	                i += rs ;
		    }
	            break ;
	        case MAILADDRTYPE_UUCP:
	            if (rs >= 0) {
	                rs = storebuf_strw(dp,dl,i,malocal,-1) ;
	                i += rs ;
		    }
	            if (rs >= 0) {
	                rs = storebuf_strw(dp,dl,i,"@",1) ;
	                i += rs ;
		    }
	            if (rs >= 0) {
	                rs = storebuf_strw(dp,dl,i,mahost,-1) ;
	                i += rs ;
		    }
	            break ;
	        case MAILADDRTYPE_ARPA:
	            if (rs >= 0) {
	                rs = storebuf_strw(dp,dl,i,malocal,-1) ;
	                i += rs ;
		    }
	            if (rs >= 0) {
	                rs = storebuf_strw(dp,dl,i,"@",1) ;
	                i += rs ;
		    }
	            if (rs >= 0) {
	                rs = storebuf_strw(dp,dl,i,mahost,-1) ;
	                i += rs ;
		    }
	            break ;
	        case MAILADDRTYPE_ARPAROUTE:
		    if (rs >= 0) {
	                rs = storebuf_strw(dp,dl,i,"@",1) ;
	                i += rs ;
		    }
	            if (rs >= 0) {
	                rs = storebuf_strw(dp,dl,i,mahost,-1) ;
	                i += rs ;
		    }
	            if (rs >= 0) {
	                if (strchr(malocal,':') != nullptr) {
	                    rs = storebuf_strw(dp,dl,i,",",1) ;
	                } else
	                    rs = storebuf_strw(dp,dl,i,":",1) ;
	                i += rs ;
	            }
	            if (rs >= 0) {
	                rs = storebuf_strw(dp,dl,i,malocal,-1) ;
	                i += rs ;
		    }
	            break ;
	        } /* end switch */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (mailaddrarpa) */


