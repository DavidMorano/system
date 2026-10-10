/* aclinfo HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Access-Control-List (ACL) information */
/* version %I% last-modified %G% */


/* Copyright © 2005 David A­D­ Morano.  All rights reserved. */

#ifndef	ACLINFO_INCLUDE
#define	ACLINFO_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system types */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */

#include	"acltypes.h"


#define	ACLINFO		struct aclinfo_head


struct aclinfo_head {
	uid_t		uid ;
	gid_t		gid ;
	int		type ;
	int		soltype ;
	int		op ;		/* add or subtract */
	int		perm ;		/* permission bits */
} ; /* end struct */

typedef ACLINFO		aclinfo ;

EXTERNC_begin

extern int	aclinfo_mksol(aclinfo *) noex ;
extern int	aclinfo_isdeftype(aclinfo *) noex ;
extern int	aclinfo_isidtype(aclinfo *) noex ;

EXTERNC_end


#endif /* ACLINFO_INCLUDE */


