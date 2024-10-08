/* osetstr_loadfile SUPPORT */
/* lang=C++20 */

/* load strings from a file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	osetstr_loadfile

	Description:
	This subroutine will read (process) a file and put all of
	the strings found into the string (supplied) list, consisting
	of a osetstr object.

	Synopsis:
	int osetstr_loadfile(osetstr *vsp,int fo,cchar *fname) noex

	Arguments:
	vsp		pointer to osetstr object
	fo		0=ignore, 1=replace
	fname		file to load

	Returns:
	>=0		number of elements loaded
	<0		error (system-return)

	Notes:

	Why use FILER over BFILE?  Yes, FILER is a tiny bit more
	lightweight than BFILE -- on a good day. But the real reason
	may be so that we don't need to load BFILE in code that
	resides very deep in a software stack if we don't need it
	-- like deep inside loadable modules.  Anyway, just a thought!

	Why are we using FIELD as opposed to |sfnext(3uc)| or
	something similar?  Because our sematics are to process
	quoted strings as a single osetstr entry!

	Note_on_uniqueness:

	The 'fo' argument:
		0=ignore
		1=replace

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<limits.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strcmp(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<linebuffer.h>
#include	<intsat.h>
#include	<filer.h>
#include	<field.h>
#include	<fieldterminit.hh>
#include	<sfx.h>
#include	<localmisc.h>		/* |BCEIL(3dam)| */

#include	"osetstr.h"


/* local defines */

#ifndef	FD_STDIN
#define	FD_STDIN	0
#endif

#define	TO_READ		-1		/* read timeout */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */

static int	osetstr_loadfd(osetstr *,int,int) noex ;
static int	osetstr_loadline(osetstr *,int,cchar *,int) noex ;


/* local structures */


/* local variables */

constexpr fieldterminit		ft("\n#") ;


/* exported variables */


/* exported subroutines */

int osetstr_loadfile(osetstr *vsp,int fu,cchar *fname) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = osetstr_magic(vsp,fname)) >= 0) {
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
	            rs = osetstr_loadfd(vsp,fu,fd) ;
	            c = rs ;
	        }
	        if (f_opened && (fd >= 0)) {
	            u_close(fd) ;
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (osetstr_loadfile) */


/* local subroutines */

static int osetstr_loadfd(osetstr *vsp,int fu,int fd) noex {
	USTAT		sb ;
	int		rs ;
	int		rs1 ;
	int		to = -1 ;
	int		c = 0 ;
	if ((rs = u_fstat(fd,&sb)) >= 0) {
	    if (! S_ISDIR(sb.st_mode)) {
		linebuffer	lb ;
		int		fbsz = 1024 ;
		int		fbo = 0 ;
	        if (S_ISREG(sb.st_mode)) {
	            int	fs = ((sb.st_size == 0) ? 1 : intsat(sb.st_size)) ;
	            int	cs ;
	            cs = BCEIL(fs,512) ;
	            fbsz = min(cs,1024) ;
	        } else {
	            to = TO_READ ;
	            if (S_ISSOCK(sb.st_mode)) fbo |= FILER_ONET ;
	        }
		if ((rs = lb.start) >= 0) {
		    filer	loadfile, *lfp = &loadfile ;
		    cint	llen = rs ;
		    char	*lbuf = lb.lbuf ;
	            if ((rs = filer_start(lfp,fd,0z,fbsz,fbo)) >= 0) {
	                while ((rs = filer_readln(lfp,lbuf,llen,to)) > 0) {
			    cchar	*cp ;
			    if (int cl ; (cl = sfcontent(lbuf,rs,&cp)) > 0) {
			        rs = osetstr_loadline(vsp,fu,cp,cl) ;
			        c += rs ;
			    }
	                    if (rs < 0) break ;
	                } /* end while (reading lines) */
	                rs1 = filer_finish(lfp) ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (filer) */
		    rs1 = lb.finish ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (linebuffer) */
	    } else {
	        rs = SR_ISDIR ;
	    }
	} /* end if (stat) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (osetstr_loadfd) */

static int osetstr_loadline(osetstr *vsp,int fu,cchar *lbuf,int len) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (field fsb ; (rs = fsb.start(lbuf,len)) >= 0) {
	    int		fl ;
	    cchar	*fp ;
	    while ((fl = fsb.get(ft.terms,&fp)) >= 0) {
		if (fl > 0) {
		    if (fu) {
			rs = osetstr_del(vsp,fp,fl) ;
		    }
		    if (rs >= 0) {
			rs = osetstr_add(vsp,fp,fl) ;
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
/* end subroutine (osetstr_loadline) */


