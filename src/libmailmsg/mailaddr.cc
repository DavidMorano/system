/* mailaddr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* parse email route addresses into host and local parts */
/* version %I% last-modified %G% */


/* revision history:

	= 2002-05-30, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2002 David A­D­ Morano.  All rights reserved. */

/*****************************************************************************

  	Group:
	mailaddr

	Description:
	This subroutine will parse email route addresses into
	hostname and localname parts.  The assumption is that only
	route addresses are given to us.  If this is wrong, the
	results are indeterminate.  The hostname part is just the
	first host in the address as if the "focus" (using SENDMAIL
	language) was on the first host.

	Synopsis:
	int mailaddrparse(dp,dl,mahost,malocal)
	int mailaddrjoin(dp,dl,mahost,malocal,type)
	int mailaddrarpa(dp,dl,mahost,malocal,type)

	Arguments:
	dp		string buffer containing route address
	dl		length of string buffer
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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<getbufsize.h>		/* for |getbufsize_mailaddr| */
#include	<bufsizevar.hh>
#include	<storebuf.h>
#include	<strn.h>
#include	<sncpyx.h>
#include	<snwcpy.h>
#include	<localmisc.h>

#include	"mailaddr.h"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int		hostnamelen ;
	int		mailaddrlen ;
	operator int () noex ;
    } ; /* end struct (val) */
} /* end namespace */


/* forward references */


/* local variables */

static vars		var ;

constexpr char		localhostpart[] = MAILADDR_LOCALHOST ;


/* exported variables */


/* exported subroutines */

int mailaddrparse(cc *sp,int sl,char *mahost,char *malocal) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		t = 0 ;
	if (sp && mahost && malocal) {
	    static cint		rsv = var ;
	    mahost[0] = '\0' ;
	    malocal[0] = '\0' ;
	    if ((rs = rsv) >= 0) {
	        cint	hnl = var.hostnamelen ; /* host-name length */
	        cint	mal = var.mailaddrlen ; /* mail-address length */
		cchar	*tp ; /* used-multiple */
	        rs = SR_OK ;
	        if (sl < 0) sl = lenstr(sp) ;
	        /* what kind of address do we have? */
	        if (cc *cp1 ; (cp1 = strnchr(sp,sl,'@')) != np) {
	            if (cc *cp2 ; (cp2 = strnchr(sp,sl,':')) != np) {
		        /* ARPAnet route address */
	                t = MAILADDRTYPE_ARPAROUTE ;
	                if ((tp = strnchr(sp,sl,',')) != np) {
	                    if (rs >= 0) {
			        cint	vlen = intconv(tp - (cp1 + 1)) ;
			        cchar	*vstr = (cp1 + 1) ;
	                        rs = snwcpy(mahost,hnl,vstr,vlen) ;
		            }
	                    if (rs >= 0) {
	                        rs = sncpy1(malocal,mal,(tp + 1)) ;
		            }
	                } else {
	                    if (rs >= 0) {
			        cint	vlen = intconv(cp2 - (cp1 + 1)) ;
			        cchar	*vstr = (cp1 + 1) ;
	                        rs = snwcpy(mahost,hnl,vstr,vlen) ;
		            }
	                    if (rs >= 0) {
	                        rs = sncpy1(malocal,mal,(cp2 + 1)) ;
		            }
	                } /* end if */
	            } else {
		        /* normal ARPAnet address */
	                t = MAILADDRTYPE_ARPA ;
	                if (rs >= 0) {
	                    rs = sncpy1(mahost,hnl,(cp1 + 1)) ;
		        }
	                if (rs >= 0) {
			    cint	tl = intconv(cp1 - sp) ;
	                    rs = snwcpy(malocal,mal,sp,tl) ;
		        }
	            } /* end if */
	        } else if ((tp = strnrchr(sp,sl,'!')) != nullptr) {
	            t = MAILADDRTYPE_UUCP ;
	            if (rs >= 0) {
			cint	tl = intconv(tp - sp) ;
	                rs = snwcpy(mahost,hnl,sp,tl) ;
	            }
	            if (rs >= 0) {
	                rs = sncpy(malocal,mal,(tp + 1)) ;
	            }
	        } else {
	           /* local */
	            t = MAILADDRTYPE_LOCAL ;
	            if (rs >= 0) {
	                rs = sncpy(mahost,hnl,localhostpart) ;
	            }
	            if (rs >= 0) {
	                rs = snwcpy(malocal,mal,sp,sl) ;
	            }
	        } /* end if */
	        /* perform some cleanup (remove stupid trailing dots) */
	        if (rs >= 0) {
	            int		cl = lenstr(mahost) ;
	            while ((cl > 0) && (mahost[cl - 1] == '.')) {
	                cl -= 1 ;
	            }
	            mahost[cl] = '\0' ;
	        } /* end if (ok) */
	    } /* end if (vars) */
	} /* end if (non-null) */
	return (rs >= 0) ? t : rs ;
}
/* end subroutine (mailaddrparse) */

/* put an address back together as it was ORIGINALLY */
int mailaddrjoin(char *dp,int dl,cc *mahost,cc *malocal,int type) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (dp && mahost && malocal) {
	    rs = SR_INVALID ;
	    if (type >= 0) {
		storebuf	b(dp,dl) ;
		rs = SR_OK ;
	        switch (type) {
	        case MAILADDRTYPE_LOCAL:
	  	    {
	                rs = b.str(malocal) ;
		    }
	            break ;
	        case MAILADDRTYPE_UUCP:
		    {
	                rs = b.str(mahost) ;
		    }
	            if (rs >= 0) {
	                rs = b.strw("!",1) ;
		    }
	            if (rs >= 0) {
	                rs = b.str(malocal) ;
		    }
	            break ;
	        case MAILADDRTYPE_ARPA:
		    {
	                rs = b.str(malocal) ;
		    }
	            if (rs >= 0) {
	                rs = b.strw("@",1) ;
		    }
	            if (rs >= 0) {
	                rs = b.str(mahost) ;
		    }
	            break ;
	        case MAILADDRTYPE_ARPAROUTE:
		    {
	                rs = b.strw("@",1) ;
		    }
	            if (rs >= 0) {
	                rs = b.str(mahost) ;
		    }
	            if (rs >= 0) {
	                if (strchr(malocal,':') != nullptr) {
	                    rs = b.strw(",",1) ;
	                } else {
	                    rs = b.strw(":",1) ;
		        }
	            }
	            if (rs >= 0) {
	                rs = b.str(malocal) ;
		    }
	            break ;
	        } /* end switch */
		len = b.idx ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mailaddjoin) */

/* put an address back together as its ARPA form only */
int mailaddrarpa(char *dp,int dl,cc *mahost,cc *malocal,int type) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (dp && mahost && malocal) {
	    rs = SR_INVALID ;
	    if (type >= 0) {
		storebuf	b(dp,dl) ;
	        rs = SR_OK ;
	        switch (type) {
	        case MAILADDRTYPE_LOCAL:
	            if (rs >= 0) {
	                rs = b.str(malocal) ;
		    }
	            break ;
	        case MAILADDRTYPE_UUCP:
	            if (rs >= 0) {
	                rs = b.str(malocal) ;
		    }
	            if (rs >= 0) {
	                rs = b.strw("@",1) ;
		    }
	            if (rs >= 0) {
	                rs = b.str(mahost) ;
		    }
	            break ;
	        case MAILADDRTYPE_ARPA:
	            if (rs >= 0) {
	                rs = b.str(malocal) ;
		    }
	            if (rs >= 0) {
	                rs = b.strw("@",1) ;
		    }
	            if (rs >= 0) {
	                rs = b.str(mahost) ;
		    }
	            break ;
	        case MAILADDRTYPE_ARPAROUTE:
		    if (rs >= 0) {
	                rs = b.strw("@",1) ;
		    }
	            if (rs >= 0) {
	                rs = b.str(mahost) ;
		    }
	            if (rs >= 0) {
	                if (strchr(malocal,':') != nullptr) {
	                    rs = b.strw(",",1) ;
	                } else {
	                    rs = b.strw(":",1) ;
			}
	            }
	            if (rs >= 0) {
	                rs = b.str(malocal) ;
		    }
	            break ;
	        } /* end switch */
		len = b.idx ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mailaddrarpa) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = getbufsize(getbufsize_hn)) >= 0) {
	    hostnamelen = rs ;
	    if ((rs = getbufsize(getbufsize_mailaddr)) >= 0) {
	        mailaddrlen = rs ;
	    }
	}
	return rs ;
}
/* end method (vars::operator) */


