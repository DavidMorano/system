/* streamsync */

/* data stream synchronization mechanism */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STREAMSYNC_INCLUDE
#define	STREAMSYNC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<localmisc.h>


#define	STREAMSYNC_MAGIC	0x96817463
#define	STREAMSYNC		struct streamsync_head


struct streamsync_head {
	char		*st ;		/* sequence to test against */
	char		*data ;		/* the data stream */
	uint		magic ;
	int		stlen ;		/* sequence length needed */
	int		i ;		/* current length */
} ;


#ifdef	__cplusplus
extern "C" {
#endif

extern int streamsync_start(STREAMSYNC *,cchar *,int) noex ;
extern int streamsync_test(STREAMSYNC *,int) noex ;
extern int streamsync_finish(STREAMSYNC *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* STREAMSYNC_INCLUDE */


