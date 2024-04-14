/* pathclean SUPPORT */
/* lang=C++20 */

/* cleanup a path */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This was written as part of the FILECHECK utility.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	pathclean

	Description:
	This subroutine takes a path and "cleans" it up by removing
	certain redundant or otherwise unnecessary elements.

	Synopsis:
	int pathclean(char *rbuf,cchar *pbuf,int plen) noex

	Arguments:
	rbuf		buffer to receive cleaned up path string
	pbuf		supplied path to clean up
	plen		length of supplied path to clean up

	Returns:
	>=0		length of cleaned up path
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"pathclean.h"


/* local defines */

#define	PATHBUF		struct pathbuf

#define	PATHBUF_BUFP	pbuf
#define	PATHBUF_BUFPL	plen
#define	PATHBUF_IDX	i


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* local structures */

namespace {
    struct pathbuf {
	char		*pbuf{} ;
	int		plen{} ;
	int		i{} ;
	int start(char *,int) noex ;
	int chr(int) noex ;
	int strw(cchar *,int) noex ;
	int remslash() noex ;
	int finish() noex ;
    } ; /* end struct (pathbuf) */
}


/* local structures */


/* forward references */

static int	nextname(cchar *,int,cchar **) noex ;


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp) ;


/* exported variables */


/* exported subroutines */

int pathclean(char *rbuf,cchar *spathbuf,int spathlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (rbuf && spathbuf) {
	    rbuf[0] = '\0' ;
	    if ((rs = maxpathlen) >= 0) {
		cint	rlen = rs ;
	        int	nc = 0 ;
	        int	pl ;
	        bool	f_prev = false ;
	        cchar	*pp = spathbuf ;
	        if ((pl = strnlen(spathbuf,spathlen)) > 0) {
	            pathbuf	pb ;
	            int		cl ;
	            cchar	*cp ;
	            if ((rs = pb.start(rbuf,rlen)) >= 0) {
	                if (*pp == '/') {
	                    pp += 1 ;
	                    pl -= 1 ;
	                    f_prev = true ;
	                    if (*pp == '/') {
	                        pb.chr('/') ;
		            }
	                } /* end if */
			if (rs >= 0) {
	                    while ((cl = nextname(pp,pl,&cp)) > 0) {
	                        if (cp[0] == '.') {
	                            if ((cp[1] == '.') && (cl == 2)) {
	                                if (nc > 0) {
	                                    pb.remslash() ;
	                                    nc -= 1 ;
	                                } else {
	                                    if (f_prev) {
	                                        pb.chr('/') ;
					    }
	                                    pb.strw(cp,2) ;
	                                    f_prev = true ;
	                                    nc = 0 ;
	                                } /* end if */
	                            } else if (cl > 1) {
	                                if (f_prev) {
	                                    pb.chr('/') ;
				        }
	                                pb.strw(cp,cl) ;
	                                f_prev = true ;
	                                nc += 1 ;
	                            } /* end if */
	                        } else {
	                            if (f_prev) {
	                                pb.chr('/') ;
			            }
	                            pb.strw(cp,cl) ;
	                            f_prev = true ;
	                            nc += 1 ;
	                        } /* end if */
	                        pl -= ((cp + cl) - pp) ;
	                        pp = (cp + cl) ;
	                    } /* end while */
			} /* end if (ok) */
	                rs1 = pb.finish() ;
	                if (rs >= 0) rs = rs1 ;
	                if (rs1 >= 0) rbuf[rs1] = '\0' ;
	            } /* end if (pathbuf) */
	        } /* end if (positive) */
	    } /* end if (maxpathlen) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (pathclean) */


/* local subroutines */

int pathbuf::start(char *pbuf,int plen) noex {
	int		rs = SR_FAULT ;
	if (pbuf) {
	    rs = SR_OK ;
	    PATHBUF_BUFP = pbuf ;
	    PATHBUF_BUFPL = plen ;
	    PATHBUF_IDX = 0 ;
	}
	return rs ;
}
/* end subroutine (pathbuf_start) */

int pathbuf::finish() noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = PATHBUF_IDX) >= 0) {
	    len = PATHBUF_IDX ;
	    PATHBUF_BUFP = nullptr ;
	    PATHBUF_BUFPL = 0 ;
	    PATHBUF_IDX = SR_NOTOPEN ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end method (pathbuf::finish) */

int pathbuf::strw(cchar *sp,int sl) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = PATHBUF_IDX) >= 0) {
	    char	*bp = (PATHBUF_BUFP + PATHBUF_IDX) ;
	    if (PATHBUF_BUFPL < 0) {
	        if (sl < 0) {
	            while (*sp) {
	                *bp++ = *sp++ ;
		    }
	        } else {
	            while (*sp && (sl > 0)) {
	                *bp++ = *sp++ ;
	                sl -= 1 ;
	            }
	        } /* end if */
	    } else {
	        if (sl < 0) {
	            while (*sp && (bp < (PATHBUF_BUFP + PATHBUF_BUFPL - 1))) {
	                *bp++ = *sp++ ;
		    }
	        } else {
	            while (*sp && (sl > 0) && 
	                (bp < (PATHBUF_BUFP + PATHBUF_BUFPL - 1))) {
	                *bp++ = *sp++ ;
	                sl -= 1 ;
	            }
	        } /* end if */
	        if (bp >= (PATHBUF_BUFP + PATHBUF_BUFPL - 1)) {
	            PATHBUF_IDX = SR_TOOBIG ;
	            rs = SR_TOOBIG ;
	        }
	    } /* end if */
	    if (rs >= 0) {
	        *bp = '\0' ;
	        len = bp - (PATHBUF_BUFP + PATHBUF_IDX) ;
	        PATHBUF_IDX = (bp - PATHBUF_BUFP) ;
	    }
	} /* end if (ok) */
	return (rs >= 0) ? len : rs ;
}
/* end method (pathbuf::strw) */

int pathbuf::chr(int ch) noex {
	char		buf[2] ;
	buf[0] = ch ;
	buf[1] = '\0' ;
	return strw(buf,1) ;
}
/* end method (pathbuf::chr) */

int pathbuf::remslash() noex {
	int		rs ;
	if ((rs = PATHBUF_IDX) >= 0) {
	    char	*bp = (PATHBUF_BUFP + PATHBUF_IDX) ;
	    if ((PATHBUF_BUFPL > 0) && (bp[-1] == '/')) {
	        PATHBUF_BUFPL -= 1 ;
	        bp -= 1 ;
	    }
	    while ((PATHBUF_BUFPL > 0) && (bp[-1] != '/')) {
	        PATHBUF_BUFPL -= 1 ;
	        bp -= 1 ;
	    } /* end while */
	    if ((PATHBUF_BUFPL > 0) && (bp[-1] == '/')) {
	        PATHBUF_BUFPL -= 1 ;
	        bp -= 1 ;
	    }
	    PATHBUF_IDX = (bp - PATHBUF_BUFP) ;
	} /* end if (ok) */
	return rs ;
}
/* end method (pathbuf::remslash) */

static int nextname(cchar *sp,int sl,cchar **rpp) noex {
	bool		f_len = (sl >= 0) ;
	while (((! f_len) || (sl > 0)) && (*sp == '/')) {
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	*rpp = sp ;
	while ((((! f_len) && *sp) || (sl > 0)) && (*sp != '/')) {
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	return (sp - (*rpp)) ;
}
/* end subroutine (nextname) */


