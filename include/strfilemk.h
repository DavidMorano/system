/* strfilemk HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* STRFILEMK management */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRFILEMK_INCLUDE
#define	STRFILEMK_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* system types */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>

#include	"strfilemks.h"


#define	STRFILEMK	struct strfilemk_head
#define	STRFILEMK_CALLS	struct strfilemk_calls
#define	STRFILEMK_MAGIC	0x99447256


struct strfilemk_calls {
	int	(*open)(void *,cchar *,int,mode_t,int) ;
	int	(*chgrp)(void *,gid_t) ;
	int	(*addfile)(void *,cchar *,int) ;
	int	(*abort)(void *) ;
	int	(*close)(void *) ;
} ;

struct strfilemk_head {
	uint		magic ;
	MODLOAD		loader ;
	STRFILEMK_CALLS call ;
	void		*sop ;		/* shared-object (SO) pointer */
	void		*obj ;		/* object pointer */
	int		objsize ;	/* object size */
} ;

typedef	STRFILEMK	strfilemk ;
typedef	STRFILEMK_CALLS	strfilemk_ca ;

EXTERNC_begin

extern int	strfilemk_open(STRFILEMK *,cchar *,int,mode_t,int) noex ;
extern int	strfilemk_chgrp(STRFILEMK *,gid_t) noex ;
extern int	strfilemk_addfile(STRFILEMK *,cchar *,int) noex ;
extern int	strfilemk_abort(STRFILEMK *) noex ;
extern int	strfilemk_close(STRFILEMK *) noex ;

EXTERNC_end


#endif /* STRFILEMK_INCLUDE */


