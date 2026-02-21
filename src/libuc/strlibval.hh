/* strlibval HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* provide a pointer to a library string-value */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strlibval

	Description:
	Recommended usage within source code:
		#include	<strlibval.hh>
		strlibval	strpath(strlibval_path) ;
		if (strpath != nullptr) {
			* do something w/ string-value pointer 'strpath' *
		}

*******************************************************************************/

#ifndef	STRENV_INCLUDE
#define	STRENV_INCLUDE
#ifdef	__cplusplus /* everything is C++ only */


#include	<envstandards.h>	/* must be ordered fist to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


enum strlibvals {
	strlibval_cdpath,
	strlibval_domain,
	strlibval_fpath,
	strlibval_home,
	strlibval_incpath,
	strlibval_infopath,
	strlibval_libpath,
	strlibval_localdomain,
	strlibval_logid,
	strlibval_logline,
	strlibval_logname,
	strlibval_mail,
	strlibval_maildir,
	strlibval_manpath,
	strlibval_node,
	strlibval_organization,
	strlibval_orgcode,
	strlibval_orgloc,
	strlibval_path,
	strlibval_tmpdir,
	strlibval_user,
	strlibval_username,
	strlibval_utmpid,
	strlibval_utmpline,
	strlibval_utmpname,
	strlibval_overlast
} ; /* end enum (strlibvals) */

class strlibval {
	cchar		*strp = nullptr ;
	strlibvals	w ;
	int		facc = false ;
    public:
	strlibval(strlibvals aw) noex : w(aw) { } ;
	strlibval() = delete ;
	strlibval &operator = (const strlibval &) = delete ;
	operator ccharp () noex ;
} ; /* end class (strlibval) */


#endif	/* __cplusplus */
#endif /* STRENV_INCLUDE */


