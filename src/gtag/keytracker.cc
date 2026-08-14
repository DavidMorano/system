/* keytracker SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* object to track used keys */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debug print-outs */

/* revision history:

	= 1994-03-24, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Object:
	keytracker

	Description:
	This object tracks keys in a key-value pair-like structure
	(not uncommonly used with file DB queries, in the response
	to such queries).  It tracks whether the keys are "done"
	or not as determined by whether the "done" object method
	is called with the index of the key in the key-value array
	as an argument.  Yes, we could have used a hash table of
	the key names to indicate whether they were "done" or not,
	but for no good reason we used a bit array (indexed by the
	key-value index) to track the "done" status.

*****************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<bits.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<localmisc.h>		/* LIHU */

#include	"keytracker.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	KT	keytracker


/* external subroutines */


/* external variables */


/* forward references */

template<typename ... Args>
local inline int keytracker_ctor(keytracker *op,Args ... args) noex {
    	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    op->bap	= nullptr ;
	    op->keyvals = nullptr ;
	    op->n	= 0 ;
	    if (op->bap = new(nt) bits ; op->bap) {
		rs = SR_OK ;
	    } /* end if (new-bits) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (keytracker_ctor) */

local int keytracker_dtor(keytracker *op) noex {
	int		rs = SR_OK ;
	if (op->bap) ylikely {
	    delete op->bap ;
	    op->bap = nullptr ;
	}
	return rs ;
} /* end subroutine (keytracker_dtor) */

local int	keytracker_checkmore(keytracker *,cchar *,int) noex ;
local int	matkey(cchar *(*)[2],cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int keytracker_start(KT *op,cchar *(*keyvals)[2]) noex {
	int		rs ;
	int		n = 0 ;
	if ((rs = keytracker_ctor(op,keyvals)) >= 0) {
	    for (n = 0 ; keyvals[n][0] ; n += 1) ;
	    if ((rs = bits_start(op->bap,n)) >= 0) {
	        op->keyvals = keyvals ;
	    }
	    if (rs < 0) {
		keytracker_dtor(op) ;
	    } /* end if (error) */
	} /* end if (keytracker_ctor) */
	return (rs >= 0) ? n : rs ;
} /* end subroutine (keytracker_start) */

/* free up this keytracker object */
int keytracker_finish(KT *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->keyvals) {
		rs = SR_OK ;
		{
		    rs1 = bits_finish(op->bap) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        {
	            delete op->bap ;
	        }
		{
		    rs1 = keytracker_dtor(op) ;
		    if (rs >= 0) rs = rs1 ;
		}
		op->keyvals = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (keytracker_finish) */

int keytracker_done(KT *op,int n) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->keyvals) {
	        rs = SR_INVALID ;
	        if (n < op->n) {
		    rs = bits_set(op->bap,n) ;
	        } /* end if (valid) */
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (keytracker_done) */

/* are there more keys that have not been completed? */
int keytracker_more(KT *op,cchar *morestr) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		f = false ; /* return-value */
	if (op && morestr) {
	    rs = SR_NOTOPEN ;
	    if (op->keyvals) {
	        cchar	*sp = morestr ;
	        rs = SR_OK ;
	        for (cchar *tp ; (tp = strchr(sp,',')) != np ; ) {
		    cint tl = conv<int>(tp - sp) ;
	            f = keytracker_checkmore(op,sp,tl) ;
	            if (f) break ;
	            sp = (tp + 1) ;
	        } /* end for */
	        if ((! f) && (sp[0] != '\0')) {
	            f = keytracker_checkmore(op,sp,-1) ;
	        }
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (keytracker_more) */


/* private subroutines */

local int keytracker_checkmore(KT *op,cchar *sp,int sl) noex {
	int		f = false ; /* return-value */
	cchar		*cp ;
	if (sl < 0) sl = lenstr(sp) ;
	if (int cl ; (cl = sfnext(sp,sl,&cp)) >= 0) {
	    int	ki ;
	    if ((ki = matkey(op->keyvals,cp,cl)) >= 0) {
	        int	rc = bits_test(op->bap,ki) ;
	        f = (rc == 0) ;
	    }
	}
	return f ;
} /* end subroutine (keytracker_checkmore) */

local int matkey(cchar *(*keyvals)[2],cchar *kp,int kl) noex {
	int		i ; /* return-value */
	int		f = false ;
	if (kl < 0) kl = lenstr(kp) ;
	for (i = 0 ; keyvals[i][0] != nullptr ; i += 1) {
	    f = (strncmp(keyvals[i][0],kp,kl) == 0) ;
	    f = f && (keyvals[i][0][kl] == '\0') ;
	    f = f && (keyvals[i][1] != nullptr) ;
	} /* end for */
	return (f) ? i : -1 ;
} /* end subroutine (matkey) */


