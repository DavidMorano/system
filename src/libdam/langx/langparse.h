/* langparse HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Language-Parse (replacement for LANGSTATE) */
/* version %I% last-modified %G% */


/* revision history:

	= 2016-06-29, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2016 David A­D­ Morano.  All rights reserved. */

#ifndef	LANGPARSE_INCLUDE
#define	LANGPARSE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<limits.h>		/* |CHAR_BIT| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	LANGPARSE_MAGIC		0x13f3c205
#define	LANGPARSE		struct langparse_head
#define	LANGPARSE_FL		struct langparse_flags
#define	LANGPARSE_NSTAGE	2


enum langparseos {
	langparseo_comment = CHAR_BIT,	/* comment */
	langparseo_strreg,		/* string - regular */
	langparseo_strraw,		/* string - C++ raw literal */
	langparseo_literal,		/* C literal */
	langparseo_overlast
} ; /* end enum (langparses) */

#ifdef	__cplusplus
struct langparsems {
    	static int	comment ;
    	static int	strreg ;
    	static int	strraw ;
    	static int	literal ;
} ; /* end struct (langparsems) */
#endif /* __cplusplus */

struct langparse_flags {
	uint		comment:1 ;
	uint		clear:1 ;
	uint		strreg:1 ;
	uint		strraw:1 ;
	uint		literal:1 ;
	uint		paren:1 ;	/* parentheses inside raw-strings */
	uint		skip:1 ;
	uint		comline:1 ;
} ; /* end struct (langparse_flags) */

struct langparse_head {
	void		*outbuf ;	/* output-buffer pointer */
	void		*rstrp ;	/* raw-string pointer */
	LANGPARSE_FL	fl ;
	uint		magic ;
	int		pch ;		/* previous character */
} ; /* end struct (langparse_head) */

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
	int remread(short *,int) noex ;
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

extern int langparse_start	(langparse *) noex ;
extern int langparse_load	(langparse *,cchar *,int) noex ;
extern int langparse_remread	(langparse *,short *,int) noex ;
extern int langparse_finish	(langparse *) noex ;

EXTERNC_end

#ifdef	__cplusplus
extern const langparsems	langparsem ;
#endif /* __cplusplus */


#endif /* LANGPARSE_INCLUDE */


