/* lookaddr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Addres-Look-List */
/* version %I% last-modified %G% */


/* revision history:

	= 2002-05-17, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2002 David A­D­ Morano.  All rights reserved. */

#ifndef	LOOKADDR_INCLUDE
#define	LOOKADDR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecstr.h>
#include	<whitelist.h>


#define	LOOKADDR_MAGIC		0x97658232
#define	LOOKADDR		struct lookaddr_head
#define	LOOKADDR_SL		struct lookaddr_sysflags
#define	LOOKADDR_US		struct lookaddr_user
#define	LOOKADDR_UF		struct lookaddr_userflags


struct lookaddr_sysflags {
	uint		swl:1 ;
	uint		sbl:1 ;
} ;

struct lookaddr_userflags {
	uint		dname:1 ;
	uint		uwl:1 ;
	uint		ubl:1 ;
} ;

struct lookaddr_user {
	cchar		*dname ;
	LOOKADDR_UF	init ;
	LOOKADDR_UF	open ;
	whitelist	uwl ;
	whitelist	ubl ;
	uint		magic ;
} ;

struct lookaddr_head {
    	char		*pbuf ;
	vecstr		*svp ;
	whitelist	swl ;
	whitelist	sbl ;
	LOOKADDR_SL	init ;
	LOOKADDR_SL	open ;
	uint		magic ;
	int		plen ;
	int		users ;
} ;

typedef	LOOKADDR	lookaddr ;
typedef	LOOKADDR_SL	lookaddr_sf ;
typedef	LOOKADDR_US	lookaddr_us ;
typedef	LOOKADDR_UF	lookaddr_uf ;

EXTERNC_begin

extern int lookaddr_start(lookaddr *,cchar *,cchar *) noex ;
extern int lookaddr_userbegin(lookaddr *,lookaddr_us *,cchar *) noex ;
extern int lookaddr_usercheck(lookaddr *,lookaddr_us *,cchar *,int) noex ;
extern int lookaddr_userend(lookaddr *,lookaddr_us *) noex ;
extern int lookaddr_finish(lookaddr *) noex ;

EXTERNC_end


#endif /* LOOKADDR_INCLUDE */


