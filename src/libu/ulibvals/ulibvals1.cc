/* ulibvals1 MODULE (implementation) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* retrieve various Kernel-Library-Values */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	ulibvals

	Description:
	This module contains a structure (ULIBVALS) that itself
	contains various system related integer values.

	Synopsis:
	import ulibvals
	ulibval.{x}

	Returns:
	>=0		requested value
	<0		error (system-return)

*******************************************************************************/

module ;

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usysdefs.h>		/* LIBU |DIGBASE_MAX| */
#include	<localmisc.h>		/* LIBU |{xxx}BUFLEN| */

module ulibvals ;


ulibvaler_co ulibvaler::endianval	(ulibvalmem_endianval) ;
ulibvaler_co ulibvaler::pagesz		(ulibvalmem_pagesz) ;
ulibvaler_co ulibvaler::clktck		(ulibvalmem_clktck) ;
ulibvaler_co ulibvaler::maxbase		(ulibvalmem_maxbase) ;
ulibvaler_co ulibvaler::maxpid		(ulibvalmem_maxpid) ;
ulibvaler_co ulibvaler::maxarg		(ulibvalmem_maxarg) ;
ulibvaler_co ulibvaler::maxline		(ulibvalmem_maxline) ;
ulibvaler_co ulibvaler::maxlink		(ulibvalmem_maxlink) ;
ulibvaler_co ulibvaler::maxlogin	(ulibvalmem_maxlogin) ;
ulibvaler_co ulibvaler::maxsymloop	(ulibvalmem_maxsymloop) ;
ulibvaler_co ulibvaler::maxsymbol	(ulibvalmem_maxsymbol) ;
ulibvaler_co ulibvaler::maxgroups	(ulibvalmem_maxgroups) ;
ulibvaler_co ulibvaler::maxnamelen	(ulibvalmem_maxnamelen) ;
ulibvaler_co ulibvaler::maxpathlen	(ulibvalmem_maxpathlen) ;
ulibvaler_co ulibvaler::maxmsglen	(ulibvalmem_maxmsglen) ;
ulibvaler_co ulibvaler::maxsysuid	(ulibvalmem_maxsysuid) ;
ulibvaler_co ulibvaler::maxtzname	(ulibvalmem_maxtzname) ;
ulibvaler_co ulibvaler::maxtzabbr	(ulibvalmem_maxtzabbr) ;
ulibvaler_co ulibvaler::nodenamelen	(ulibvalmem_nodenamelen) ;
ulibvaler_co ulibvaler::usernamelen	(ulibvalmem_usernamelen) ;
ulibvaler_co ulibvaler::groupnamelen	(ulibvalmem_groupnamelen) ;
ulibvaler_co ulibvaler::projnamelen	(ulibvalmem_projnamelen) ;
ulibvaler_co ulibvaler::protnamelen	(ulibvalmem_protnamelen) ;
ulibvaler_co ulibvaler::hostnamelen	(ulibvalmem_hostnamelen) ;
ulibvaler_co ulibvaler::servnamelen	(ulibvalmem_servnamelen) ;
ulibvaler_co ulibvaler::binbuflen	(ulibvalmem_binbuflen) ;
ulibvaler_co ulibvaler::octbuflen	(ulibvalmem_octbuflen) ;
ulibvaler_co ulibvaler::decbuflen	(ulibvalmem_decbuflen) ;
ulibvaler_co ulibvaler::hexbuflen	(ulibvalmem_hexbuflen) ;
ulibvaler_co ulibvaler::digbuflen	(ulibvalmem_digbuflen) ;

ulibvaler		ulibval ;


