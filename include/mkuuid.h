/* mkuuid HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* make UUID (also a specialized object) */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MKUUID_INCLUDE
#define	MKUUID_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<stdint.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	UUID_VERSION	4		/* default UUID version */
#define	UUID_DAT	struct uuid_data


struct uuid_data {
	uint64_t	time ;		/* 60-bits */
	uint64_t	node ;		/* 48-bits */
	uint16_t	clk ;		/* 14 (or 13¹) bits */
	uint		version:4 ;	/* 4-bits */
} ;

/* Note ¹: Micro$oft used 13 bits in the past (we always use 14 bits) */

typedef UUID_DAT	uuid_dat ;

EXTERNC_begin

extern int mkuuid(uuid_dat *,int) noex ;

EXTERNC_end

#ifdef	__cplusplus

static inline int mkuuid(uuid_dat *udp) noex {
    	return mkuuid(udp,0) ;
}

#endif /* __cplusplus */


#endif /* MKUUID_INCLUDE */


