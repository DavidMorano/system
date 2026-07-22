/* mkuuid HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* make UUID (also a specialized object) */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MKUUID_INCLUDE
#define	MKUUID_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	UUID_VERSION	4		/* default UUID version */
#define	UUID_DAT	struct uuid_data


struct uuid_data {
	uint64		time ;		/* 60-bits */
	uint64		node ;		/* 48-bits */
	uint64		clk ;		/* 14 (or 13¹) bits */
	uint8		version ;	/* 4-bits */
} ; /* end struct (uuid_data) */

/* Note ¹: Micro$oft used 13 bits in the past (we always use 14 bits) */

typedef UUID_DAT	uuid_dat ;

EXTERNC_begin

extern int mkuuid(uuid_dat *,int) noex ;

EXTERNC_end

#ifdef	__cplusplus

local inline int mkuuid(uuid_dat *udp) noex {
    	return mkuuid(udp,0) ;
}

#endif /* __cplusplus */


#endif /* MKUUID_INCLUDE */


