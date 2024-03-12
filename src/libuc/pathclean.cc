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
	<0		error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstring>
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"pathclean.h"


/* local defines */

#define	PATHBUF		struct pathbuf

#define	PATHBUF_BUF	(pbp->pbuf)
#define	PATHBUF_BUFLEN	(pbp->plen)
#define	PATHBUF_INDEX	(pbp->i)


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* local structures */

struct pathbuf {
	char		*pbuf ;
	int		plen ;
	int		i ;
} ;


/* local structures */


/* forward references */

static int	pathbuf_start(PATHBUF *,char *,int) noex ;
static int	pathbuf_char(PATHBUF *,int) noex ;
static int	pathbuf_strw(PATHBUF *,cchar *,int) noex ;
static int	pathbuf_remove(PATHBUF *) noex ;
static int	pathbuf_finish(PATHBUF *) noex ;

#ifdef	COMMENT
static int	pathbuf_getlen(PATHBUF *) noex ;
#endif

static int	nextname(cchar *,int,cchar **) noex ;


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp) ;


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
	            PATHBUF	pb ;
	            int		cl ;
	            cchar	*cp ;
	            if ((rs = pathbuf_start(&pb,rbuf,rlen)) >= 0) {
	                if (*pp == '/') {
	                    pp += 1 ;
	                    pl -= 1 ;
	                    f_prev = true ;
	                    if (*pp == '/') {
	                        pathbuf_char(&pb,'/') ;
		            }
	                } /* end if */
			if (rs >= 0) {
	                    while ((cl = nextname(pp,pl,&cp)) > 0) {
	                        if (cp[0] == '.') {
	                            if ((cp[1] == '.') && (cl == 2)) {
	                                if (nc > 0) {
	                                    pathbuf_remove(&pb) ;
	                                    nc -= 1 ;
	                                } else {
	                                    if (f_prev) {
	                                        pathbuf_char(&pb,'/') ;
					    }
	                                    pathbuf_strw(&pb,cp,2) ;
	                                    f_prev = true ;
	                                    nc = 0 ;
	                                } /* end if */
	                            } else if (cl > 1) {
	                                if (f_prev) {
	                                    pathbuf_char(&pb,'/') ;
				        }
	                                pathbuf_strw(&pb,cp,cl) ;
	                                f_prev = true ;
	                                nc += 1 ;
	                            } /* end if */
	                        } else {
	                            if (f_prev) {
	                                pathbuf_char(&pb,'/') ;
			            }
	                            pathbuf_strw(&pb,cp,cl) ;
	                            f_prev = true ;
	                            nc += 1 ;
	                        } /* end if */
	                        pl -= ((cp + cl) - pp) ;
	                        pp = (cp + cl) ;
	                    } /* end while */
			} /* end if (ok) */
	                rs1 = pathbuf_finish(&pb) ;
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

static int pathbuf_start(PATHBUF *pbp,char *pbuf,int plen) noex {
	int		rs = SR_FAULT ;
	if (pbp && pbuf) {
	    rs = SR_OK ;
	    PATHBUF_BUF = pbuf ;
	    PATHBUF_BUFLEN = plen ;
	    PATHBUF_INDEX = 0 ;
	}
	return rs ;
}
/* end subroutine (pathbuf_start) */

static int pathbuf_finish(PATHBUF *pbp) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = PATHBUF_INDEX) >= 0) {
	    len = PATHBUF_INDEX ;
	    PATHBUF_BUF = nullptr ;
	    PATHBUF_BUFLEN = 0 ;
	    PATHBUF_INDEX = SR_NOTOPEN ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (pathbuf_finish) */

static int pathbuf_strw(PATHBUF *pbp,cchar *sp,int sl) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = PATHBUF_INDEX) >= 0) {
	    char	*bp = (PATHBUF_BUF + PATHBUF_INDEX) ;
	    if (PATHBUF_BUFLEN < 0) {
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
	            while (*sp && (bp < (PATHBUF_BUF + PATHBUF_BUFLEN - 1))) {
	                *bp++ = *sp++ ;
		    }
	        } else {
	            while (*sp && (sl > 0) && 
	                (bp < (PATHBUF_BUF + PATHBUF_BUFLEN - 1))) {
	                *bp++ = *sp++ ;
	                sl -= 1 ;
	            }
	        } /* end if */
	        if (bp >= (PATHBUF_BUF + PATHBUF_BUFLEN - 1)) {
	            PATHBUF_INDEX = SR_TOOBIG ;
	            rs = SR_TOOBIG ;
	        }
	    } /* end if */
	    if (rs >= 0) {
	        *bp = '\0' ;
	        len = bp - (PATHBUF_BUF + PATHBUF_INDEX) ;
	        PATHBUF_INDEX = (bp - PATHBUF_BUF) ;
	    }
	} /* end if (ok) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (pathbuf_strw) */

static int pathbuf_char(PATHBUF *pbp,int ch) noex {
	char		buf[2] ;
	buf[0] = ch ;
	buf[1] = '\0' ;
	return pathbuf_strw(pbp,buf,1) ;
}
/* end subroutine (pathbuf_char) */

static int pathbuf_remove(PATHBUF *pbp) noex {
	int		rs ;
	if ((rs = PATHBUF_INDEX) >= 0) {
	    char	*bp = (PATHBUF_BUF + PATHBUF_INDEX) ;
	    if ((PATHBUF_BUFLEN > 0) && (bp[-1] == '/')) {
	        PATHBUF_BUFLEN -= 1 ;
	        bp -= 1 ;
	    }
	    while ((PATHBUF_BUFLEN > 0) && (bp[-1] != '/')) {
	        PATHBUF_BUFLEN -= 1 ;
	        bp -= 1 ;
	    } /* end while */
	    if ((PATHBUF_BUFLEN > 0) && (bp[-1] == '/')) {
	        PATHBUF_BUFLEN -= 1 ;
	        bp -= 1 ;
	    }
	    PATHBUF_INDEX = (bp - PATHBUF_BUF) ;
	} /* end if (ok) */
	return rs ;
}
/* end subroutine (pathbuf_remove) */

#ifdef	COMMENT /* not needed */
static int pathbuf_getlen(PATHBUF *pbp) noex {
	return PATHBUF_INDEX ;
}
/* end subroutine (pathbuf_getlen) */
#endif /* COMMENT */

static int nextname(cchar *sp,int sl,cchar **rpp) noex {
	int		f_len = (sl >= 0) ;
	while (((! f_len) || (sl > 0)) && (*sp == '/')) {
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	*rpp = sp ;
/* skip the non-slash characters */
	while ((((! f_len) && *sp) || (sl > 0)) && (*sp != '/')) {
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	return (sp - (*rpp)) ;
}
/* end subroutine (nextname) */


