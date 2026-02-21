/* strenv HEADER */
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
	strenv

	Description:
	Recommended usage within source code:
		#include	<strenv.hh>
		strenv	strpath(strenv_path) ;
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


enum strenvs {
	strenv_cdpath,
	strenv_domain,
	strenv_fpath,
	strenv_home,
	strenv_incpath,
	strenv_infopath,
	strenv_libpath,
	strenv_localdomain,
	strenv_logid,
	strenv_logline,
	strenv_logname,
	strenv_mail,
	strenv_maildir,
	strenv_manpath,
	strenv_node,
	strenv_organization,
	strenv_orgcode,
	strenv_orgloc,
	strenv_path,
	strenv_tmpdir,
	strenv_user,
	strenv_username,
	strenv_utmpid,
	strenv_utmpline,
	strenv_utmpname,
	strenv_overlast
} ; /* end enum (strenvs) */

class strenv {
	cchar		*strp = nullptr ;
	const strenvs	w ;
	int		facc = false ;
    public:
	strenv(strenvs aw) noex : w(aw) { } ;
	strenv() = delete ;
	strenv &operator = (const strenv &) = delete ;
	operator ccharp () noex ;
} ; /* end class (strenv) */


#endif	/* __cplusplus */
#endif /* STRENV_INCLUDE */


