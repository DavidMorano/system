/* pimkface SUPPORT */
/* lang=C++20 */

/* retrieve the FACE information for this user */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This subroutine is originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	pimkface

	Description:
	This subroutine reads the FACE image for the calling user
	(if it is available) and puts it into the given buffer.

	Synopsis:
	int pimkface(PROGINFO *pip,char *rbuf,int rlen) noex

	Arguments:
	pip		pointer to program information
	rbuf		result buffer
	rlen		length of result buffer

	Returns:
	>0		OK, got a face and returning length of result in buffer
	==0		did *not* get a face
	<0		error (system-error)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<sbuf.h>
#include	<bfile.h>
#include	<sfx.h>
#include	<mkpathx.h>
#include	<isnot.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* local defines */

#ifndef	FACEFNAME
#define	FACEFNAME	".xface"
#endif


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int pimkface(PROGINFO *pip,char *fbuf,int flen) noex {
	int		rs ;
	int		rs1 ;
	int		fl = 0 ;
	cchar		*fn = FACEFNAME ;
	char		tbuf[MAXPATHLEN + 1] ;

	if (pip == NULL) return SR_FAULT ;
	if (fbuf == NULL) return SR_FAULT ;

	pip->fl.init_face = TRUE ;
	if ((rs = mkpath2(tbuf,pip->homedname,fn)) >= 0) {
	    cint	am = R_OK ;
	    if ((rs = uc_access(tbuf,am)) >= 0) {
	        sbuf	b ;
	        if ((rs = sbuf_start(&b,fbuf,flen)) >= 0) {
	            bfile	xfile, *xfp = &xfile ;

	            if ((rs = bopen(xfp,tbuf,"r",0666)) >= 0) {
	                cint	llen = LINEBUFLEN ;
	                int		len ;
	                int		cl ;
			int		c = 0 ;
	                cchar	*cp ;
	                char		lbuf[LINEBUFLEN + 1] ;

	                while ((rs = breadln(xfp,lbuf,llen)) > 0) {
	                    len = rs ;

	                    while (len && iseol(lbuf[len-1])) len -= 1 ;

	                    if ((cl = sfshrink(lbuf,len,&cp)) > 0) {
				    if (c++ > 0) {
	                                rs = sbuf_chr(&b,' ') ;
				    }
				    if (rs >= 0) {
	                                rs = sbuf_strw(&b,cp,cl) ;
				    }
	                    }

	                    if (rs < 0) break ;
	                } /* end while */

	                rs1 = bclose(xfp) ;
	                if (rs >= 0) rs = rs1 ;
	            } else if (isNotPresent(rs)) {
	                rs = SR_OK ;
	            } /* end if (face-file) */

	            fl = sbuf_finish(&b) ;
	            if (rs >= 0) rs = fl ;
	        } /* end if (sbuf) */
	    } else if (isNotAccess(rs)) {
	        rs = SR_OK ;
	    }
	} /* end if (mkpath) */
	return (rs >= 0) ? fl : rs ;
}
/* end subroutine (mkface) */


