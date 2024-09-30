/* rmermsg HEADER */
/* lang=C20 */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	RMERMSG_INCLUDE
#define	RMERMSG_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


struct rmermsg_fname {
	uint	msglen ;
	uint	tag ;
	uint	delay ;
	uid_t	uid ;
	uchar	msgtype ;		/* message type */
	char	fname[MAXPATHLEN + 1] ;
} ;

/* unknown */
struct rmermsg_unknown {
	uint	msglen ;
	uchar	msgtype ;		/* message type */
} ;


/* message types */
enum rmermsgtypes {
	rmermsgtype_fname,
	rmermsgtype_unknown,
	rmermsgtype_overlast
} ;

EXTERNC_begin

extern int	rmermsg_fname(struct rmermsg_fname *,int,char *,int) noex ;
extern int	rmermsg_unknown(struct rmermsg_unknown *,int,char *,int) noex ;

EXTERNC_end


#endif /* RMERMSG_INCLUDE */


