/* strpack HEADER */
/* lang=C20 */

/* string-pack */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRPACK_INCLUDE
#define	STRPACK_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<vechand.h>
#include	<localmisc.h>


#define	STRPACK_MAGIC		0x42114683
#define	STRPACK			struct strpack_head
#define	STRPACK_CHUNK		struct strpack_chunk


struct strpack_chunk {
	char		*cdata ;
	int		csz ;		/* allocated buffer length */
	int		i ;		/* index length */
	int		c ;		/* item count within chunk */
} ;

struct strpack_head {
	STRPACK_CHUNK	*ccp ;		/* current chunk pointer */
	vechand		*clp ;		/* chunk-list-pointer */
	uint		magic ;
	int		chsize ;
	int		totalsize ;
	int		c ;		/* total count */
} ;

typedef struct strpack_head	strpack ;
typedef struct strpack_chunk	strpack_ch ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int	strpack_start(STRPACK *,int) noex ;
extern int	strpack_store(STRPACK *,cchar *,int,cchar **) noex ;
extern int	strpack_count(STRPACK *) noex ;
extern int	strpack_size(STRPACK *) noex ;
extern int	strpack_finish(STRPACK *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* STRPACK_INCLUDE */


