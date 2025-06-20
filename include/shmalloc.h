/* SHMALLOC (Shared-Memory Allocator) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Shared-Memory-Allocation management */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SHMALLOC_INCLUDE
#define	SHMALLOC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	SHMALLOC		struct shmalloc_head
#define	SHMALLOC_BLK		struct shmalloc_block
#define	SHMALLOC_ALIGNSIZE	(2 * szof(int))
#define	SHMALLOC_BSZ		(4 * SHMALLOC_ALIGNSIZE)


struct shmalloc_block {
	int		bsz ;		/* block size */
	int		next ;		/* offset to next block */
} ;

struct shmalloc_head {
	int		str ;		/* string-area offset */
	int		used ;
	SHMALLOC_BLK	b ;
} ;

typedef	SHMALLOC	shmalloc ;
typedef	SHMALLOC_BLK	shmalloc_blk ;

EXTERNC_begin

extern int shmalloc_init(shmalloc *,char *,int) noex ;
extern int shmalloc_fini(shmalloc *) noex ;
extern int shmalloc_alloc(shmalloc *,int) noex ;
extern int shmalloc_free(shmalloc *,int) noex ;
extern int shmalloc_avail(shmalloc *) noex ;
extern int shmalloc_audit(shmalloc *) noex ;
extern int shmalloc_used(shmalloc *) noex ;
extern int shmalloc_already(shmalloc *,int) noex ;

EXTERNC_end


#endif /* SHMALLOC_INCLUDE */


