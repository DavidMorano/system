/* percache HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* persistent cache */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-01-22, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PERCACHE_INCLUDE
#define	PERCACHE_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	PERCACHE	struct percache_head
#define	PERCACHE_ITEM	struct percache_dataitem


enum pertypes {
	pertype_hostid,
	pertype_btime,
	pertype_nprocs,
	pertype_runlevel,
	pertype_nusers,
	pertype_nodename,
	pertype_netdomain,
	pertype_nisdomain,
	pertype_netload,
	pertype_systat,
	pertype_overlast
} ; /* end enum (pertypes) */

struct percache_dataitem {
    	char		*data ;
	uint		t ;		/* time (of acquisition) */
	uint		v ;		/* value (acquired) */
} ;

struct percache_head {
	PERCACHE_ITEM	items[pertype_overlast] ;
	volatile uint	f_void ;
	volatile uint	f_init ;
	volatile uint	f_initdone ;
	volatile uint	f_finireg ;
} ; /* end struct (percache_head) */

typedef PERCACHE	percache ;
typedef	PERCACHE_ITEM	percache_item ;

EXTERNC_begin

extern int percache_init	(percache *) noex ;
extern int percache_finireg	(percache *) noex ;
extern int percache_invalidate	(percache *) noex ;
extern int percache_gethostid	(percache *,time_t,uint *) noex ;
extern int percache_getbtime	(percache *,time_t,time_t *) noex ;
extern int percache_getnprocs	(percache *,time_t) noex ;
extern int percache_getrunlevel	(percache *,time_t) noex ;
extern int percache_getnusers	(percache *,time_t) noex ;
extern int percache_getnodename	(percache *,time_t,cchar **) noex ;
extern int percache_getnetdomain(percache *,time_t,cchar **) noex ;
extern int percache_getnisdomain(percache *,time_t,cchar **) noex ;
extern int percache_getnetload	(percache *,time_t,cchar *,cchar **) noex ;
extern int percache_getsystat	(percache *,time_t,cchar *,cchar **) noex ;
extern int percache_fini	(percache *) noex ;

EXTERNC_end


#endif /* PERCACHE_INCLUDE */


