/* setostr_loadfile SUPPORT */
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
	setostr_loadfile

	Description:
	This subroutine will read (process) a file and put all of
	the strings found into the string (supplied) list, consisting
	of a SETSTR object.

	Synopsis:
	int setostr_loadfile(setostr *op,int fo,cchar *fname) noex

	Arguments:
	op		pointer to SETSTR object
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
	quoted strings as a single setostr entry!

	Note_on_uniqueness:

	The 'fo' argument:
		0=ignore
		1=replace

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
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
#include	<sfx.h>
#include	<localmisc.h>		/* |BCEIL(3dam)| */

#include	"setostr.h"


/* local defines */

#define	TO_READ		-1		/* read timeout */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	setostr_loadfd(setostr *,int,int) noex ;
static int	setostr_loadfds(setostr *,int,int,int,int,int) noex ;
static int	setostr_loadln(setostr *,int,cchar *,int) noex ;


/* local variables */

static constexpr fieldterminit		ft("\n#") ;


/* exported variables */


/* exported subroutines */

int setostr_loadfile(setostr *op,int fu,cchar *fname) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op && fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        int	fd = FD_STDIN ;
	        bool	f_opened = false ;
		rs = SR_OK ;
	        if (strcmp(fname,"-") != 0) {
		    cint	of = O_RDONLY ;
		    cmode	om = 0666 ;
	            if ((rs = uc_open(fname,of,om)) >= 0) {
	                fd = rs ;
	                f_opened = true ;
	            }
	        }
	        if (rs >= 0) {
	            rs = setostr_loadfd(op,fu,fd) ;
	            c = rs ;
	        }
	        if (f_opened && (fd >= 0)) {
	            u_close(fd) ;
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (setostr_loadfile) */


/* local subroutines */

static int setostr_loadfd(setostr *op,int fu,int fd) noex {
	int		rs ;
	int		to = -1 ;
	int		c = 0 ;
	if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) {
	    if (! S_ISDIR(sb.st_mode)) {
		int	fbsz = 1024 ;
		int	fbo = 0 ;
	        int	cs ;
	        if (S_ISREG(sb.st_mode)) {
	            int	fs = ((sb.st_size == 0) ? 1 : intsat(sb.st_size)) ;
	            cs = BCEIL(fs,512) ;
	            fbsz = min(cs,1024) ;
	        } else {
	            to = TO_READ ;
	            if (S_ISSOCK(sb.st_mode)) {
			fbo |= FILER_ONET ;
		    }
	        }
		{
		    rs = setostr_loadfds(op,fd,fu,fbsz,fbo,to) ;
		    c = rs ;
		}
	    } else {
	        rs = SR_ISDIR ;
	    }
	} /* end if (stat) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (setostr_loadfd) */

static int setostr_loadfds(setostr *op,int fd,int fu,int fbsz,int fbo,
		int to) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (char *lbuf ; (rs = libmalloc_ml(&lbuf)) >= 0) {
	    cint	llen = rs ;
	    if (filer fb ; (rs = fb.start(fd,0z,fbsz,fbo)) >= 0) {
	        while ((rs = fb.readln(lbuf,llen,to)) > 0) {
		    cchar	*cp{} ;
		    if (int cl ; (cl = sfcontent(lbuf,rs,&cp)) > 0) {
			rs = setostr_loadln(op,fu,cp,cl) ;
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
} /* end subroutine (setostr_loadfds) */

static int setostr_loadln(setostr *op,int fu,cchar *lp,int ll) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (field fsb ; (rs = fsb.start(lp,ll)) >= 0) {
	    cchar	*fp ;
	    for (int fl ; (fl = fsb.get(ft.terms,&fp)) >= 0 ; ) {
		if (fl > 0) {
		    if (fu) {
			rs = setostr_del(op,fp,fl) ;
		    }
		    if (rs >= 0) {
			rs = setostr_add(op,fp,fl) ;
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
}
/* end subroutine (setostr_loadln) */


