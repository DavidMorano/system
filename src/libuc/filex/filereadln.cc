/* filereadln SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* read a single line from a file! */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	filereadln

	Description:
	This subroutine reads a single line from a file (the string
	of which means something to someone).

	Synopsis:
	int filereadln(cchar *fname,char *rbuf,int rlen) noex

	Arguments:
	fname		file to read
	rbuf		user supplied buffer to hold result
	rlen		length of user supplied buffer

	Returns:
	>=0		length of returned string
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
#include	<sfx.h>			/* LIBUC */
#include	<sncpyxw.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"filereadln.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int reader(char *,int,char *,int,cchar *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int filereadln(cchar *fname,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (fname && rbuf) ylikely {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (fname[0] && (rlen > 0)) ylikely {
		if (char *lbuf ; (rs = lm_ml(&lbuf)) >= 0) ylikely {
		    cint	llen = rs ;
		    {
			rs = reader(lbuf,llen,rbuf,rlen,fname) ;
			len = rs ;
		    }
	            rs1 = lm_free(lbuf) ;
	            if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (filereadln) */


/* local subroutines */

local int reader(char *lbuf,int llen,char *rbuf,int rlen,cchar *fname) noex {
	cint		of = O_RDONLY ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	cmode		om = 0666 ;
        if ((rs = uc_open(fname,of,om)) >= 0) ylikely {
            cint    fd = rs ;
            if (filer b ; (rs = b.start(fd,0z,512,0)) >= 0) ylikely {
                while ((rs = b.readln(lbuf,llen)) > 0) {
                    cchar   *cp{} ;
                    if (int cl ; (cl = sfcontent(lbuf,rs,&cp)) > 0) {
                        rs = sncpy1w(rbuf,rlen,cp,cl) ;
                        len = rs ;
                    }
                    if (len > 0) break ;
                    if (rs < 0) break ;
                } /* end while (reading lines) */
                rs1 = filer_finish(&b) ;
                if (rs >= 0) rs = rs1 ;
            } /* end if (filer) */
            rs1 = uc_close(fd) ;
            if (rs >= 0) rs = rs1 ;
	} /* end if (ucfile) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (reader) */


