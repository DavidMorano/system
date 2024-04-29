/* aclinfo HEADER */
/* lang=C20 */

/* ACL information */
/* version %I% last-modified %G% */


/* Copyright © 2005 David A­D­ Morano.  All rights reserved. */

#ifndef	ACLINFO_INCLUDE
#define	ACLINFO_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/acl.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<clanguage.h>
#include	<localmisc.h>

#include	"acltypes.h"


#define	ACLINFO		struct aclinfo


struct aclinfo {
	uid_t		uid ;
	gid_t		gid ;
	int		type ;
	int		soltype ;
	int		op ;		/* add or subtract */
	int		perm ;		/* permission bits */
} ;

typedef ACLINFO		aclinfo_t ;


EXTERNC_begin

extern int	aclinfo_mksol(ACLINFO *) noex ;
extern int	aclinfo_isdeftype(ACLINFO *) noex ;
extern int	aclinfo_isidtype(ACLINFO *) noex ;

EXTERNC_end


#endif /* ACLINFO_INCLUDE */


