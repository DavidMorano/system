/* varmk HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* VARMK management */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	VARMK_INCLUDE
#define	VARMK_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<modload.h>		/* LIBDAM */
#include	<varmks.h>		/* LIBDAM */


#define	VARMK		struct varmk_head
#define	VARMK_FL	struct varmk_flags
#define	VARMK_MAGIC	0x99447246


struct varmk_flags {
    	uint		modload:1 ;
} ; /* end struct */

struct varmk_head {
	modload		*mlp ;
	void		*callp ;
	void		*obj ;		/* object pointer */
	VARMK_FL	fl ;
	uint		magval ;
	int		objsz ;		/* object size */
	int		cursz ;		/* cursor size */
} ; /* end struct */

typedef VARMK		varmk ;
typedef VARMK_FL	varmk_fl ;

EXTERNC_begin

extern int	varmk_open	(varmk *,cchar *,int,mode_t,int) noex ;
extern int	varmk_chgrp	(varmk *,gid_t) noex ;
extern int	varmk_addvar	(varmk *,cchar *,cchar *,int) noex ;
extern int	varmk_abort	(varmk *) noex ;
extern int	varmk_close	(varmk *) noex ;

EXTERNC_end


#endif /* VARMK_INCLUDE */


