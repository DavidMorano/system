/* statvfsdir SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* like |statvfs(2)| but will not return zero blocks on automounts */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	The subroutine was written from scratch.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	Name:
	statvfsdir

	Description:
	This subroutine performs almost exactly like |statvfs(2)|.
	The difference is that if zero total-blocks are returned
	by the OS we assume that an unmounted automount point was
	accessed.  In this case we will try to access something
	inside the directory in order to get it mounted so that a
	second attempt will succeed.

	Synopsis:
	int statvfsdir(cchar *fname,STATVFS *sbp) noex

	Arguments:
	fname		source string
	sbp		pointer to 'statvfs' structure

	Return:
	>=0		OK
	<0		error (system-return)

******************************************************************************/

#include	<envstandards.h>	/* must be before others */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/statvfs.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<fsdir.h>
#include	<mkpathx.h>
#include	<localmisc.h>

#include	"statvfsdir.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	trydive(cchar *,STATVFS *) noex ;
static int	trytouch(cchar *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int statvfsdir(cchar *fname,STATVFS *sbp) noex {
	int		rs = SR_FAULT ;
	if (fname && sbp) {
	    if ((rs = uc_statvfs(fname,sbp)) >= 0) {
	        if (sbp->f_blocks == 0) {
		    rs = trydive(fname,sbp) ;
	        } /* end if (zero-blocks) */
	    } /* end if (uc_statvfs) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (statvfsdir) */


/* local subroutines */

static int trydive(cchar *fname,STATVFS *sbp) noex {
	int		rs ;
	int		rs1 ;
        if (USTAT sb ; (rs = uc_stat(fname,&sb)) >= 0) {
            if (S_ISDIR(sb.st_mode)) {
                 if (char *tbuf{} ; (rs = malloc_mp(&tbuf)) >= 0) {
                     rs1 = SR_NOENT ;
                     if ((rs = mkpath2(tbuf,fname,".")) >= 0) {
                         if ((rs1 = u_stat(tbuf,&sb)) >= 0) {
                             rs = uc_statvfs(fname,sbp) ;
                         }
                     }
                     if (rs >= 0) {
                         bool       f = false ;
                         f = f || (rs1 == SR_NOENT) ;
                         f = f || (rs1 == SR_ACCESS) ;
                         if (f) {
                             if ((rs1 = trytouch(fname)) >= 0) {
                                 rs = uc_statvfs(fname,sbp) ;
                             }
                         }
                     } /* end if (ok) */
                     rs1 = uc_free(tbuf) ;
                     if (rs >= 0) rs = rs1 ;
                 } /* end if (m-a-f) */
            } /* end if (is-dir) */
        } /* end if (uc_stat) */
	return rs ;
}
/* end subroutine (trydive) */

static int trytouch(cchar *fname) noex {
	int		rs ;
	int		rs1 ;
	if (char *nbuf{} ; (rs = malloc_mn(&nbuf)) >= 0) {
	    cint	nlen = rs ;
	    if (fsdir dir ; (rs = fsdir_open(&dir,fname)) >= 0) {
		fsdir_ent	ds ;
	        while ((rs = fsdir_read(&dir,&ds,nbuf,nlen)) > 0) {
	            cchar	*sp = ds.name ;
	            if (sp[0] != '.') break ;
	            if ((sp[1] != '\0') && (sp[1] != '.')) break ;
	        } /* end while */
	        rs1 = fsdir_close(&dir) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (fsdir) */
	    rs1 = uc_free(nbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (trytouch) */


