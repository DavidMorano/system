/* termconseq HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Terminal-Control-Sequene */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	TERMCONSEQ_INCLUDE
#define	TERMCONSEQ_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<stdarg.h>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */


#define	TERMCONSEQ_ARGS		struct termconseq_args


EXTERNC_begin

extern int termconseqva	(char *,int,int,cchar *,int,va_list) noex ;
extern int termconseqa	(char *,int,int,cchar *,int,...) noex ;

EXTERNC_end

#ifdef	__cplusplus

#ifdef	COMMENT

local inline int termconseq(char *dp,int dl,int n,
		int a1 = -1,int a2 = -1,int a3 = -1,int a4 = -1) noex {
	return termconseqa(dp,dl,n,nullptr,4,a1,a2,a3,a4) ;
} /* end subroutine */

#else /* COMMENT */

template<typename ... Args>
local inline int termconseq(char *dp,int dl,int nm,Args ... args) noex {
	std::nullptr_t	np{} ;
	cint		na = npack(Args) ;
	return termconseqa(dp,dl,nm,np,na,args ...) ;
} /* end subroutine */

#endif /* COMMENT */

local inline int termconseqi(char *dp,int dl,int n,cchar *is,
		int a1 = -1,int a2 = -1,int a3 = -1,int a4 = -1) noex {
	return termconseqa(dp,dl,n,is,4,a1,a2,a3,a4) ;
} /* end subroutine */

#else /* __cplusplus */

local inline int termconseq(char *dp,int dl,int n,
		int a1,int a2,int a3,int a4) noex {
	return termconseqa(dp,dl,n,nullptr,4,a1,a2,a3,a4) ;
} /* end subroutine */
local inline int termconseqi(char *dp,int dl,int n,cchar *is,
		int a1,int a2,int a3,int a4) noex {
	return termconseqa(dp,dl,n,is,4,a1,a2,a3,a4) ;
} /* end subroutine */

#endif /* __cplusplus */


#endif /* TERMCONSEQ_INCLUDE */


