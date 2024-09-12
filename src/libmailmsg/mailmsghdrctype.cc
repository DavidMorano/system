/* mailmsghdrct SUPPORT */
/* lang=C++20 */

/* manage content-types with header field */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object module parses a "content-type" header specification.
	The parsed results are broken into three types of items:
	the type, the sub-type, and parameters.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/param.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mkpathx.h>
#include	<char.h>
#include	<localmisc.h>

#include	"mailmsghdrct.h"


/* local defines */

#define	MMHCT		mailmsghdrct
#define	MMHCT_PAR	mailmsghdrct_par


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int mailmsghdrct_start(MMHCT *op,cchar *hp,int hl) noex {
	mode_t	operms = 0660 ;

	int	rs ;
	int	vopts ;
	int	oflags = O_RDWR ;
	int	nmsgs = 0 ;

	const char	*tmpdname ;

	char	tpat[MAXPATHLEN + 1] ;
	char	tmpfname[MAXPATHLEN + 1] ;


	if (op == NULL)
	    return SR_FAULT ;

	if (mfd < 0)
	    return SR_BADF ;

	memclear(op) ;

	op->tfd = -1 ;
	op->f.useclen = (opts & MAILMSGHDRCT_OUSECLEN) ? 1 : 0 ;
	op->f.useclines = (opts & MAILMSGHDRCT_OUSECLINES) ? 1 : 0 ;

	tmpdname = getenv(VARTMPDNAME) ;

	if (tmpdname == NULL)
	    tmpdname = MAILMSGHDRCT_TMPDNAME ;

	rs = mkpath2(tpat,tmpdname,"msXXXXXXXXXXXX") ;

	if (rs >= 0) {
	    rs = opentmpfile(tpat,oflags,operms,tmpfname) ;
	    op->tfd = rs ;
	}

	if (rs < 0)
	    goto bad0 ;

	rs = uc_mallocstrw(tmpfname,-1,&op->tmpfname) ;
	if (rs < 0)
	    goto bad1 ;

	vopts = (VECHAND_OCOMPACT | VECHAND_OSTATIONARY) ;
	rs = vechand_start(&op->msgs,4,vopts) ;
	if (rs < 0)
	    goto bad2 ;

	rs = mailmsghdrct_gather(op,mfd,to) ;
	nmsgs = rs ;
	if (rs < 0)
	    goto bad3 ;

	if (op->tflen > 0) {
	    cnullptr	np{} ;
	    size_t	ms = op->tflen ;
	    char	*p ;
	    int		mp = PROT_READ ;
	    int		mf = MAP_SHARED ;
	    if ((rs = u_mmapbegin(np,ms,mp,mf,op->tfd,0L,&p)) >= 0) {
	        op->mapdata = p ;
	        op->mapsize = msize ;
	    }
	} /* end if */

	op->magic = MAILMSGHDRCT_MAGIC ;

ret0:
	return (rs >= 0) ? nmsgs : rs ;

/* bad stuff */
bad3:
	mailmsghdrct_msgsfree(op) ;

	vechand_finish(&op->msgs) ;

bad2:
	if (op->tmpfname != NULL) {
	    uc_free(op->tmpfname) ;
	    op->tmpfname = NULL ;
	}

bad1:
	if (op->tfd >= 0) {
	    u_close(op->tfd) ;
	    op->tfd = -1 ;
	}

	if (tmpfname != NULL) {
	    if (tmpfname[0] != '\0') {
	        u_unlink(tmpfname) ;
		tmpfname[0] = '\0' ;
	    }
	}

bad0:
	goto ret0 ;
}
/* end subroutine (mailmsghdrct_start) */

int mailmsghdrct_finish(MMHCT *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;


	if (op == NULL)
	    return SR_FAULT ;

	if (op->magic != MAILMSGHDRCT_MAGIC)
	    return SR_NOTOPEN ;

	if ((op->mapsize > 0) && (op->mapdata != NULL)) {
	    u_munmap(op->mapdata,op->mapsize) ;
	    op->mapdata = NULL ;
	    op->mapsize = 0 ;
	}

	rs1 = mailmsghdrct_msgsfree(op) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = vechand_finish(&op->msgs) ;
	if (rs >= 0) rs = rs1 ;

	if (op->tfd >= 0) {
	    u_close(op->tfd) ;
	    op->tfd = -1 ;
	}

	if (op->tmpfname) {
	    if (op->tmpfname[0] != '\0') {
	        u_unlink(op->tmpfname) ;
		op->tmpfname[0] = '\0' ;
	    }
	    rs1 = uc_free(op->tmpfname) ;
	    op->tmpfname = NULL ;
	    if (rs >= 0) rs = rs1 ;
	}

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (mailmsghdrct_finish) */

int mailmsghdrct_paramget(MMHCT *op,int i,cchar **rpp) noex {
	int		rs ;

	if (op == NULL)
	    return SR_FAULT ;

	if (op->magic != MAILMSGHDRCT_MAGIC)
	    return SR_NOTOPEN ;

	rs = vechand_count(&op->msgs) ;

	return rs ;
}
/* end subroutine (mailmsghdrct_paramget) */

int mailmsghdrct_paramfind(MMHCT *op,cchar *key,cchar **rpp) noex {
	int		rs ;

	if (op == NULL) return SR_FAULT ;
	if (op->magic != MAILMSGHDRCT_MAGIC) return SR_NOTOPEN ;

	rs = vechand_count(&op->msgs) ;

	return rs ;
}
/* end subroutine (mailmsghdrct_paramfind) */


