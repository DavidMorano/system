/* aclinfo SUPPORT */
/* lang=C++20 */

/* ACL information */
/* version %I% last-modified %G% */


/* revision history:

	= 2005-02-24, David A­D­ Morano
	This code was adopted from the SHCAT program, which in turn
	had been adopted from programs with a lineage dating back
	(from the previous notes in this space) from 1989! I deleted
	the long list of notes here to clean this up.

*/

/* Copyright © 2005 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	These subroutines perform (various) ACL handling functions.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/acl.h>
#include	<unistd.h>
#include	<climits>
#include	<usystem.h>
#include	<localmisc.h>

#include	"aclinfo.h"


/* local defines */

#ifdef	MAX_ACL_ENTRIES
#define	MAXACLS		MAX_ACL_ENTRIES
#else
#define	MAXACLS		1024
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr uid_t		uidend = uid_t(-1) ;
constexpr gid_t		gidend = gid_t(-1) ;


/* exported variables */


/* exported subroutines */

int aclinfo_mksol(aclinfo *ap) noex {
	int		soltype = -1 ;
	switch (ap->type) {
	case acltype_user:
	    soltype = (ap->uid != uidend) ? USER : USER_OBJ ;
	    break ;
	case acltype_group:
	    soltype = (ap->gid != gidend) ? GROUP : GROUP_OBJ ;
	    break ;
	case acltype_other:
	    soltype = OTHER_OBJ ;
	    break ;
	case acltype_mask:
	    soltype = CLASS_OBJ ;
	    break ;
	case acltype_defuser:
	    soltype = (ap->uid != uid_end) ? DEF_USER : DEF_USER_OBJ ;
	    break ;
	case acltype_defgroup:
	    soltype = (ap->gid != gidend) ? DEF_GROUP : DEF_GROUP_OBJ ;
	    break ;
	case acltype_defother:
	    soltype = DEF_OTHER_OBJ ;
	    break ;
	case acltype_defmask:
	    soltype = DEF_CLASS_OBJ ;
	    break ;
	} /* end switch */
	ap->soltype = soltype ;
	return soltype ;
}
/* end subroutine (aclinfo_mksol) */

int aclinfo_isdeftype(aclinfo *ap) noex {
	int		f = false ;
	switch (ap->type) {
	case acltype_defuser:
	case acltype_defgroup:
	case acltype_defother:
	case acltype_defmask:
	    f = true ;
	    break ;
	} /* end switch */
	return f ;
}
/* end subroutine (aclinfo_isdeftype) */

int aclinfo_isidtype(aclinfo *ap) noex {
	int		f = false ;
	switch (ap->type) {
	case acltype_user:
	case acltype_defuser:
	    f = (ap->uid != uidend) ;
	    break ;
	case acltype_group:
	case acltype_defgroup:
	    f = (ap->gid != gidend) ;
	    break ;
	} /* end switch */
	return f ;
}
/* end subroutine (aclinfo_isidtype) */


