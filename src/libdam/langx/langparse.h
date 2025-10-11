/* langparse HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Language-Parse (replacement for LANGSTATE) */
/* version %I% last-modified %G% */


/* revision history:

	= 2016-06-29, David A�D� Morano
	This code was originally written.

*/

/* Copyright � 2016 David A�D� Morano.  All rights reserved. */

#ifndef	LANGPARSE_INCLUDE
#define	LANGPARSE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	LANGPARSE_MAGIC		0x13f3c205
#define	LANGPARSE		struct langparse_head
#define	LANGPARSE_FL		struct langparse_flags
#define	LANGPARSE_NSTAGE	2


struct langparse_flags {
	uint		clear:1 ;
	uint		literal:1 ;
	uint		quote:1 ;
	uint		comment:1 ;
	uint		skip:1 ;
} ;

struct langparse_head {
	void		*outbuf ;	/* output-buffer */
	LANGPARSE_FL	fl ;
	uint		magic ;
	int		rl ;		/* stage length */
	int		pch ;
	char		rb[LANGPARSE_NSTAGE + 1] ;	/* stage buffer */
} ;

typedef	LANGPARSE_FL	langparse_fl ;

#ifdef	__cplusplus
enum langparsemems {
    	langparsemem_start,
	langparsemem_finish,
	langparsemem_overlast
} ;
struct langparse ;
struct langparse_co {
	langparse	*op = nullptr ;
	int		w = -1 ;
	void operator () (langparse *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (langparse_co) */
struct langparse : langparse_head {
	langparse_co	start ;
	langparse_co	finish ;
	langparse() noex {
	    start	(this,langparsemem_start) ;
	    finish	(this,langparsemem_finish) ;
	    magic = 0 ;
	} ; /* end ctor */
	langparse(const langparse &) = delete ;
	langparse &operator = (const langparse &) = delete ;
	int load(cchar *,int = -1) noex ;
	int read(short *,int) noex ;
	void dtor() noex ;
	operator int () noex ;
	destruct langparse() {
	    if (magic) dtor() ;
	} ;
} ; /* end struct (langparse) */
#else	/* __cplusplus */
typedef LANGPARSE	langparse ;
#endif /* __cplusplus */

EXTERNC_begin

extern int langparse_start	(langparse *,int) noex ;
extern int langparse_load	(langparse *,cchar *,int) noex ;
extern int langparse_read	(langparse *,short *,int) noex ;
extern int langparse_finish	(langparse *) noex ;

EXTERNC_end


#endif /* LANGPARSE_INCLUDE */


