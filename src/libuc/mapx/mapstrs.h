/* mapstrs HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* environment container */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MAPSTRS_INCLUDE
#define	MAPSTRS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<hdb.h>
#include	<strpack.h>


/* object defines */
#define	MAPSTRS			struct mapstrs_head
#define	MAPSTRS_CHUNKSZ		200
#define	MAPSTRS_MAGIC		0x20109143


struct mapstrs_head {
	hdb		*dbp ;
	strpack		*spp ;
	uint		magic ;
} ;

typedef MAPSTRS		mapstrs ;

EXTERNC_begin

extern int mapstrs_start	(mapstrs *,int) noex ;
extern int mapstrs_add		(mapstrs *,cchar *,int,cchar *,int) noex ;
extern int mapstrs_delkey	(mapstrs *,cchar *,int) noex ;
extern int mapstrs_present	(mapstrs *,cchar *,int,cchar **) noex ;
extern int mapstrs_count	(mapstrs *) noex ;
extern int mapstrs_finish	(mapstrs *) noex ;

EXTERNC_end


#endif /* MAPSTRS_INCLUDE */


