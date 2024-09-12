/* cksum */

/* perform the POSIX 1003.2 CRC checksum algorithm */


/* revision history:

	= 1998-03-17, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */


#ifndef	CKSUM_INCLUDE
#define	CKSUM_INCLUDE	1


#include	<envstandards.h>	/* MUST be first to configure */

#include	<sys/types.h>
#include	<sys/param.h>


/* object defines */

#define	CKSUM		struct cksum_head
#define	CKSUM_FL	struct cksum_flags
#define	CKSUM_ST	struct cksum_state


struct cksum_flags {
	uint		local:1 ;
} ;

struct cksum_state {
	uint		len, sum ;
} ;

struct cksum_head {
	CKSUM_FL	f ;
	CKSUM_ST	local, total ;
} ;

typedef	CKSUM		cksum ;

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


