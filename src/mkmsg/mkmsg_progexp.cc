/* mkmsg_progexp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* support building a message without output related subroutines */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* run-time debug print-outs */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	The subroutine was written from scratch but based on previous
	versions of the 'mkmsg' program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	mkmsg_progexp

	Description:
	Output a header.

	Synopsis:
	int progexp(PI *pip,char *rbuf,int rlen,cchar *sbuf,int slen) noex

	Arguments:
	pip
	rbuf
	rlen
	sp
	sl

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<buffer.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEUG |DPRINTF(3debug)| */

#include	"mksmg_config.h"
#include	"defs.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* external subroutines */

extern int	pcsgetfacility(cchar *,char *,int) noex ;


/* external variables */


/* external variables */


/* local structures */

enum cooks {
	cook_varprname,
	cook_s,
	cook_v,
	cook_facility,
	cook_b,
	cook_overlast
} ; /* end enum */

constexpr cpcchar	cooks[] = {
	"varprname",
	"s",
	"v",
	"facility",
	"b",
	nullptr
} ; /* end array */


/* forward references */

local int progexp_beginner(PI *) noex ;
local int progexp_loadcooks(PI *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int progexp_begin(PI *pip) noex {
	int		rs = SR_OK ;
	if (pip) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (progexp_begin) */

int progexp_end(PI *pip) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (pip) {
	    rs = SR_OK ;
	    if (pip->open.pec) {
	        pip->open.pec = false ;
	        rs1 = expcook_finish(&pip->pec) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (progexp_end) */

int progexp_sub(PI *pip,char *rbuf,int rlen,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ; /* return-value */
	if (pip && rbuf && sp) {
	    if (sl < 0) sl = lenstr(sp) ;
	    rs = SR_OK ;
	    if (! pip->open.pec) {
	        rs = progexp_beginner(pip) ;
	    }
	    if (rs >= 0) {
	        expcook	*ecp = &pip->pec ;
	        cint	wch = MKCHAR('¿') ;
	        rs = expcook_exp(ecp,wch,rbuf,rlen,sp,sl) ;
	        rl = rs ;
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (progexp_sub) */

int progexp_subbuf(PI *pip,buffer *bufp,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ; /* return-value */
	if (pip && bufp && sp) {
	    rs = SR_OK ;
	    if (sl < 0) sl = strlen(sp) ;
	    DEBUGPRINTF("s=>%r<\n", sp,strlinelen(sp,sl,50)) ;
	    if (! pip->open.pec) {
	        rs = progexp_beginner(pip) ;
	    }
	    if (rs >= 0) {
	        expcook	*ecp = &pip->pec ;
	        cint	wch = MKCHAR('¿') ;
	        rs = expcook_expbuf(ecp,wch,bufp,sp,sl) ;
	        rl = rs ;
	    } /* end if */
	} /* end if (non-null) */
	DEBUGPRINTF("ret rs=%d rl=%u\n",rs,rl) ;
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (progexp_subbuf) */


/* local subroutines */

local int progexp_beginner(PI *pip) noex {
	int		rs = SR_OK ;
	if (! pip->open.pec) {
	    if (expcook	*ecp = &pip->pec ; (rs = expcook_start(ecp)) >= 0) {
		pip->open.pec = true ;
		rs = progexp_loadcooks(pip) ;
		if (rs < 0) {
		    pip->open.pec = false ;
		    expcook_finish(ecp) ;
		} /* end if (error */
	    } /* end if (expcook_start) */
	} /* end if */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (progexp_beginner) */

local int progexp_loadcooks(PI *pip) noex {
	expcook		*ecp = &pip->pec ;
	int		rs = SR_OK ;
	for (int i = 0 ; cooks[i] ; i += 1) {
	    int		vl = -1 ;
	    cchar	*vp = nullptr ;
	    switch (i) {
	    case cook_varprname:
		vp = VARPRNAME ;
		break ;
	    case cook_s:
		vp = pip->searchname ;
		break ;
	    case cook_v:
		vp = pip->version ;
		break ;
	    case cook_facility:
		vp = pip->facility ;
		break ;
	    case cook_b:
		vp = pip->banner ;
		break ;
	    } /* end switch */
	    if ((rs >= 0) && (vp != nullptr)) {
		rs = expcook_add(ecp,cooks[i],vp,vl) ;
	    }
	    if (rs < 0) break ;
	} /* end for */
	return rs ;
} /* end subroutine (progexp_loadcooks) */


