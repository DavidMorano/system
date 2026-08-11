/* filereadstrs SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* read a string(s) from a file! */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	filereadstrs

	Description:
	This subroutine reads strings from a file (the string of
	which means something to someone).  Comments and empty lines
	are ignored.  So the first line that has some content is
	used as the souce for a line of c-strings.

	Synopsis:
	int filereadstrs(cchar *fname,char *rbuf,int rlen) noex

	Arguments:
	fname		file to read
	rbuf		user supplied buffer to hold result
	rlen		length of user supplied buffer

	Returns:
	>=0		length of return organization string
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<filer.h>		/* LIBUC */
#include	<sfx.h>			/* |sfcontent(3uc)| + |sfnext(3uc)| */
#include	<strwcpy.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"filereadstrs.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	reader(cchar *,char *,int) noex ;


/* local variables */

cint		bsz		= 512 ; /* buffer size */


/* exported variables */


/* exported subroutines */

int filereadstrs(cchar *fname,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (fname && rbuf) ylikely {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (fname[0] && (rlen > 0)) ylikely {
		rs = reader(fname,rbuf,rlen) ;
		rl = rs ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (filereadstrs) */


/* local subroutines */

local int reader(cchar *fname,char *rbuf,int rlen) noex {
	int		rs ;
	int		rs1 ;
	int		rl = 0 ;
        if (char *lbuf ; (rs = lm_ml(&lbuf)) >= 0) ylikely {
	    cint	llen = rs ;
            cint        of = O_RDONLY ;
            cmode       om = 0 ;
            if ((rs = uc_open(fname,of,om)) >= 0) ylikely {
                cint    fd = rs ;
                if (filer b ; (rs = b.start(fd,0z,bsz)) >= 0) ylikely {
                    int         c = 0 ;
                    while ((rs = b.readln(lbuf,llen)) > 0) {
			cchar   *sp{} ;
                        if (int sl ; (sl = sfcontent(lbuf,rs,&sp)) > 0) {
			    cchar	*cp{} ;
                            for (int cl ; (cl = sfnext(sp,sl,&cp)) > 0 ; ) {
				bool	fs ; /* used-afterwards */
                                if ((fs = ((rlen - rl) >= (cl+1)))) {
                                    if (c++ > 0) {
                                        rbuf[rl++] = ' ' ;
                                    }
                                    strwcpy((rbuf+rl),cp,cl) ;
                                    rl += cl ;
                                } /* end if (have space) */
				sl -= intconv((cp + cl) - sp) ;
				sp = (cp + cl) ;
				if (! fs) break ;
                            } /* end for */
                        } /* end if (sfcontent) */
                        if (rs < 0) break ;
                    } /* end while (reading lines) */
                    rs1 = b.finish ;
                   if (rs >= 0) rs = rs1 ;
                } /* end if (filer) */
                rs1 = uc_close(fd) ;
                if (rs >= 0) rs = rs1 ;
            } /* end if (open) */
            rs1 = lm_free(lbuf) ;
            if (rs >= 0) rs = rs1 ;
        } /* end if (m-a-f) */
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (reader) */


