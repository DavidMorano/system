/* kshvar SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* KSH variable framework */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */
#define	CF_DEBUG	0		/* switchable at invocation */
#define	CF_DEBUGSIG	0		/* debug csignalandling */
#define	CF_DEBUGMALL	1		/* debug memory allocation */

/* revision history:

	= 2004-03-01, David A­D­ Morano
	This code was originally written.  

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/**************************************************************************

	Synopsis:
	int kshvar_start(KSHVAR *op)

*****************************************************************************/

#if	defined(SFIO) && (SFIO > 0)
#define	CF_SFIO	1
#else
#define	CF_SFIO	0
#endif

#if	(defined(KSHBUILTIN) && (KSHBUILTIN > 0))
#include	<shell.h>
#endif

#include	<envstandards.h>	/* MUST be first to configure */

#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/time.h>		/* for 'gethrtime(3c)' */
#include	<climits>
#include	<csignal>
#include	<unistd.h>
#include	<fcntl.h>
#include	<dlfcn.h>
#include	<cstdlib>
#include	<cstring>
#include	<ctype.h>

#include	<usystem.h>
#include	<sigman.h>
#include	<baops.h>
#include	<keyopt.h>
#include	<vecstr.h>
#include	<field.h>
#include	<exitcodes.h>
#include	<localmisc.h>

#include	"shio.h"
#include	"kshvar.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* external variables */


/* local variables */


/* exported variables */


/* exported subroutines */

int kshvar_start(KSHVAR *op) noex {
	cint	sz = sizeof(KSHVAR_VAR) ;
	int	rs ;
	memclear(op) ;
	if ((rs = vecobj_start(&op->vars,sz,0,0)) >= 0) {
	    op->magval = KSHVAR_MAGIC ;
	}
	return rs ;
} /* end subroutine (kshvar_start) */

int kshvar_finish(KSHVAR *op) noex {
	KSHVAR_VAR	*ep ;
	int	rs = SR_OK ;
	int	rs1 ;
	if (op == NULL) return SR_FAULT ;
	if (op->magval != KSHVAR_MAGIC) return SR_NOTOPEN ;
	for (int i = 0 ; vecobj_get(&op->vars,i,&ep) >= 0 ; i += 1) {
	    if (ep == NULL) continue ;
	    varname = ep->varname ;
	} /* end for */
	op->magval = 0 ;
	return rs ;
} /* end subroutine (kshvar_finish) */






