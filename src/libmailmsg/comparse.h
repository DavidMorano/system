/* comparse HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* comment-parse (for RFC822) small strings (like for stupid RFC822 date) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-22, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	COMPARSE_INCLUDE
#define	COMPARSE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	COMPARSE_MAGIC	0x77811276
#define	COMPARSE	struct comparse_head
#define	COMPARSE_STR	struct comparse_string
#define	COMPARSE_DEFSZ	80


struct comparse_string {
	cchar		*sp ;
	int		sl ;
} ;

struct comparse_head {
	COMPARSE_STR	val ;
	COMPARSE_STR	com ;
	uint		magic ;
	int		mailaddrlen ;
	int		mailcommlen ;
} ;

typedef COMPARSE_STR	comparse_str ;

#ifdef	__cplusplus
enum comparsemems {
	comparsemem_finish,
	comparsemem_overlast
} ;
struct comparse ;
struct comparse_co {
	comparse	*op = nullptr ;
	int		w = -1 ;
	void operator () (comparse *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (comparse_co) */
struct comparse : comparse_head {
	comparse_co	finish ;
	comparse() noex {
	    finish	(this,comparsemem_finish) ;
	    magic = 0 ;
	} ;
	comparse(const comparse &) = delete ;
	comparse &operator = (const comparse &) = delete ;
	int start(cchar *,int = -1) noex ;
	int getval(cchar **) noex ;
	int getcom(cchar **) noex ;
	void dtor() noex ;
	destruct comparse() {
	    if (magic) dtor() ;
	} ;
} ; /* end struct (comparse) */
#else	/* __cplusplus */
typedef COMPARSE	comparse ;
#endif /* __cplusplus */

EXTERNC_begin

extern int comparse_start(comparse *,cchar *,int) noex ;
extern int comparse_getval(comparse *,cchar **) noex ;
extern int comparse_getcom(comparse *,cchar **) noex ;
extern int comparse_finish(comparse *) noex ;

EXTERNC_end


#endif /* COMPARSE_INCLUDE */


