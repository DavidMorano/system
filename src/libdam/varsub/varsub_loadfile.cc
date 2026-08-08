/* varsub_loadfile SUPPORT */
/* varsub_addvec SUPPORT */
/* lang=C++20 */

/* process a file with variable substitutions */
/* version %I% last-modified %G% */


/* revision history:

	= 1994-09-10, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	varsub_loadfile

	Description:
	This subroutine will read a file of variable substitutions
	and put them into the passed varsub object.

	Synopsis:
	int varsub_loadfile(varsub *op,cchar *fn) noex

	Arguments:
	op		pointer to varsub object to accumulate results
	fn		file to process

	Returns:
	>=0		count of environment variables loaded
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>		/* POSIX */
#include	<climits>		/* CSTD |UCHAR_MAX| + |CHAR_BIT| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstrings>		/* CSTD |strncasecmp(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<stdfnames.h>		/* LIBU |STDFNIN| */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucstream.hh>		/* LIBUC */
#include	<field.h>		/* LIBUC */
#include	<fieldterms.h>		/* LIBUC */
#include	<rmx.h>			/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"varsub.h"
#include	"varsub_util.hh"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/ucstream.ccm"

import libutil ;			/* |lenstr(3u)| */
import ucstream ;
import varsub_util ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct sub_loadfile {
	varsub	*op ;
	char	*lbuf ;
	char	*abuf ;
	int	llen ;
	int	alen ;
	sub_loadfile(varsub *p,char *lb,int ll,char *ab,int al) noex {
	    op = p ;
	    lbuf = lb ;
	    abuf = ab ;
	    llen = ll ;
	    alen = al ;
	} ; /* end ctor */
	int operator () (cchar *) noex ;
	int procln	(cchar *,int) noex ;
    } ; /* end struct (sub_loadfile) */
    struct vars {
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */

local bool	hasexport(cchar *,int) noex ;


/* local variables */

static char	fterms[fieldterms_termsize] ;
static vars	var ;


/* exported variables */


/* exported subroutines */

int varsub_loadfile(varsub *op,cchar *fn) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	if ((fn == nullptr) || (fn[0] == '\0') || (fn[0] == '-')) {
	    fn = STDFNIN ; /* standard-input */
	}
	if ((rs = varsub_magic(op)) >= 0) ylikely {
	     if (static cint rsv = var ; (rs = rsv) >= 0) ylikely {
	        if (char *lbuf ; (rs = lm_mp(&lbuf)) >= 0) ylikely {
	            cint	llen = rs ;
	            if (char *abuf ; (rs = lm_mp(&abuf)) >= 0) ylikely {
		        sub_loadfile	lo(op,lbuf,llen,abuf,rs) ;
		        {
		            rs = lo(fn) ;
		            c = rs ;
		        }
		        rs1 = lm_free(abuf) ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (m-a-f) */
	            rs1 = lm_free(lbuf) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (mkterms) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (varsub_loadfile) */


/* local subroutines */

int sub_loadfile::operator () (cchar *fn) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (ucstream fi ; (rs = fi.open(fn,"r",0666)) >= 0) ylikely {
	    cint	to = -1 ;
	    while ((rs = fi.readlns(lbuf,llen,to,np)) > 0) {
		if (int len ; (len = rmcomment(lbuf,llen)) > 0) {
		    rs = procln(lbuf,len) ;
		    c = rs ;
		} /* end if (comment) */
	        if (rs < 0) break ;
	    } /* end while (reading lines) */
	    rs1 = fi.close ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (open-file) */
	return (rs >= 0) ? c : rs ;
} /* end method (sub_loadfile::operator) */

int sub_loadfile::procln(cchar *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
        if (field fsb ; (rs = fsb.start(sp,sl)) >= 0) ylikely {
            cchar   *kp{} ;
            if (int kl ; (kl = fsb.get(fterms,&kp)) > 0) ylikely {
                int         al = alen ;
                char        *ap = abuf ;
                if ((kl == 6) && hasexport(kp,kl)) {
                    kl = fsb.get(fterms,&kp) ;
                } /* end if (skipping 'export' keyword') */
                for (int fl ; al > 0 ; ) {
                    fl = fsb.sharg(fterms,ap,al) ;
                    if (fl < 0) break ;
                    if (fl > 0) ap += fl ;
                    al = intconv(abuf + alen - ap) ;
                    if (fsb.term == '#') break ;
                } /* end for */
                *ap = '\0' ;
                al = intconv(ap - abuf) ;
                rs = varsub_add(op,kp,kl,abuf,al) ;
                if (rs < INT_MAX) c += 1 ;
            } /* end if (have a variable keyname) */
            rs1 = fsb.finish ;
            if (rs >= 0) rs = rs1 ;
        } /* end if (field) */
	return (rs >= 0) ? c : rs ;
} /* end method (sub_loadfile::procln) */

vars::operator int () noex {
	return fieldterms(fterms,false,' ','#','=') ;
} /* end method (vars::operator) */

local bool hasexport(cchar *sp,int sl) noex {
	return (strncasecmp(sp,"export",sl) == 0) ;
} /* end subroutine (hasexport) */


