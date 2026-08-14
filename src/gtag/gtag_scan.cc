/* gtag_progscan SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* process a file by inserting bibliographical references */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */
#define	CF_DEBUG	0		/* run-time debug print-outs */
#define	CF_MULTICITE	1		/* allow multiple citations */

/* revision history:

	= 1992-03-01, David A­D­ Morano
	This code was originally written.

	= 1998-09-01, David A­D­ Morano
	This subroutine was modified to process the way MMCITE does
	citation.  It used to use the old GNU 'lookbib' program in
	addition to the (old) standard UNIX version.  But neither
	of these are used now.  Straight out citeation keywrd lookup
	is done directly in a BIB database (files of which are
	suffixed 'rbd').

*/

/* Copyright © 1992,1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	This subroutine processes a file by looking up and inserting
	the bibliographical references into the text.  All input is
	copied to the output with the addition of the bibliographical
	references.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<paramopt.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<ascii.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<bfile.h>		/* LIBB */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"gtag_config.h"
#include	"defs.h"
#include	"tagtrack.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#ifndef	PI
#define	PI	proginfo
#endif

#define	SI	subinfo

#define	PO	paramopt


/* external subroutines */


/* external variables */


/* local structures */

struct subinfo {
	PO		*app ;
	TAGTRACK	*ttp ;
	int		fi ;
} ; /* end struct (subinfo) */

struct mbdinfo {
	cchar		*pp ;
	cchar		*rp ;
	cchar		*kp ;
	uint		loff ;
	int		pl ;
	int		rl ;
	int		kl ;
} ; /* end struct (mdbinfo) */


/* forward references */

#ifdef	COMMENT
local int mbdescape(PI *,struct mbdinfo *,uint,cchar *,int) noex ;
#endif


/* local variables */


/* exported variables */


/* exported subroutines */

int progscan(PI *pip,PO *app,TAGTRACK *ttp,int fi,cchar *fname) noex {
	SI		pc ;
	bfile		ifile, *ifp = &ifile ;
	int		rs ;
	int		rs1 ;
	int		tlen = 0 ;
	cchar	*cp ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("progscan: fname=%s\n",fname) ;
#endif

	if (fname == nullptr) return SR_FAULT ;

	if (fname[0] == '\0') return SR_INVALID ;

	memclear(&pc) */
	pc.app = app ;
	pc.ttp = ttp ;

/* enter the filename into storage for possible need later (on error) */

	cp = fname ;
	if (cp[0] == '-')
	    cp = "*STDIN*" ;

/* proceed to open the file */

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("progscan: open file\n") ;
#endif

	if (fname[0] == '-') fname = BFILE_STDIN ;

	if ((rs = bopen(ifp,fname,"r",0666)) >= 0) {
	    bfile	*tfp = &pip->tf.tfile ;
	    cint	llen = LINEBUFLEN ;
	    uint	foff = pip->tf.tlen ;
	    int		ll ;
	    int		f_bol, f_eol ;
	    cchar	*pn = pip->progname ;
	    cchar	*fmt ;
	    cchar	*lp ;
	    char	lbuf[LINEBUFLEN + 1] ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(3))
	        debugprintf("progscan: while-above\n") ;
#endif

	    f_bol = true ;
	    while ((rs = breadln(ifp,lbuf,llen)) > 0) {
	        uint	loff = (foff+tlen) ;
	        int	len = rs ;

	        f_eol = (lbuf[len - 1] == '\n') ;

#if	CF_DEBUG
	        if (DEBUGLEVEL(2))
	            debugprintf("progscan: begin loff=%u\n",loff) ;
#endif

#if	CF_DEBUG
	        if (DEBUGLEVEL(3))
	            debugprintf("progscan: line=>%r<\n",lbuf,
	                ((lbuf[len - 1] == '\n') ? (len - 1) : len)) ;
#endif

/* check for macros and escapes */

	        lp = lbuf ;
	        ll = (f_eol) ? (len - 1) : len ;
	        if (f_bol) {
		    if ((rs = tagtrack_scanline(ttp,fi,loff,lp,ll)) > 0) {

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	debugprintf("progscan: got a macro li=%d\n",rs) ;
#endif

	                rs = bprintln(tfp,".\\\" TAG\n",-1) ;
		        tlen += rs ;
		    } else if (rs == SR_INVALID) {
		        fmt = "%s: same label for multiple tags (%d)\n" ;
		        bprintf(pip->efp,fmt,pn,rs) ;
	                rs = bprinter(tfp,f_eol,lp,ll) ;
	                tlen += rs ;
		    } else if (rs >= 0) {
	                rs = bprinter(tfp,f_eol,lp,ll) ;
	                tlen += rs ;
		    } else if (rs < 0) {
		        fmt = "%s: error in scaning file=%d (%d)\n" ;
		        bprintf(pip->efp,fmt,pn,fi,rs) ;
		    }
	        } else {
	            rs = bprinter(tfp,f_eol,lp,ll) ;
	            tlen += rs ;
	        } /* end if (specialized processing) */

	        f_bol = f_eol ;
	        if (rs < 0) break ;
	    } /* end while (reading input lines) */

	    rs1 = bclose(ifp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (file-open) */
	pip->tf.tlen += tlen ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("progscan: ret rs=%d tlen=%u\n",rs,tlen) ;
#endif

	return (rs >= 0) ? tlen : rs ;
} /* end subroutine (progscan) */


/* local subroutines */

#ifdef	COMMENT
local int mbdescape(pip,ip,loff,lp,ll)
PI	*pip ;
struct mbdinfo	*ip ;
uint		loff ;
cchar	*lp ;
int		ll ;
{
	cint		el = lenstr(BIBESCAPE) ;
	int		sl, cl ;
	int		si ;
	int		f = false ;
	cchar	*tp ;
	cchar	*sp, *cp ;

	memclear(ip) ;
	ip->loff = loff ;
	ip->pp = lp ;
	ip->pl = ll ;
	ip->kp = nullptr ;
	ip->kl = 0 ;
	ip->rp = lp ;
	ip->rl = ll ;
	if ((si = sicite(lp,ll,BIBESCAPE,el)) >= 0) {
	    ip->pl = si ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(2))
	        debugprintf("progscan/mbdescape: pl=%u\n",ip->pl) ;
#endif

	    sp = (lp + (si + el + 1)) ;
	    sl = ll - (si + el + 1) ;
	    if ((si = silbrace(sp,sl)) >= 0) {

	        cp = (sp + si + 1) ;
	        cl = sl - (cp - sp) ;
	        tp = strnchr(cp,cl,CH_RBRACE) ;

	        if (tp != nullptr) {

	            f = true ;

#if	CF_MULTICITE
	            ip->kp = cp ;
	            ip->kl = (tp - cp) ;
#else
	            ip->kl = nextfield(cp,(tp - cp),&ip->kp) ;
#endif /* CF_MULTICITE */

	            ip->rp = (tp + 1) ;
	            ip->rl = sl - ((tp + 1) - sp) ;

	        } else {
	            f = false ;
		}

	    } /* end if (open brace) */

	} /* end if (possible escape) */

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("progscan/mbdescape: ret f=%u\n",f) ;
#endif

	return f ;
}
/* end subroutine (mbdescape) */
#endif /* COMMENT */


