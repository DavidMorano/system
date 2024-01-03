/* field HEADER */
/* lang=C20 */

/* structure definition for field extraction calls */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This code module was originally written in VAX assembly
	(back around 1980 or so). It was then written into C-language
	perhaps around 1983 (at AT&T Bell Laboratories).  This
	(below) is a new incarnation based on that older C-language
	code.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is a classic, ported forward from the old VAX-11/70
	days. This was translated from VAX assembly language.

*******************************************************************************/

#ifndef	FIELD_INCLUDE
#define	FIELD_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>


#define	FIELD		struct field_head


#ifndef	TYPEDEF_UCHAR
#define	TYPEDEF_UCHAR
typedef unsigned char	uchar ;
#endif /* TYPEDEF_UCHAR */

struct field_head {
	cchar		*lp ;		/* line pointer */
	cchar		*fp ;		/* field pointer */
	int		ll ;		/* line length remaining */
	int		fl ;		/* field length */
	int		term ;		/* terminating character */
} ; /* end struct (field_head) */

typedef FIELD		field ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int	fieldterms(char *,int,cchar *) noex ;
extern int	fieldtermsx(char *,int,int,...) noex ;

extern int	field_start(field *,cchar *,int) noex ;
extern int	field_get(field *,cchar *,cchar **) noex ;
extern int	field_term(field *,cchar *,cchar **) noex ;
extern int	field_sharg(field *,cchar *,char *,int) noex ;
extern int	field_remaining(field *,cchar **) noex ;
extern int	field_finish(field *) noex ;

#ifdef	__cplusplus
}
#endif

#ifdef	__cplusplus

template<typename ... Args>
static inline int fieldterms(char *t,int f,Args ... args) noex {
	cint		na = npack(Args) ;
	return fieldtermsx(t,f,na,args ...) ;
}

#endif /* __cplusplus */


#endif /* FIELD_INCLUDE */


