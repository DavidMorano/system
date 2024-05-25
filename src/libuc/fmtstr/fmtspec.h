/* fmtspec HEADER */
/* lang=C20 */

/* format-specification */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	Of course, this subroutine was inspired by the UNIX®
	equivalent, but this is my own version for a) when I do not
	have the UNIX® libraries around, and b) to customize it to
	what I want!

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************


*******************************************************************************/

#ifndef	FMTSPEC_INCLUDE
#define	FMTSPEC_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>
#include	<cstdlib>
#include	<cstdint>
#include	<cstring>
#include	<wchar.h>
#include	<stdarg.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<ascii.h>
#include	<stdintx.h>
#include	<localmisc.h>


#define	FMTSPEC_FL	struct fmtspec_flags


enum lenmods {
	lenmod_none,
	lenmod_half,
	lenmod_long,
	lenmod_longlong,
	lenmod_longdouble,
	lenmod_wide,
	lenmod_overlast
} ;

enum fmtspecmems {
	fmtspecmem_finish,
	fmtspecmem_overlast
} ;

struct fmtspec_flags {
	uint		alternate:1 ;	/* alternate form */
	uint		zerofill:1 ;	/* zero fill on left */
	uint		plus:1 ;	/* leading plus sign */
	uint		minus:1 ;	/* leading minus sign */
	uint		left:1 ;	/* left justified */
	uint		thousands:1 ;	/* group by thousands */
	uint		space:1 ;	/* leading space */
	uint		isdigit:1 ;	/* is a digit-type character */
} ;

struct fmtspec_head {
	FMTSPEC_FL	f ;
	short		fcode ;		/* the format "code" */
	short		width ;		/* <0 turns off */
	short		prec ;		/* <0 turns off */
	short		lenmod ;	/* length modifier */
} ;

struct fmtspec ;

struct fmtspec_co {
	fmtspec		*op = nullptr ;
	int		w = -1 ;
	void operator () (vecstr *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
} ; /* end struct (fmtspec_co) */

struct fmtspec : fmtspec_head {
	friend		fmtspec_co ;
	fmtspec_co	finish ;
	fmtspec() noex : fmtspec_head{} {
	    finish(this,fmtspecmem_finish) ;
	} ;
	int start(cchar *,int) noex ;
	int ifinish() noex ;
} ;

typedef	FMTSPEC_FL	fmtspec_fl ;


#endif /* __cplusplus */
#endif /* FMTSPEC_INCLUDE */


