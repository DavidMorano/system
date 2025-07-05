/* outline HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* output lines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

	= 1999-02-01, David A­D­ Morano
        I added a little code to "post" articles that do not have a valid
        newsgroup to a special "dead article" directory in the BB spool area.

*/

/* Copyright © 1998,1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	outline

	Description:
	This object deals with printing lines.

*******************************************************************************/

#ifndef	OUTLINE_INCLUDE
#define	OUTLINE_INCLUDE	


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<stddef.h>
#include	<stdlib.h>
#include	<stdarg.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<bfile.h>


#define	OUTLINE		struct outline_head
#define	OUTLINE_FL	struct outline_flags


struct outline_flags {
	uint		comma:1 ;
} ;

struct outline_head {
	bfile		*ofp ;
	OUTLINE_FL	fl ;
	int		maxlen ;
	int		rlen ;
	int		llen ;
	int		wlen ;
	int		c_values ;
	int		c_items ;
} ;

#ifdef	__cplusplus
enum outlinemems {
	outlinemem_finish,
	outlinemem_overlast
} ;
struct outline ;
struct outline_co {
	outline		*op = nullptr ;
	int		w = -1 ;
	void operator () (outline *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (outline_co) */
struct outline : outline_head {
	outline_co	finish ;
	outline() noex {
	    finish(this,outlinemem_finish) ;
	    ofp = nullptr ;
	} ;
	outline(const outline &) = delete ;
	outline &operator = (const outline &) = delete ;
	int start(bfile *,int = 0) noex ;
	int write(cchar *,int = -1) noex ;
	int vprintf(cchar *,va_list) noex ;
	int printf(cchar *,...) noex ;
	int item(cchar *,int = -1) noex ;
	int value(cchar *,int = -1) noex ;
	int needlength(int) noex ;
	void dtor() noex ;
	operator int () noex ;
	destruct outline() {
	    if (ofp) dtor() ;
	} ;
} ; /* end struct (outline) */
#else	/* __cplusplus */
typedef OUTLINE		outline ;
#endif /* __cplusplus */

typedef	OUTLINE_FL	outline_fl ;

EXTERNC_begin

extern int	outline_start(outline *,bfile *,int) noex ;
extern int	outline_finish(outline *) noex ;
extern int	outline_write(outline *,cchar *,int) noex ;
extern int	outline_vprintf(outline *,cchar *,va_list) noex ;
extern int	outline_printf(outline *,cchar *,...) noex ;
extern int	outline_item(outline *,cchar *,int) noex ;
extern int	outline_value(outline *,cchar *,int) noex ;
extern int	outline_needlength(outline *,int) noex ;

EXTERNC_end


#endif	/* OUTLINE_INCLUDE */


