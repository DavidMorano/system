/* outstore HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* manage some dynamic output storage (mostly for SHIO use) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	OUTSTORE_INCLUDE
#define	OUTSTORE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	OUTSTORE	struct outstore_head
#define	OUTSTORE_FL	struct outstore_flags
#define	OUTSTORE_SLEN	128


struct outstore_flags {
    	uint		open:1 ;
} ; /* end struct (outstore_flags) */

struct outstore_head {
	char		*dbuf ;	/* dynamically sized (allocated) */
	OUTSTORE_FL	fl ;
	int		dlen ;
	int		len ;
	char		sbuf[OUTSTORE_SLEN+1] ; /* "static" or "stack" */
} ;

#ifdef	__cplusplus
enum outstoremems {
	outstoremem_start,
	outstoremem_clear,
	outstoremem_finish,
	outstoremem_overlast
} ;
struct outstore ;
struct outstore_co {
	outstore	*op = nullptr ;
	int		w = -1 ;
	void operator () (outstore *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (outstore_co) */
struct outstore : outstore_head {
	outstore_co	start ;
	outstore_co	clear ;
	outstore_co	finish ;
	OUTSTORE_FL	fl{} ;
	outstore() noex {
	    start(this,outstoremem_start) ;
	    clear(this,outstoremem_clear) ;
	    finish(this,outstoremem_finish) ;
	    dbuf = nullptr ;
	} ;
	outstore(const outstore &) = delete ;
	outstore &operator = (const outstore &) = delete ;
	int strw(cchar *,int = -1) noex ;
	int str(cchar *,int = -1) noex ;
	int get(cchar **) noex ;
	void dtor() noex ;
	operator int () noex ;
	destruct outstore() {
	    if (dbuf || fl.open) dtor() ;
	} ;
} ; /* end struct (outstore) */
#else	/* __cplusplus */
typedef OUTSTORE	outstore ;
#endif /* __cplusplus */

typedef OUTSTORE_FL	outstore_fl ;

EXTERNC_begin

extern int	outstore_start(outstore *) noex ;
extern int	outstore_strw(outstore *,cchar *,int) noex ;
extern int	outstore_get(outstore *,cchar **) noex ;
extern int	outstore_clear(outstore *) noex ;
extern int	outstore_finish(outstore *) noex ;

EXTERNC_end


#endif /* OUTSTORE_INCLUDE */


