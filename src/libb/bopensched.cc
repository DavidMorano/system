/* bopensched SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* open a file name according to rules */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */

/* revision history:

	= 1998-09-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	This subroutine will form a file name according to some
	rules.  We try in order:

	programroot/etc/name/name.fname
	programroot/etc/name/fname
	programroot/etc/name.fname
	programroot/name.fname
	programroot/fname

	if (programroot != PWD) {

		etc/name/name.fname
		etc/name/fname
		etc/name.fname
		name.fname
		fname

	}

	%p/%e/%n/%n.%f
	%p/%e/%n/%f
	%p/%e/%n.%f
	%p/%n.%f
	%p/%f
	
	%e/%n/%n.%f
	%e/%n.%f
	%e/%n.%f
	%n.%f
	%f

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strchr(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<sbuf.h>
#include	<vecstr.h>
#include	<outbuf.h>
#include	<permx.h>
#include	<vstrkeycmp.h>
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */

#define	MODELEN		12
#define	KEYBUF(c)	(keybuf[0] = c,keybuf[1] = '\0',keybuf)


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	schedexpand(cchar *,VECSTR *,cchar *,char *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int bopensched(fp,sched,nsp,fname,outname,mode,permission)
bfile		*fp ;
cchar	*sched[] ;
VECSTR		*nsp ;
cchar	fname[] ;
char		outname[] ;
cchar	mode[] ;
int		permission ;
{
	OUTBUF		ob ;
	int		rs = SR_OK ;
	int		i, sl ;
	int		imode ;
	int		f_create = FALSE ;
	cchar	*mp ;
	char		openmode[MODELEN + 1], *omp = openmode ;
	char		*tmpfname ;

	if ((fp == nullptr) || (sched == nullptr)) return SR_FAULT ;

	if (nsp == nullptr) return SR_NOEXIST ;

/* no check on 'fname' because that may be OK! */

#if	CF_DEBUGS
	    debugprintf("bopensched: ent fname=%s\n",fname) ;
#endif

	imode = 0 ;
	for (mp = mode ; *mp && (mp < (mode + MODELEN)) ; mp += 1) {
	    int	ch = (*mp & 0xff) ;

	    switch (ch) {

	    case 'r':
	        *omp++ = *mp ;
	        imode += R_OK ;
	        break ;

	    case 'w':
	        *omp++ = *mp ;
	        imode += W_OK ;
	        break ;

	    case 'x':
	        *omp++ = *mp ;
	        imode += X_OK ;
	        break ;

	    case 'c':
	        f_create = TRUE ;
	        break ;

	    default:
	        *omp++ = *mp ;
		break ;

	    } /* end switch */

	} /* end for */

	*omp = '\0' ;

/* get ready */

	rs = outbuf_start(&ob,outname,-1) ;
	if (rs < 0)
	    goto ret0 ;

	rs = outbuf_get(&ob,&tmpfname) ;
	if (rs < 0)
	    goto ret1 ;

/* loop through the schedules, expanding them as we go */

	rs = SR_NOEXIST ;
	for (i = 0 ; sched[i] ; i += 1) {

	    sl = schedexpand(sched[i],nsp,fname,tmpfname,MAXPATHLEN) ;
	    if (sl < 0) continue ;

	    if ((rs = perm(tmpfname,-1,-1,nullptr,R_OK)) >= 0)
	        break ;

	} /* end for */

	if (rs < 0) {

	    rs = SR_NOEXIST ;
	    if (f_create) {

/* continue, but this time we create the file as we go! */

	        for (i = 0 ; sched[i] ; i += 1) {

	            sl = schedexpand(sched[i],nsp,fname,tmpfname,MAXPATHLEN) ;
	            if (sl < 0) continue ;

	            if ((rs = bopen(fp,tmpfname,mode,permission)) >= 0)
	                break ;

	        } /* end for */

	    } /* end if (creating) */

	} else
	    rs = bopen(fp,tmpfname,mode,permission) ;

/* we are done */
ret1:
	outbuf_finish(&ob) ;

ret0:
	return (rs >= 0) ? sl : rs ;
} /* end subroutine (bopensched) */


/* local subroutines */

local int schedexpand(cc *fmt,vecstr *nsp,cc *fname,char *buf,int buflen) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	cchar	*fp ;
	cchar	*tp, *cp ;
	char		keybuf[2] ;

#if	CF_DEBUGS
	debugprintf("bopensched/expand: ent fname=%s\n",fname) ;
#endif

	buf[0] = '\0' ;
	if (buflen <= 0)
	    return SR_TOOBIG ;

	sbuf		buffer ;
	if ((rs = sbuf_start(&buffer,buf,buflen)) >= 0) {
	for (fp = fmt ; *fp && (rs >= 0) ; fp += 1) {
	    if (*fp == '%') {
	        fp += 1 ;
	        if (! *fp) break ;
	        if (*fp == '%') {
	            rs = sbuf_chr(&buffer,'%') ;
	        } else if (*fp == 'f') {
	            rs = SR_FAULT ;
	            if (fname != nullptr) {
	                rs = sbuf_strw(&buffer,fname,-1) ;
		    }
	        } else {
	            rs = vecstr_finder(nsp,KEYBUF(*fp),vstrkeycmp,&cp) ;
	            if (rs >= 0) {
	                rs = 0 ;
	                if ((tp = strchr(cp,'=')) != nullptr) {
	                    rs = sbuf_strw(&buffer,(tp+1),-1) ;
	                }
	            }
	        }
	    } else {
	        rs = sbuf_chr(&buffer,*fp) ;
	    } /* end if */
	} /* end for */
	rs1 = sbuf_finish(&buffer) ;
	if (rs >= 0) rs = rs1 ;
	} /* end if (sbuf) */

#if	CF_DEBUGS
	debugprintf("bopensched/expand: ret rs=%d\n",rs) ;
#endif

	return rs ;
} /* end subroutine (schedexpand) */


