/* bbhosts HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* get the list of BB hosts from a PCS site BB hosts file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	BBHOSTS_INCLUDE
#define	BBHOSTS_INCLUDE
#ifdef	__cplusplus /* C++ only */


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<vecpstr.h>


#define	BBHOSTS_MAGIC	0x86543267


EXTERNC_begin

typedef int (*bbhosts_vcmp)(cchar **,cchar **) noex ;
typedef int (*bbhosts_f)(cchar **,cchar **) noex ;

EXTERNC_end

enum bbhostsmems {
	bbhostsmem_count,
	bbhostsmem_audit,
	bbhostsmem_finish,
	bbhostsmem_overlast
} ; /* end enum (bbhostsmems) */
struct bbhosts ;
struct bbhosts_co {
	bbhosts		*op = nullptr ;
	int		w = -1 ;
	void operator () (bbhosts *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (bbhosts_co) */
struct bbhosts : vecpstr {
	bbhosts_co	count ;
	bbhosts_co	audit ;
	bbhosts_co	finish ;
	uint		magic ;
	bbhosts() noex {
	    count	(this,bbhostsmem_count) ;
	    audit	(this,bbhostsmem_audit) ;
	    finish	(this,bbhostsmem_finish) ;
	    magic = 0 ;
	} ; /* end ctor */
	bbhosts(const bbhosts &) = delete ;
	bbhosts &operator = (const bbhosts &) = delete ;
	int start	(cchar *,cchar *) noex ;
	int get		(int,cchar **) noex ;
	int find	(cchar *) noex ;
	operator int () noex ;
	void dtor() noex ;
	destruct bbhosts() {
	    if (magic) dtor() ;
	} ;
} ; /* end struct (bbhosts) */

extern int	bbhosts_start(bbhosts *,cchar *,cchar *) noex ;
extern int	bbhosts_finish(bbhosts *) noex ;
extern int	bbhosts_get(bbhosts *,int,cchar **) noex ;
extern int	bbhosts_count(bbhosts *) noex ;
extern int	bbhosts_audit(bbhosts *) noex ;
extern int	bbhosts_find(bbhosts *,cchar *) noex ;


#endif /* __cplusplus (C++ only) */
#endif /* BBHOSTS_INCLUDE */


