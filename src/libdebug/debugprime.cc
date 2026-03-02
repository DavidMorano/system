/* debugprime SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* debugging stubs */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-15, David A­D­ Morano
	This was written to debug the REXEC program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	debugprime

	Description:
	This modeule provides debugging support.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdarg>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<usupport.h>		/* |strwcpy(3u)| */
#include	<uclibmem.h>
#include	<snflagsx.h>
#include	<localmisc.h>		/* |COLUMNS| */

#include	"debugutil.hh"
#include	"libdebug.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	PRINTBUFLEN	(COLUMNS + 2)

#ifndef	HEXBUFLEN
#define	HEXBUFLEN	100
#endif


/* imported namespaces */


/* local typedefs */

using libu::strwcpy ;			/* subroutine */
using libdebug::checkbasebounds ;	/* subroutine */


/* external subroutines */


/* external variables */


/* local structures */

struct debug_oflags {
	int		m ;
	char		*s ;
} ;


/* forward subroutines */


/* local variables */


/* exported variables */


/* exported subroutines */

#if	CF_DEBUGS
int debuginit() noex {
	int		rs = SR_OK ;
	return rs ;
}
/* end subroutine (debuginit) */
#endif /* CF_DEBUGS */

char *d_reventstr(int revents,char *bp,int bl) noex {
	snflagspoll(bp,bl,revents) ;
	return bp ;
}
/* end subroutine (d_reventstr) */

/* who is open? */
void d_whoopen(int *s) noex {
	int		rs ;
	int		i ;
	if (s != nullptr) {
	    debugprintf("d_whoopen: %s\n",s) ;
	}
	for (i = 0 ; i < 20 ; i += 1) {
	    if ((rs = u_fcntl(i,F_GETFL,0)) >= 0) {
	        debugprintf("d_whoopen: open on %d accmod=%08x\n",
	            i,(rs & O_ACCMODE)) ;
	    }
	} /* end for */
}
/* end subroutine (d_whoopen) */

/* return a count of the number of open files */
int d_openfiles() noex {
	USTAT		sb ;
	int		count = 0 ;
	for (int i = 0 ; i < 2048 ; i += 1) {
	    if (u_fstat(i,&sb) >= 0) {
	        count += 1 ;
	    }
	} /* end for */
	return count ;
}
/* end subroutine (d_openfiles) */

int d_ispath(cchar *p) noex {
	if (p == nullptr) return FALSE ;

#ifdef	DEBFILE
	nprintf(DEBFILE,"d_ispath: PATH=>%W<\n",
	    p,strnlen(p,30)) ;
#endif

	return ((*p == '/') || (*p == ':')) ;
}
/* end subroutine (d_ispath) */

int gdb() noex {
	return 0 ;
}
/* end subroutine (gdb) */

int debugprinthex(cchar *ids,int maxcols,cchar *sp,int sl) noex {
	cint		plen = PRINTBUFLEN ;
	int		rs ;
	int		idlen = 0 ;
	int		wlen = 0 ;
	char		pbuf[PRINTBUFLEN + 1] ;

	if (ids != nullptr) idlen = lenstr(ids) ;

	if (maxcols < 0) maxcols = COLUMNS ;

	if (idlen > 0) maxcols -= (idlen + 1) ;

	if ((rs = mkhexnstr(pbuf,plen,maxcols,sp,sl)) >= 0) {
	    if (idlen > 0) {
	        rs = debugprintf("%r %s\n",ids,idlen,pbuf) ;
	    } else {
	        rs = debugprintf("%s\n",pbuf) ;
	    }
	    wlen = rs ;
	}

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (debugprinthex) */

int debugprinthexblock(cchar *ids,int maxcols,cvoid *vp,int vl) noex {
	int		rs = SR_OK ;
	int		idlen = 0 ;
	int		sl = vl ;
	int		wlen = 0 ;
	cchar		*sp = (cchar *) vp ;
	char		printbuf[PRINTBUFLEN + 1] ;

	if (ids != nullptr) idlen = lenstr(ids) ;

	if (maxcols < 0) maxcols = COLUMNS ;

	if (sl < 0) sl = lenstr(sp) ;

	while ((rs >= 0) && (sl > 0)) {
	    char	*pbp = printbuf ;
	    int		pbl = PRINTBUFLEN ;
	    int		cols = maxcols ;

	    if (ids != nullptr) {
	        if ((idlen+2) < pbl) {
		    int	i = intconv(strwcpy(pbp,ids,idlen) - pbp) ;
	            pbp[i++] = ':' ;
	            pbp[i++] = ' ' ;
	            pbp += i ;
	            pbl -= i ;
	            cols -= i ;
	        } else {
	            rs = SR_OVERFLOW ;
		}
	    }

	    if (rs >= 0) {
	        cint	n = (cols / 3) ;
		int		cslen ;
	        cslen = MIN(n,sl) ;
	        if ((rs = mkhexstr(pbp,pbl,sp,cslen)) >= 0) {
	            sp += cslen ;
	            sl -= cslen ;
	            rs = debugprint(printbuf,-1) ;
	            wlen += rs ;
		}
	    } /* end if (ok) */

	} /* end while */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (debugprinthexblock) */

/* audit a HOSTENT structure */
int heaudit(HOSTENT *hep,cchar *buf,int buflen) noex {
	int		rs = SR_OK ;
	int		i ;
	char		**cpp ;

	if (hep == nullptr) return SR_FAULT ;
	if (buf == nullptr) return SR_FAULT ;

	if (buflen < 0) return SR_INVALID ;

	if (rs >= 0) {
	    rs = checkbasebounds(buf,buflen,hep->h_name) ;
	}

	if (rs >= 0) {
	    cpp = hep->h_aliases ;
	    if (cpp != nullptr) {
	        if ((rs = checkbasebounds(buf,buflen,cpp)) >= 0) {
	            for (i = 0 ; cpp[i] != nullptr ; i += 1) {
	                rs = checkbasebounds(buf,buflen,(cpp + i)) ;
	                if (rs >= 0) {
	                    rs = checkbasebounds(buf,buflen,cpp[i]) ;
			}
	                if (rs < 0) break ;
	            } /* end for */
	        } /* end if (checkbasebounds) */
	    }
	} /* end if (ok) */

	if (rs >= 0) {
	    cpp = hep->h_aliases ;
	    if (cpp != nullptr) {
	        if ((rs = checkbasebounds(buf,buflen,cpp)) >= 0) {
	            for (i = 0 ; cpp[i] != nullptr ; i += 1) {
	                rs = checkbasebounds(buf,buflen,(cpp + i)) ;
	                if (rs >= 0) {
	                    rs = checkbasebounds(buf,buflen,cpp[i]) ;
			}
	                if (rs < 0) break ;
	            } /* end for */
	        } /* end if (checkbasebounds) */
	    }
	} /* end if (ok) */

	return rs ;
}
/* end subroutine (heaudit) */

char *stroflags(char *buf,int oflags) noex {
	int		rs = snflagsopen(buf,TIMEBUFLEN,oflags) ;
	return (rs >= 0) ? buf : nullptr ;
}
/* end subroutine (stroflags) */


/* local subroutines */

int debugprintfsize(cchar *id,int fd) noex {
	int		rs ;
	if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) {
	    ulong	fs = sb.st_size ;
#if	defined(_I32LPx)
	    debugprintf("debugprintfsize: I32LPx\n") ;
#endif
	    debugprintf("debugprintfsize: %s size=%lu\n",id,fs) ;
	}
	return rs ;
}
/* end subroutine (debugprintstat) */


