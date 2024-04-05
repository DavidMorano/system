/* strlistmk HEADER */
/* lang=C20 */

/* STRLISTMK management */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRLISTMK_INCLUDE
#define	STRLISTMK_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* systems IDs */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<modload.h>
#include	<localmisc.h>

#include	"strlistmks.h"


#define	STRLISTMK	struct strlistmk_head
#define	STRLISTMK_CALLS	struct strlistmk_calls
#define	STRLISTMK_MAGIC	0x99447256


struct strlistmk_calls {
	int	(*open)(void *,cchar *,cchar *,int,mode_t,int) ;
	int	(*chgrp)(void *,gid_t) ;
	int	(*add)(void *,cchar *,int) ;
	int	(*abort)(void *) ;
	int	(*close)(void *) ;
} ;

struct strlistmk_head {
	modload		loader ;
	void		*sop ;		/* shared-object (SO) pointer */
	void		*obj ;		/* object pointer */
	STRLISTMK_CALLS call ;
	uint		magic ;
	int		objsize ;	/* object size */
} ;

EXTERNC_begin

extern int strlistmk_open(STRLISTMK *,cc *,cc *,int,mode_t,int) noex ;
extern int strlistmk_chgrp(STRLISTMK *,gid_t) noex ;
extern int strlistmk_add(STRLISTMK *,cchar *,int) noex ;
extern int strlistmk_abort(STRLISTMK *) noex ;
extern int strlistmk_close(STRLISTMK *) noex ;

EXTERNC_end


#endif /* STRLISTMK_INCLUDE */


