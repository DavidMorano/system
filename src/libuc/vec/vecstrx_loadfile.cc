/* vecstrx_loadfile SUPPORT */
/* charset=ISO8859-1 */
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
	vecstrx_loadfile

	Description:
	This subroutine will read (process) a file and put all of
	the strings found into the string (supplied) list, consisting
	of a vecstrx object.

	Synopsis:
	int vecstrx_loadfile(vecstrx *vsp,int fu,cchar *fname) noex

	Arguments:
	vsp		pointer to vecstrx object
	fu		flag specifying uniqueness
	fname		file to load

	Returns:
	>=0		number of elements loaded
	<0		error (system-return)

	Notes:
	Why use filer over BFILE? Yes, filer is a tiny bit more
	lightweight than BFILE -- on a good day. But the real reason
	may be so that we do not need to load BFILE in code that
	resides very deep in a software stack if we do not need it
	-- like deep inside loadable modules. Anyway, just a thought!

	Why are we using field as opposed to |sffield(3uc)| or
	something similar?  Because our sematics are to process
	quoted strings as a single vecstrx entry!

	Note_on_uniqueness:

	The 'fu' argument:
		0=no_unique
		1=unique

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>		/* |uc_fstat(3uc)| */
#include	<unistd.h>
#include	<fcntl.h>		/* |uc_open(3uc)| */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strcmp(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<getfdfile.h>
#include	<bufsizevar.hh>
#include	<filer.h>
#include	<field.h>
#include	<fieldterms.h>
#include	<localmisc.h>		/* |BCEIL| */

#include	"vecstrx.hh"


/* local defines */

#define	LINEBUFMULT	5		/* line-buffer size multiplier */
#define	DEFBUFLEN	1024

#define	TO_READ		-1		/* read timeout */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
        int	linebuflen ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */

static int	vecstrx_loadfiler(vecstrx *,int,cchar *) noex ;
static int	vecstrx_loadfd(vecstrx *,int,int) noex ;
static int	vecstrx_loadline(vecstrx *,int,cchar *,int) noex ;
static int	mkterms() noex ;


/* local variables */

static char		fterms[fieldterms_termsize] ;

static vars		var ;


/* exported variables */


/* exported subroutines */

int vecstrx::loadfile(int fu,cchar *fname) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        static cint	rst = mkterms() ;
	        if ((rs = rst) >= 0) {
	            static cint		rsv = var ;
	            if ((rs = rsv) >= 0) {
			rs = vecstrx_loadfiler(this,fu,fname) ;
			c = rs ;
		    } /* end if (mkvars) */
		} /* end if (mkterms) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstrx_loadfile) */


/* local subroutines */

static int vecstrx_loadfiler(vecstrx *op,int fu,cchar *fname) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
        if (strcmp(fname,"-") != 0) {
            cint    of = O_RDONLY ;
            cmode   om = 0666 ;
            if ((rs = uc_open(fname,of,om)) >= 0) {
		cint	fd = rs ;
		{
                    rs = vecstrx_loadfd(op,fu,fd) ;
		    c = rs ;
		}
                rs1 = uc_close(fd) ;
		if (rs >= 0) rs = rs1 ;
            } /* end if (read-file) */
        } else {
	    cint	fd = FD_STDIN ;
            rs = vecstrx_loadfd(op,fu,fd) ;
            c = rs ;
        } /* end if (file or STDIN) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstrx_loadfiler) */

static int vecstrx_loadfd(vecstrx *vsp,int fu,int fd) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (ustat sb ; (rs = uc_fstat(fd,&sb)) >= 0) {
	    csize	fsz = size_t(sb.st_size) ;
	    rs = SR_ISDIR ;
	    if (! S_ISDIR(sb.st_mode)) {
		cint	llen = var.linebuflen ;
	        cint	fs = ((fsz == 0) ? 1 : int(fsz & INT_MAX)) ;
		int	to = -1 ;
	        int	fbsz = DEFBUFLEN ;
	        int	fbo = 0 ;
	        if (S_ISREG(sb.st_mode)) {
	            cint	cs = BCEIL(fs,512) ;
	            fbsz = min(cs,1024) ;
	        } else {
	            to = TO_READ ;
	            if (S_ISSOCK(sb.st_mode)) fbo |= FILER_ONET ;
	        }
	        if (char *lbuf ; (rs = uc_libmalloc((llen+1),&lbuf)) >= 0) {
		    if (filer lf ; (rs = lf.start(fd,0z,fbsz,fbo)) >= 0) {
	                while ((rs = lf.readln(lbuf,llen,to)) > 0) {
			    rs = vecstrx_loadline(vsp,fu,lbuf,rs) ;
			    c += rs ;
	                    if (rs < 0) break ;
	                } /* end while (reading lines) */
	                rs1 = lf.finish ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (filer) */
		    rs1 = uc_libfree(lbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (not a directory) */
	} /* end if (stat) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstrx_loadfd) */

static int vecstrx_loadline(vecstrx *vsp,int fu,cchar *lbuf,int len) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (field fsb ; (rs = fsb.start(lbuf,len)) >= 0) {
	    cchar	*fp ;
	    for (int fl ; (fl = fsb.get(fterms,&fp)) >= 0 ; ) {
		if (fl > 0) {
		    if (fu) {
			rs = vsp->adduniq(fp,fl) ;
			if (rs < INT_MAX) c += 1 ;
		    } else {
			c += 1 ;
			rs = vsp->add(fp,fl) ;
		    }
		} /* end if (got one) */
		if (fsb.term == '#') break ;
		if (rs < 0) break ;
	    } /* end for (fields) */
	    rs1 = fsb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (fields) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstrx_loadline) */

static int mkterms() noex {
	return fieldterms(fterms,false,'\n','#') ;
}
/* end subroutine (mkterms) */

vars::operator int () noex {
        int             rs ;
        if ((rs = ucmaxline) >= 0) {
            var.linebuflen = (rs * LINEBUFMULT) ;
        }
        return rs ;
}       
/* end subroutine (vars::operator) */


