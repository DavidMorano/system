/* setstrx_loadfile SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* load strings from a file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	setstrx_loadfile

	Description:
	This subroutine will read (process) a file and put all of
	the strings found into the string (supplied) list, consisting
	of a SETSTRX object.

	Synopsis:
	int setstrx_loadfile(setstrx *op,int fo,cchar *fname) noex

	Arguments:
	op		pointer to SETSTRX object
	fo		0=ignore, 1=replace
	fname		file to load

	Returns:
	>=0		number of elements loaded
	<0		error (system-return)

	Notes:

	Why use FILER over BFILE?  Yes, FILER is a tiny bit more
	lightweight than BFILE -- on a good day.  But the real reason
	may be so that we do not need to load BFILE in code that
	resides very deep in a software stack if we do not need it
	-- like deep inside loadable modules.  Anyway, just a thought!

	Why are we using FIELD as opposed to |sfnext(3uc)| or
	something similar?  Because our sematics are to process
	quoted strings as a single setstrx entry!

	Note_on_uniqueness:

	The 'fo' argument:
		0=ignore
		1=replace

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>		/* |O_{xx}| */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strcmp(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<getfdfile.h>
#include	<libmallocxx.h>
#include	<intsat.h>
#include	<filer.h>
#include	<field.h>
#include	<fieldterminit.hh>
#include	<sfx.h>			/* |sfcontent(3uc)| */
#include	<localmisc.h>		/* |BCEIL(3dam)| */

#include	"setstrx.hh"


/* local defines */

#define	TO_READ		-1		/* read timeout */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct suber {
	setstrx		*op ;
	int		fu ;
	int		fd ;
	bool		fopened = false ;
	suber(setstrx *p,int u) noex : op(p), fu(u) { 
	    fd = FD_STDIN ;
	} ;
	int operator () (cchar *) noex ;
	int fileopen(cchar *) noex ;
	int fileclose() noex ;
	int loadfd() noex ;
	int loadfds(int,int,int) noex ;
	int loadln(cchar *,int) noex ;
    } ; /* end struct (suber) */
} /* end namespace */


/* forward references */


/* local variables */

static constexpr fieldterminit		ft("\n#") ;


/* exported variables */


/* exported subroutines */

int setstrx::loadfile(int fu,cchar *fname) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
		suber sub(this,fu) ;
		rs = sub(fname) ;
		c = rs ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
} /* end method (setstrx::loadfile) */


/* local subroutines */

int suber::operator () (cchar *fname) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = fileopen(fname)) >= 0) ylikely {
	    {
	  	rs = loadfd() ;
		c = rs ;
	    }
	    rs1 = fileclose() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (file) */
	return (rs >= 0) ? c : rs ;
} /* end method (suber::operator) */

int suber::fileopen(cchar *fname) noex {
	int		rs = SR_OK ;
	if (strcmp(fname,"-") != 0) {
	    cint	of = O_RDONLY ;
	    cmode	om = 0666 ;
	    if ((rs = uc_open(fname,of,om)) >= 0) {
	        fd = rs ;
	        fopened = true ;
	    }
	} /* end if (actually needed an open) */
	return rs ;
} /* end method (suber::fileopen) */

int suber::fileclose() noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	if (fopened && (fd >= 0)) {
	    rs1 = u_close(fd) ;
	    if (rs >= 0) rs = rs1 ;
	    fd = -1 ;
	    fopened = false ;
	} /* end if (was open) */
	return rs ;
} /* end method (suber::fileclose) */

int suber::loadfd() noex {
	int		rs ;
	int		c = 0 ;
	if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) {
	    if (! S_ISDIR(sb.st_mode)) {
		int	to = -1 ;
	        int	cs ;
		int	fbsz = 1024 ;
		int	fbo = 0 ;
	        if (S_ISREG(sb.st_mode)) {
	            int	fs = ((sb.st_size == 0) ? 1 : intsat(sb.st_size)) ;
	            cs = BCEIL(fs,512) ;
	            fbsz = min(cs,1024) ;
	        } else {
	            to = TO_READ ;
	            if (S_ISSOCK(sb.st_mode)) {
			fbo |= FILER_ONET ;
		    }
	        } /* end if */
		{
		    rs = loadfds(fbsz,fbo,to) ;
		    c = rs ;
		} /* end block */
	    } else {
	        rs = SR_ISDIR ;
	    }
	} /* end if (stat) */
	return (rs >= 0) ? c : rs ;
}
/* end method (suber::loadfd) */

int suber::loadfds(int fbsz,int fbo,int to) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (char *lbuf ; (rs = libmalloc_ml(&lbuf)) >= 0) {
	    cint	llen = rs ;
	    if (filer fb ; (rs = fb.start(fd,0z,fbsz,fbo)) >= 0) {
	        while ((rs = fb.readln(lbuf,llen,to)) > 0) {
		    cchar	*cp{} ;
		    if (int cl ; (cl = sfcontent(lbuf,rs,&cp)) > 0) {
			rs = loadln(cp,cl) ;
			c += rs ;
		    } /* end if (sfcontent) */
	            if (rs < 0) break ;
	        } /* end while (reading lines) */
	        rs1 = fb.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (filer) */
	    rs = rslibfree(rs,lbuf) ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
} /* end method (suber::loadfds) */

int suber::loadln(cchar *lp,int ll) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (field fsb ; (rs = fsb.start(lp,ll)) >= 0) {
	    cchar	*fp ;
	    for (int fl ; (fl = fsb.get(ft.terms,&fp)) >= 0 ; ) {
		if (fl > 0) {
		    if (fu) {
			rs = op->del(fp,fl) ;
		    }
		    if (rs >= 0) {
			rs = op->add(fp,fl) ;
		    }
		    if (rs != INT_MAX) c += 1 ;
		} /* end if (got one) */
		if (fsb.term == '#') break ;
		if (rs < 0) break ;
	    } /* end while (fields) */
	    rs1 = fsb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (fields) */
	return (rs >= 0) ? c : rs ;
} /* end method (suber::loadln) */


