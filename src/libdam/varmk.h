/* varmk HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* VARMK management */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	VARMK_INCLUDE
#define	VARMK_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<modload.h>
#include	<varmks.h>


#define	VARMK		struct varmk_head
#define	VARMK_CALLS	struct varmk_callsubs
#define	VARMK_MAGIC	0x99447246


EXTERNC_begin
    struct varmk_callsubs {
	int	(*open)(void *,cchar *,int,mode_t,int) ;
	int	(*chgrp)(void *,gid_t) ;
	int	(*addvar)(void *,cchar *,cchar *,int) ;
	int	(*abort)(void *) ;
	int	(*close)(void *) ;
    } ; /* end struct (varmk_callsubs) */
EXTERNC_end

struct varmk_head {
	modload		loader ;
	VARMK_CALLS	call ;
	void		*sop ;		/* shared-object (SO) pointer */
	void		*obj ;		/* object pointer */
	uint		magic ;
	int		objsize ;	/* object size */
} ;

typedef VARMK		varmk ;
typedef VARMK_CALLS	varmk_calls ;

EXTERNC_begin

extern int	varmk_open(varmk *,cchar *,int,mode_t,int) noex ;
extern int	varmk_chgrp(varmk *,gid_t) noex ;
extern int	varmk_addvar(varmk *,cchar *,cchar *,int) noex ;
extern int	varmk_abort(varmk *) noex ;
extern int	varmk_close(varmk *) noex ;

EXTERNC_end


#endif /* VARMK_INCLUDE */


