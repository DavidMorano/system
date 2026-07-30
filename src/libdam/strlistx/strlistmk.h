/* strlistmk HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* STRLISTMK management */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRLISTMK_INCLUDE
#define	STRLISTMK_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® systems types */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<modload.h>		/* LIBUC */
#include	<strlistmks.h>		/* LIBUC */


#define	STRLISTMK		struct strlistmk_head
#define	STRLISTMK_FL		struct strlistmk_flags
#define	STRLISTMK_MAGIC		0x99447256
#define	STRLISTMK_MODBNAME	"strlistmks"
#define	STRLISTMK_OBJNAME	"strlistmks"


struct strlistmk_flags {
    	uint		modload:1 ;
} ; /* end struct */

struct strlistmk_head {
	modload		*mlp ;		/* module-load-pointer */
	void		*callp ;	/* calls-structure-pointer */
	void		*obj ;		/* object pointer */
	STRLISTMK_FL	fl ;
	uint		magval ;
	int		objsize ;	/* object size */
} ; /* end struct */

typedef	STRLISTMK	strlistmk ;
typedef	STRLISTMK_FL	strlistmk_fl ;

EXTERNC_begin

extern int strlistmk_open(strlistmk *,cc *,cc *,int,mode_t,int) noex ;
extern int strlistmk_chgrp(strlistmk *,gid_t) noex ;
extern int strlistmk_add(strlistmk *,cchar *,int) noex ;
extern int strlistmk_abort(strlistmk *) noex ;
extern int strlistmk_close(strlistmk *) noex ;

EXTERNC_end


#endif /* STRLISTMK_INCLUDE */


