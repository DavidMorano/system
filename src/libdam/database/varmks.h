/* varmks HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	VARMKS_INCLUDE
#define	VARMKS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<strtab.h>


#define	VARMKS		struct varmks_head
#define	VARMKS_FL	struct varmks_flags
#define	VARMKS_OBJ	struct varmks_object
#define	VARMKS_REC	struct varmks_rectab
#define	VARMKS_MAGIC	0x88773422
#define	VARMKS_NENTRIES	(2 * 1024)
#define	VARMKS_INTOPEN	(10*60)
#define	VARMKS_INTSTALE	(5*60)


struct varmks_object {
	char		*name ;
	uint		objsize ;
} ;

struct varmks_flags {
	uint		viopen:1 ;
	uint		abort:1 ;
	uint		ofcreat:1 ;
	uint		ofexcl:1 ;
	uint		inprogress:1 ;
	uint		created:1 ;
} ;

struct varmks_rectab {
	uint		(*rectab)[2] ;
	int		i ;			/* highest index */
	int		n ;			/* extent */
} ;

struct varmks_head {
	cchar 		*dbname ;
	cchar		*idname ;
	char		*nidxfname ;
	strtab		keys ;
	strtab		vals ;
	VARMKS_REC	rectab ;
	gid_t		gid ;
	VARMKS_FL	fl ;
	uint		magic ;
	int		nvars ;
	int		nfd ;
	mode_t		om ;
} ;

typedef	VARMKS		varmks ;
typedef	VARMKS_FL	varmks_fl ;
typedef	VARMKS_OBJ	varmks_obj ;
typedef	VARMKS_REC	varmks_rec ;

EXTERNC_begin

extern int	varmks_open(varmks *,cchar *,int,mode_t,int) noex ;
extern int	varmks_addvar(varmks *,cchar *,cchar *,int) noex ;
extern int	varmks_abort(varmks *) noex ;
extern int	varmks_chgrp(varmks *,gid_t) noex ;
extern int	varmks_close(varmks *) noex ;

EXTERNC_end


#endif /* VARMKS_INCLUDE */


