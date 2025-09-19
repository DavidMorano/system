/* cksum HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* perform the POSIX 1003.2 CRC checksum algorithm */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-17, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CKSUM_INCLUDE
#define	CKSUM_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	CKSUM		struct cksum_head
#define	CKSUM_FL	struct cksum_flags
#define	CKSUM_ST	struct cksum_state


struct cksum_flags {
	uint		loc:1 ;
} ;

struct cksum_state {
	uint		len, sum ;
} ;

struct cksum_head {
	CKSUM_ST	loc, total ;
	CKSUM_FL	fl ;
} ;

typedef	CKSUM		cksum ;
typedef	CKSUM_FL	cksum_fl ;
typedef	CKSUM_ST	cksum_st ;

EXTERNC_begin

extern int cksum_start(cksum *) noex ;
extern int cksum_begin(cksum *) noex ;
extern int cksum_accum(cksum *,cvoid *,int) noex ;
extern int cksum_end(cksum *) noex ;
extern int cksum_getsum(cksum *,uint *) noex ;
extern int cksum_getsumall(cksum *,uint *) noex ;
extern int cksum_finish(cksum *) noex ;

EXTERNC_end


#endif /* CKSUM_INCLUDE */


