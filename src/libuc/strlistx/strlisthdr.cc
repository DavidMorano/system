/* strlisthdr SUPPORT */
/* lang=C++20 */

/* string-list database-file header */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	This subroutine writes out a STRLIST file.

	Synopsis:
	int strlisthdr(strlisthdr *ep,int f,char *hbuf,int hlen) noex

	Arguments:
	- ep		object pointer
	- f		read=1, write=0
	- hbuf		buffer containing object
	- hlen		length of buffer

	Returns:
	>=0		OK
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* must be before others */
#include	<sys/param.h>
#include	<unistd.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<endian.h>
#include	<localmisc.h>

#include	"strlisthdr.h"


/* local defines */

#ifndef	ITEMLEN
#define	ITEMLEN		100
#endif


/* external subroutines */

extern int	sncpy2(char *,int,cchar *,cchar *) ;
extern int	mkmagic(char *,int,cchar *) ;
extern int	sfshrink(cchar *,int,cchar **) ;
extern int	sfbasename(cchar *,int,cchar **) ;
extern int	sfdirname(cchar *,int,cchar **) ;

extern char	*strwcpy(char *,cchar *,int) ;
extern char	*strnchr(cchar *,int,int) ;


/* external variables */


/* local structures */

enum his {
	hi_fsize,			/* file size */
	hi_wtime,			/* write (creation) time */
	hi_stoff,			/* string table offset */
	hi_stlen,			/* string table length (entries) */
	hi_rtoff,			/* record table offset */
	hi_rtlen,			/* record table length */
	hi_itoff,			/* index (hash) table offset */
	hi_itlen,			/* index (hash) table length */
	hi_nstrs,			/* total number of strings */
	hi_nskip,			/* hash lookup paramter */
	hi_overlast
} ;


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int strlisthdr_msg(strlisthdr *ep,int f,char *hbuf,int hlen) noex {
	uint		*header ;
	cint	magicsize = STRLISTHDR_MAGICSIZE ;
	int		rs = SR_OK ;
	int		headsize ;
	int		bl, cl ;
	cchar	*magicstr = STRLISTHDR_MAGICSTR ;
	cchar	*tp, *cp ;
	char		*bp ;

	if (ep == NULL) return SR_FAULT ;
	if (hbuf == NULL) return SR_FAULT ;

	bp = hbuf ;
	bl = hlen ;
	headsize = hi_overlast * sizeof(uint) ;
	if (f) { /* read */

/* the magic string is with the first 16 bytes */

	    if ((rs >= 0) && (bl > 0)) {
	        if (bl >= magicsize) {

	            cp = bp ;
	            cl = magicsize ;
	            if ((tp = strnchr(cp,cl,'\n')) != NULL)
	                cl = (tp - cp) ;

	            bp += magicsize ;
	            bl -= magicsize ;

/* verify the magic string */

	            if (strncmp(magicstr,cp,cl) != 0)
	                rs = SR_NXIO ;

	        } else
	            rs = SR_ILSEQ ;

	    } /* end if (item) */

/* read out the VETU information */

	    if ((rs >= 0) && (bl > 0)) {
	        if (bl >= 4) {

	            memcpy(ep->vetu,bp,4) ;

	            if (ep->vetu[0] != STRLISTHDR_VERSION)
	                rs = SR_PROTONOSUPPORT ;

	            if ((rs >= 0) && (ep->vetu[1] != ENDIAN))
	                rs = SR_PROTOTYPE ;

	            bp += 4 ;
	            bl -= 4 ;

	        } else
	            rs = SR_ILSEQ ;
	    } /* end if (item) */

	    if ((rs >= 0) && (bl > 0)) {
	        if (bl >= headsize) {

	            header = (uint *) bp ;

	            ep->fsize = header[hi_fsize] ;
	            ep->wtime = header[hi_wtime] ;
	            ep->stoff = header[hi_stoff] ;
	            ep->stlen = header[hi_stlen] ;
	            ep->rtoff = header[hi_rtoff] ;
	            ep->rtlen = header[hi_rtlen] ;
	            ep->itoff = header[hi_itoff] ;
	            ep->itlen = header[hi_itlen] ;
	            ep->nstrs = header[hi_nstrs] ;
	            ep->nskip = header[hi_nskip] ;

	            bp += headsize ;
	            bl -= headsize ;

	        } else
	            rs = SR_ILSEQ ;
	    } /* end if (item) */

	} else { /* write */

	    mkmagic(bp, magicsize, magicstr) ;
	    bp += magicsize ;
	    bl -= magicsize ;

	    memcpy(bp,ep->vetu,4) ;
	    *bp = STRLISTHDR_VERSION ;
	    bp += 4 ;
	    bl -= 4 ;

	    if ((rs >= 0) && (bl >= headsize)) {

	        header = (uint *) bp ;

	        header[hi_fsize] = ep->fsize ;
	        header[hi_wtime] = ep->wtime ;
	        header[hi_stoff] = ep->stoff ;
	        header[hi_stlen] = ep->stlen ;
	        header[hi_rtoff] = ep->rtoff ;
	        header[hi_rtlen] = ep->rtlen ;
	        header[hi_itoff] = ep->itoff ;
	        header[hi_itlen] = ep->itlen ;
	        header[hi_nstrs] = ep->nstrs ;
	        header[hi_nskip] = ep->nskip ;

	        bp += headsize ;
	        bl -= headsize ;

	    } /* end if */

	} /* end if */

	return (rs >= 0) ? (bp - hbuf) : rs ;
}
/* end subroutine (strlisthdr) */


