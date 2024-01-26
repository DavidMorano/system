/* permsched SUPPORT */
/* lang=C++20 */

/* find a file according to rules */
/* version %I% last-modified %G% */

#define	CF_BADNOFILE	0		/* is it bad if there was no file? */

/* revision history:

	= 1998-09-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	permsched

	Description:
	This subroutine will form a file name according to some
	rules.

	Synopsis:
	int permsched(mainv sched,vecstr *nsp,char *rbuf,int rlen,
			cc *fn,mode_t am) noex

	Arguments:
	sched		array of strings forming the search schedule
	nsp		string-list of substitution keys
	rbuf		user supplied buffer to hold resuling filepath
	rlen		length of user supplied buffer
	fn		filename to search for
	am		access-mode needed to get a file hit

	Returns:
	>=0	length of found filepath
	<0	error (system-error)

	Notes:
	A typical example is something like trying the following:

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
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<sbuf.h>
#include	<ids.h>
#include	<vecstr.h>
#include	<vstrkeycmpx.h>
#include	<localmisc.h>

#include	"xperm.h"


/* local defines */

#define	KBUF(c)	(keybuf[0] = (c),keybuf[1] = '\0',keybuf)


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	schedexpand(cchar *,vecstr *,char *,int,cchar *) noex ;


/* local variables */


/* exported subroutines */

int permsched(mainv sched,vecstr *nsp,char *rbuf,int rlen,
		cc *fn,mode_t am) noex {
	ids		id ;
	int		rs ;
	int		sl = 0 ;

	if (sched == NULL) return SR_FAULT ;
	if (rbuf == NULL) return SR_FAULT ;
	if (nsp == NULL) return SR_FAULT ;

#if	CF_BADNOFILE
	if ((fn == NULL) || (fn[0] == '\0'))
	    return SR_NOEXIST ;
#endif

	if ((rs = ids_load(&id)) >= 0) {
	    rs = SR_NOEXIST ;
	    for (int i = 0 ; sched[i] ; i += 1) {
	        if ((rs = schedexpand(sched[i],nsp,rbuf,rlen,fname)) >= 0) {
	            USTAT	sb ;
	            sl = rs ;
	            if ((rs = uc_stat(rbuf,&sb)) >= 0) {
	                rs = sperm(&id,&sb,am) ;
	            }
	        } /* end if (schedexpand) */
	        if (rs >= 0) break ;
	    } /* end for */

	    ids_release(&id) ;
	} /* end if (ids) */

	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (permsched) */


/* local subroutines */

static int schedexpand(cc *fmt,vecstr *nsp,char *rbuf,int rlen,cc *fn) noex {
	sbuf		b ;
	int		rs ;
	int		len = 0 ;
	char		keybuf[2] ;

	rbuf[0] = '\0' ;
	if (rlen <= 0)
	    return SR_TOOBIG ;

	if ((rs = sbuf_start(&b,rbuf,rlen)) >= 0) {
	    int		(*vs)(cchar **,cchar **) = vstrkeycmp ;
	    cchar	*fp ;
	    cchar	*tp, *cp ;

	    for (fp = fmt ; *fp && (rs >= 0) ; fp += 1) {

	        if (*fp == '%') {

	            fp += 1 ;
	            if (! *fp) break ;
	            if (*fp == '%') {
	                rs = sbuf_char(&b,'%') ;
	            } else {
	                if ((rs = vecstr_finder(nsp,KBUF(*fp),vs,&cp)) >= 0) {
	                    if ((tp = strchr(cp,'=')) != NULL) {
	                        rs = sbuf_strw(&b,(tp + 1),-1) ;
	                    } /* end if (it had a value) */
	                } else if (*fp == 'f') {
	                    rs = SR_FAULT ;
	                    if (fn != NULL) {
	                        rs = sbuf_strw(&b,fn,-1) ;
			    }
	                } else {
	                    rs = SR_NOTFOUND ;
			}
	            } /* end if (tried to expand a key) */
	        } else {
	            rs = sbuf_char(&b,*fp) ;
	        } /* end if (escape or regular character) */

	    } /* end for */

	    len = sbuf_finish(&b) ;
	    if (rs >= 0) rs = len ;
	} /* end if (sbuf) */

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (schedexpand) */


