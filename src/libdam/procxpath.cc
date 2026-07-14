/* procxpath SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* process a 'xpath' file */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debug print-outs */
#define	CF_PATHCLEAN	1		/* clean up the path */
#define	CF_STAT		0		/* directory needs to be there */

/* revision history:

	= 1998-09-10, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	This subroutine will read (process) a file that has directory
	paths in it.  The directory paths are read in and added
	(one by one) to the specified list.

	Synopsis:
	int procxpath(vecstr *lp,cchar *fname) noex

	Arguments:
	lp		pointer to VECSTR list
	fname		filename to process

	Returns:
	>=0		count of paths read in
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<cassert>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<bufsizevar.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<pathclean.h>		/* LIBUC */
#include	<fieldterminit.hh>	/* LIBUC */

#include	<field.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */

#include	<char.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<bfile.h>		/* LIBB */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"procxpath.h"

/* local defines */

#define	PATHMULT	4		/* four (4) times path-len */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* compile-time debug print-outs */P
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* externals variables */


/* forward references */


/* local structures */

namespace {
    struct pather {
	vecstr *lp ;
	char	*pbuf ;
	int	plen ;
	pather(vecstr *p) noex : lp(p) {
	    pbuf = nullptr ;
	    lbuf = nullptr ;
	} ; /* end ctor */
	int operator () (cchar *) noex ;
	int procfile	(cchar *) noex ;
	int procln	(cchar *,int) noex ;
    } ; /* end struct */
} /* end namespace */


/* local variables */

constexpr fieldterminit		ft("#:;") ;
static bufsizevar		maxpathlen(bufsize_mp) ;
cbool				f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int procxpath(vecstr *lp,cchar *fname) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	assert(lp && fname) ;
	if (lp && fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        pather po(lp) ;
	        rs = po(fname) ;
	        c = rs ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (procxpath) */


/* local subroutines */

int pather::operator(cchar *fn) noex 
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	if ((rs = maxpathlen) >= 0) {
	    cint sz = (PATHMULT * rs) ;
	    if ((rs = mem.mall(sz,&pbuf)) >= 0) {
		plen = rs ;
		{
		    rs = file(fn) ;
		    c = rs ;
		}
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (maxpathlen) */
	return (rs >= 0) ? c : rs ;
} /* end method (pather::operator) */

int pather::procfile(cchar *fn) noex {
    	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	if (char *lbuf ; (rs = mem.ml(&lbuf)) >= 0) {
	    cint llen = rs ;
	    if (bfile pf ; (rs = pf.open(fname,"r")) >= 0) {
	        while ((rs = pf.readlns(lbuf,llen)) > 0) {
		    cchar *cp ;
		    if (int cl = (cl = sfcontent(lbuf,rs,&cp)) > 0) {
		        rs = procln(cp,cl) ;
		        c += rs ;
		    }
	        } /* end while */
	        rs1 = pf.close ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (bfile) */
	    rs1 = mem.free(lbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
} /* end method (pather::procfile) */

int pather::procln(cchar *lp,int ll) noex {
    	int		rs ;
	int		rs1 ;
	if (field fsb ; (rs = fsb.start(cp,cl)) >= 0) {
	    cchar	*fp ;
	    for (int fl ; (fl = fsb.get(ft.terms,&fp)) >= 0 ; ) {
		if (fl > 0) {
		    rs = procent(fp,fl) ;
		}
	        if (fsb.term == '#') break ;
	    } /* end for */
	    rs1 = fsb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (field) */
	return rs ;
} /* end method (pather::procln) */

int pather::procent(cchar *,int) noex {
    	int		rs = SR_OK ;
	if ((rs = pathclean(pbuf,fp,fl)) > 0) {
	    if (ustat sb ; (rs = u_stat(pbuf,&sb) ;

	                    f_add = ((rs >= 0) && S_ISDIR(sb.st_mode)) ;
#else
	                    f_add = (pl > 0) ;
#endif /* CF_STAT */

	                } else {
	                    f_add = ((pl == 0) && (fsb.term == ':')) ;
	                }

	                if (f_add && (vecstr_findn(lp,pp,pl) == SR_NOTFOUND)) {

#if	CF_DEBUG
	                    debugprintf("procxpath: add=%r\n",pp,pl) ;
#endif

#if	CF_STAT
	                    rs = u_stat(pp,&sb) ;
	                    f_add = ((rs >= 0) && S_ISDIR(sb.st_mode)) ;
#endif

	                    if (f_add) {
	                        c += 1 ;
	                        rs = vecstr_add(lp,pp,pl) ;
	                        if (rs < 0) break ;
	                    }

	                } /* end if (needed to add) */

	            } /* end while (reading fields) */

	            field_finish(&fsb) ;
	        } /* end if (field) */

#endif /* COMMENT */


