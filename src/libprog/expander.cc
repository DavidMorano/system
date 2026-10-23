/* expander SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* expand out some per program parameters */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debugging */

/* revision history:

	= 1998-09-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	expander

	Description:
        This subroutine expands out some per program (the daemon program)
        parameters into the configuration strings. Actually this little
        subroutine is used by many programs.


#	The following substitutions are made on command strings:

#		%V	version string
#		%B	programroot basename
#		%P	program name
#		%S	program search name
#		%N	machine node name
#		%D	machine INET domain name
#		%H	machine INET host name
#		%U	invoking username
#		%G	invoking groupname
#		%R	program root directory
#

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<netdb.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<ucmem.h>		/* LIBUC */
#include	<snx.h>			/* LIBUC */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"proginfo.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* local namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct exper {
	PI		*pip ;
	charp		rbuf ;
	ccharp		sp ;
	int		rlen ;
	int		sl ;
	exper(PI *p,char *rb,int rl,ccp ªsp,int ªsl) noex {
	    pip		= p ;
	    rbuf	= rb ;
	    sp		= ªsp ;
	    sl		= ªsl ;
	    rlen	= rl ;
	} ; /* end ctor */
	operator int	() noex ;
	int proc	() noex ;
    } ; /* end struct (exper) */
} /* end namespace */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int expander(PI *pip,char *rbuf,int rlen,cchar *sp,int µsl) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ; /* return-value */
	if (pip && rbuf && sp) ylikely {
	    if (int sl ; (sl = getlenstr(sp,µsp)) >= 0) ylikely {
		exer eo(pip,rbuf,rlen,sp,sl) ;
		rs = eo ;
	    }
	} /* end if (non-null) */
} /* end subroutine (expander) */


/* local subroutines */

exer::operator int () noex {
    	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (char *hname ; (rs = mem.hostname(&hbuf)) >= 0) {
	    {
		rs = proc(hbuf,rs) ;
		len = rs ;
	    }
	    rs1 = mem.free(hbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
} /* end method (exer::operator) */

int exer::proc(char *hbuf,int hlen) noex {
    	int		rs = SR_OK ;
	int		cl ;
	int		elen = 0 ;
	cchar		*cp ;
	char		*rbp = rbuf ;

	DEBUGPRINTF("ent buflen=%d rbuflen=%d\n",buflen,rlen) ;
	DEBUGPRINTF("sp=>%r<\n",sp,sl);

	while ((sl > 0) && *sp && (elen < rlen)) {
	    switch (int ch = MKCHAR(*sp) ; ch) {
	    case '%':
	        sp += 1 ;
	        sl -= 1 ;
	        if (sl == 0) break ;
		cl = 0 ;
	        switch (ch) {
	        case 'V':
	            cp = pip->version ;
	            cl = lenstr(cp) ;
	            break ;
#if	defined(DEFS_ROOTNAME) && (DEFS_ROOTNAME > 0)
	        case 'B':
		    if ((cp = pip->rootname) != nullptr) {
	            	cl = lenstr(cp) ;
		    }
	            break ;
#endif /* P_RCPMUXD */
	        case 'P':
	            cp = pip->progname ;
	            cl = lenstr(cp) ;
	            break ;
	        case 'S':
		    if ((cp = pip->searchname) != nullptr) {
	            	cl = lenstr(cp) ;
		    }
	            break ;
	        case 'N':
	            cp = pip->nodename ;
	            cl = lenstr(cp) ;
	            break ;
	        case 'D':
	            cp = pip->domainname ;
	            cl = lenstr(cp) ;
	            break ;
	        case 'H':
	            cp = hbuf ;
	            cl = snsds(hbuf,hlen,pip->nodename,pip->domainname) ;
	            break ;
/* handle the expansion of our program root */
	        case 'R':
	            if ((cp = pip->pr) != nullptr) {
	                cl = lenstr(cp) ;
		    }
	            break ;
	        case 'U':
		    if ((cp = pip->username) != nullptr) {
			cl = lenstr(cp) ;
		    }
	            break ;
	        case 'G':
		    if ((cp = pip->groupname) != nullptr) {
			cl = lenstr(cp) ;
		    }
	            break ;
	        default:
	            cp = sp ;
	            cl = 1 ;
		    break ;
	        } /* end switch */
	        sp += 1 ;
	        sl -= 1 ;
	        if ((elen + cl) > rlen) {
		    rs = SR_OVERFLOW ;
	            break ;
		}
	        if (cl > 0) strncpy(rbp,cp,cl) ;
	        rbp += cl ;
	        elen += cl ;
	        break ;
	    default:
	        *rbp++ = *sp++ ;
	        sl -= 1 ;
	        elen += 1 ;
		break ;
	    } /* end switch */
	    if (rs < 0) break ;
	} /* end while */
	if ((rs >= 0) && (elen >= rlen)) {
	    rs = SR_OVERFLOW ;
	} /* end if (error) */
	rbuf[elen] = '\0' ;
	DEBUGPRINTF("expander: ret rs=%d elen=%d\n",rs,elen) ;
	DEBUGPRINTF("expnader: ebuf=>%r<\n",rbuf,elen) ;
	return (rs >= 0) ? elen : rs ;
} /* end method (exer::proc) */


