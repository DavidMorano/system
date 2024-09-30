/* mkuuid HEADER */
/* lang=C20 */

/* make UUID (also a specialized object) */
/* version %I% last-modified %G% */


/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	MKUUID_INCLUDE
#define	MKUUID_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	MKUUID			struct mkuuid_head


struct mkuuid_head {
	uint64_t	time ;		/* 60-bits */
	uint64_t	node ;		/* 48-bits */
	uint16_t	clk ;		/* 14 (or 13�) bits */
	uint		version:4 ;	/* 4-bits */
} ;

/* Note �: Micro$oft used 13 bits in the past (we always use 14 bits) */

EXTERNC_begin

extern int mkuuid(MKUUID *,int) noex ;

EXTERNC_end


#endif /* MKUUID_INCLUDE */


