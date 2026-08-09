/* hello (HELLO) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* this is a MFSERVE loadable service-module */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */

/* revision history:

	= 2008-10-07, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	This object is a MFSERVE loadable service-module.  This
	basically just prints out "Hello world!".

	Synopsis:
	int hello_start(op,pr,jep,argv,envv)
	HELLO		*op ;
	const char	*pr ;
	SREQ		*jep ;
	const char	**argv ;
	const char	**envv ;

	Arguments:
	op		object pointer
	pr		program-root
	sn		search-name (of program calling us)
	argv		array-arguments
	envv		array-environment array

	Returns:
	>=0		OK
	<0		error code

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<estrings.h>
#include	<buffer.h>
#include	<localmisc.h>

#include	"mfserve.h"
#include	"hello.h"


/* local defines */

#ifndef	ORGCODELEN
#define	ORGCODELEN	MAXNAMELEN
#endif

#ifndef	ORGCODELEN
#define	ORGCODELEN	USERNAMELEN
#endif

#define	HELLO_CSIZE	100 	/* default arg-chuck size */


/* typedefs */

extern "C" {
    typedef int	(*thrsub_t)(void *) noex ;
}


/* external subroutines */

#if	CF_DEBUGS
extern int	debugprintf(cchar *,...) ;
extern int	strlinelen(cchar *,int,int) ;
#endif


/* external variables */


/* local structures */


/* forward references */

local int hello_argsbegin(HELLO *,cchar **) ;
local int hello_argsend(HELLO *) ;


/* local variables */


/* exported variables */

MFSERVE_MOD	hello = {
	"hello",
	szof(HELLO),
	0
} ;


/* exported subroutines */

int hello_start(HELLO *op,cchar *pr,SREQ *jep,mainv argv,mainv envv) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == NULL) return SR_FAULT ;

#if	CF_DEBUGS
	debugprintf("hello_start: ent {%p}\n",op) ;
	debugprintf("hello_start: pr=%s\n",pr) ;
#endif

	if (pr == NULL) return SR_FAULT ;
	if (argv == NULL) return SR_FAULT ;
	if (envv == NULL) return SR_FAULT ;

	memset(op,0,sizeof(HELLO)) ;
	op->pr = pr ;
	op->jep = jep ;
	op->envv = envv ;

	if ((rs = sreq_getstdout(jep)) >= 0) {
	    op->ofd = rs ;
	    if ((rs = hello_argsbegin(op,argv)) >= 0) {
	        BUFFER	b ;
		if ((rs = buffer_start(&b,100)) >= 0) {
		    cchar	*s = "Hello world!" ;
		    if ((rs = buffer_strw(&b,s,-1)) >= 0) {
			int	i ;
			for (i = 0 ; argv[i] != NULL ; i += 1) {
			    buffer_strw(&b," ­ ",-1) ;
			    rs = buffer_strw(&b,argv[i],-1) ;
			    if (rs < 0) break ;
			} /* end for */
			if (rs >= 0) {
			    cchar	*bp ;
			    if ((rs = buffer_get(&b,&bp)) >= 0) {
				cint	bl = rs ;
			        if ((rs = uc_writen(op->ofd,bp,bl)) >= 0) {
				    if ((rs = sreq_closefds(jep)) >= 0) {
					op->magic = HELLO_MAGIC ;
				    }
				}
			    }
			} /* end if (ok) */
		    }
		    rs1 = buffer_finish(&b) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (buffer) */
	        if (rs < 0)
	            hello_argsend(op) ;
	    } /* end if (hello_argsbegin) */
	} /* end if (sreq_getstdout) */

#if	CF_DEBUGS
	debugprintf("hello_start: ret rs=%d\n",rs) ;
#endif

	op->f_exiting = true ;
	return rs ;
}
/* end subroutine (hello_start) */

int hello_finish(HELLO *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

#if	CF_DEBUGS
	debugprintf("hello_finish: ent {%p}\n",op) ;
#endif

	if (op == NULL) return SR_FAULT ;

	if (op->magic != HELLO_MAGIC) return SR_NOTOPEN ;

#if	CF_DEBUGS
	debugprintf("hello_finish: f_working=%d\n",op->fl.working) ;
#endif

	rs1 = hello_argsend(op) ;
	if (rs >= 0) rs = rs1 ;

#if	CF_DEBUGS
	debugprintf("hello_finish: ret rs=%d\n",rs) ;
#endif

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (hello_finish) */

int hello_check(HELLO *op) noex {
	int		rs = SR_OK ;
	int		f = false ;

#if	CF_DEBUGS
	debugprintf("hello_check: ent {%p}\n",op) ;
#endif

	if (op == NULL) return SR_FAULT ;

	if (op->magic != HELLO_MAGIC) return SR_NOTOPEN ;

	f = op->f_exiting ;

#if	CF_DEBUGS
	debugprintf("hello_check: ret rs=%d f=%u\n",rs,f) ;
#endif

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (hello_check) */

int hello_abort(HELLO *op) noex {
	int	f ;
#if	CF_DEBUGS
	debugprintf("hello_abort: ent {%p}\n",op) ;
#endif
	if (op == NULL) return SR_FAULT ;
	if (op->magic != HELLO_MAGIC) return SR_NOTOPEN ;
	f = op->f_exiting ;
#if	CF_DEBUGS
	debugprintf("hello_abort: cont f=%u\n",f) ;
#endif
	op->f_abort = true ;
	return f ;
}
/* end subroutine (hello_abort) */


/* provate subroutines */

local int hello_argsbegin(HELLO *op,cchar **argv) noex {
	vecpstr		*alp = &op->args ;
	cint		ss = HELLO_CSIZE ;
	int		rs ;
	if ((rs = vecpstr_start(alp,5,0,ss)) >= 0) {
	    op->fl.args = true ;
	    for (int i = 0 ; (rs >= 0) && (argv[i] != NULL) ; i += 1) {
	        rs = vecpstr_add(alp,argv[i],-1) ;
	    }
	    if (rs < 0) {
	        op->fl.args = false ;
	        vecpstr_finish(alp) ;
	    }
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (hello_argsbegin) */

local int hello_argsend(HELLO *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fl.args) {
	    vecpstr	*alp = &op->args ;
	    rs1 = vecpstr_finish(alp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (hello_argsend) */


