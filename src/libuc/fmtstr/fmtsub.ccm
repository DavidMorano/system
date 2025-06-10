/* fmtsub MODULE (primary-module-interface) */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* subroutine to format string output */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	Of course, this subroutine was inspired by the UNIX®
	equivalent, but this is my own version for a) when I do not
	have the UNIX® libraries around, and b) to customize it to
	what I want!

	= 2020-02-15, David A­D­ Morano
	I modularized (w/ C++20 modules) this code.

*/

/* Copyright © 1998,2020 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	fmtsub

	Description:
	This (FMTSUB) is a helper object for the FMTSTR facility.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>

export module fmtsub ;

import libutil ;
import fmtstrdata ;
import fmtspec ;
import cvtfloat ;

#define	FMTSUB		fmtsub_head
#define	FMTSUB_FL	fmtsub_flags

export {
    struct fmtsub_flags {
	uint		ov:1 ;		/* overflow */
	uint		mclean:1 ;	/* mode: clean-up */
	uint		mnooverr:1 ;	/* mode: return-error */
    } ;
    struct fmtsub_head {
	char		*ubuf{} ;	/* user buffer */
	FMTSUB_FL	fl ;		/* flags */
	int		ulen ;		/* buffer length */
	int		len ;		/* current buffer usage count */
	int		mode ;		/* format mode */
    } ;
    enum fmtsubmems {
	fmtsubmem_audit,
	fmtsubmem_finish,
	fmtsubmem_overlast
    } ;
    struct fmtsub ;
    struct fmtsub_co {
	fmtsub		*op = nullptr ;
	int		w = -1 ;
	void operator () (fmtsub *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
    } ; /* end struct (fmtsub_co) */
    struct fmtsub : fmtsub_head {
	fmtsub_co	audit ;
	fmtsub_co	finish ;
	fmtsub() noex : fmtsub_head{} {
	    audit(this,fmtsubmem_audit) ;
	    finish(this,fmtsubmem_finish) ;
	} ;
	fmtsub(const fmtsub &) = delete ;
	fmtsub &operator = (const fmtsub &) = delete ;
	int start(char *,int,int) noex ;
	int cleanstrw(cchar *,int = -1) noex ;
	int strw(cchar *,int = -1) noex ;
	int chr(int) noex ;
	int blanks(int = 1) noex ;
	int emitter(fmtspec *,cchar *,int) noex ;
	int emit(fmtspec *,cchar *,int) noex ;
	int formstr(fmtspec *,fmtstrdata *) noex ;
	template<typename T> 
	int floater(char *tb,int tl,int fc,int fg,int wi,int pr,T dv) noex ;
	void dtor() noex ;
	destruct fmtsub() {
	    if (ubuf) dtor() ;
	} ;
    } ; /* end struct (fmtsub) */
    typedef	FMTSUB_FL	fmtsub_fl ;
} /* end export */

export {
    int fmtsub_start(fmtsub *,char *,int,int) noex ;
    int fmtsub_finish(fmtsub *) noex ;
    int fmtsub_strw(fmtsub *,cchar *,int) noex ;
    int fmtsub_chr(fmtsub *,int) noex ;
    int fmtsub_blanks(fmtsub *,int) noex ;
    int fmtsub_cleanstrw(fmtsub *,cchar *,int) noex ;
    int fmtsub_emit(fmtsub *,fmtspec *,cchar *,int) noex ;
    int fmtsub_formstr(fmtsub *,fmtspec *,fmtstrdata *) noex ;
    int fmtsub_reserve(fmtsub *,int) noex ;
    int fmtsub_audit(fmtsub *) noex ;
} /* end export */

template<typename T> 
int fmtsub::floater(char *tb,int tl,int fc,int fg,int wi,int pr,T dv) noex {
    	    int		rs ;
	    if ((rs = cvtfloat(tb,tl,fc,fg,wi,pr,dv)) >= 0) {
	        rs = strw(tb,rs) ;
	    }
	    return rs ;
} /* end method (floater) */

