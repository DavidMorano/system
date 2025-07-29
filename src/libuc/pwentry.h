/* pwentry HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* PW-ENTRY object and interface */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-00-00, David A-D- Morano
	This was originally written.

	= 2018-12-21, David A-D- Morano
	Added |pwentrybufsize(3dam)|.  I added this to facilitate
	buffer allocations for using this object.

****/

/* Copyright © 1998,2018 David A-D- Morano.  All rights reserved. */

#ifndef	PWENTRY_INCLUDE
#define	PWENTRY_INCLUDE	


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system IDs */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#ifndef	PWENTRY
#define	PWENTRY		struct pwentry_head
#endif


struct pwentry_head {
	cchar		*username ;
	cchar		*password ;
	cchar		*gecos ;
	cchar		*dir ;
	cchar		*shell ;
	cchar		*organization ;
	cchar		*realname ;
	cchar		*account ;
	cchar		*bin ;
	cchar		*name_f ;
	cchar		*name_m1 ;
	cchar		*name_m2 ;
	cchar		*name_l ;
	cchar		*office ;
	cchar		*wphone ;
	cchar		*hphone ;
	cchar		*printer ;
	long		lstchg ;	/* password lastchanged date */
	long		daymin ;	/* min days between password changes */
	long		daymax ;	/* max days password is valid */
	long		warn ;		/* days to warn user to change passwd */
	long		inact ;		/* days the login may be inactive */
	long		expire ;	/* login expiration date */
	uint		flag ;		/* currently not being used */
	uid_t		uid ;
	gid_t		gid ;
} ;

#ifdef	__cplusplus
enum pwentrymems {
    	pwentrymem_start,
	pwentrymem_mkextras,
	pwentrymem_finish,
	pwentrymem_overlast
} ;
struct pwentry ;
struct pwentry_co {
	pwentry		*op = nullptr ;
	int		w = -1 ;
	void operator () (pwentry *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (pwentry_co) */
struct pwentry : pwentry_head {
	pwentry_co	start ;
	pwentry_co	mkextras ;
	pwentry_co	finish ;
	pwentry() noex {
	    start(this,pwentrymem_start) ;
	    mkextras(this,pwentrymem_mkextras) ;
	    finish(this,pwentrymem_finish) ;
	    username = nullptr ;
	} ;
	pwentry(const pwentry &) = delete ;
	pwentry &operator = (const pwentry &) = delete ;
	int fieldpw(int,cchar *,int = -1) noex ;
	int mkcopy(pwentry *,char *,int) noex ;
	void dtor() noex ;
	destruct pwentry() {
	    if (username) dtor() ;
	} ;
} ; /* end struct (pwentry) */
#else	/* __cplusplus */
typedef PWENTRY		pwentry ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	pwentry_start(pwentry *) noex ;
extern int	pwentry_finish(pwentry *) noex ;
extern int	pwentry_fieldpw(pwentry *,int,cchar *,int) noex ;
extern int	pwentry_mkextras(pwentry *) noex ;
extern int	pwentry_mkcopy(pwentry *,pwentry *,char *,int) noex ;

extern int	pwentrybufsize(void) noex ;

EXTERNC_end


#endif /* PWENTRY_INCLUDE */


