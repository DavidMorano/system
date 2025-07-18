/* gecos HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* parse a GECOS field located in a buffer */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GECOS_INCLUDE
#define	GECOS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	GECOS		struct gecos_head
#define	GECOS_VAL	struct gecos_value
#define	GECOS_ITEMLEN	256


enum gecosvals {
	gecosval_organization,
	gecosval_realname,
	gecosval_account,
	gecosval_bin,
	gecosval_office,
	gecosval_wphone,
	gecosval_hphone,
	gecosval_printer,
	gecosval_overlast
} ;

struct gecos_value {
	cchar		*vp ;
	int		vl ;
} ;

struct gecos_head {
	GECOS_VAL	vals[gecosval_overlast] ;
} ;

typedef GECOS_VAL	gecos_val ;

#ifdef	__cplusplus
enum gecosmems {
	gecosmem_finish,
	gecosmem_overlast
} ;
struct gecos ;
struct gecos_co {
	gecos		*op = nullptr ;
	int		w = -1 ;
	void operator () (gecos *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (gecos_co) */
struct gecos : gecos_head {
	gecos_co	finish ;
	gecos() noex {
	    finish(this,gecosmem_finish) ;
	    vals[gecosval_realname].vp = nullptr ;
	    vals[gecosval_realname].vl = 0 ;
	} ; /* end ctor */
	gecos(const gecos &) = delete ;
	gecos &operator = (const gecos &) = delete ;
	int start(cchar *,int = -1) noex ;
	int compose(char *,int) noex ;
	int getval(int,cchar **) noex ;
	void dtor() noex ;
	destruct gecos() {
	    cchar *p = vals[gecosval_realname].vp ;
	    if (p) dtor() ;
	} ;
} ; /* end struct (gecos) */
#else	/* __cplusplus */
typedef GECOS		gecos ;
#endif /* __cplusplus */


EXTERNC_begin

extern int	gecos_start(gecos *,cchar *,int) noex ;
extern int	gecos_compose(gecos *,char *,int) noex ;
extern int	gecos_getval(gecos *,int,cchar **) noex ;
extern int	gecos_finish(gecos *) noex ;

EXTERNC_end


#endif /* GECOS_INCLUDE */


