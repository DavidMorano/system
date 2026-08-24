/* ucopenisvc SUPPORT (open-facility-intercept) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* open a facility-intercept */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uc_openisvc

	Description:
	This subroutine opens what is referred to as a
	"facility-intercept."  Facility intercepts provide a way for
	opens on files to be intercepted by a facility manager
	specified by the intercept name.  Facilities are normally
	or usually really a software distribution that has its own
	source-root or program-root.  Examples of software distributions
	within AT&T are:

	NCMP
	PCS (Personal Communication Services)
	EXPTOOLS (Experimental Tools)
	LOCAL
	REXEC (Remote Execution)
	POST
	EXTRA
	AST (Advanced Software Technologies)
	GNU

	Facilities are stored in the filesystem hierarchy rooted
	at a certain place.  Facilities are usually rooted at one
	of the following locations in the system filesystem hierarchy:

	/usr
	/usr/add-on
	/opt

	Facility services are represented in the filesystem as files
	with names of the form:

	<facility>º<int>[­<arg(s)>

	These sorts of file names are often actually stored in the
	filesystem as symbolic links.

	Synopsis:
	int uc_openisvc(cc *pr,cc *dn,cc *bn,cc *prn,cc *svc,int of,
		mode_t om,mainv argv,mainv envv,int to) noex

	Arguments:
	pr		program-root
	dn		directory name (containing the file)
	dn		base name of the file
	prn		facility name
	svc		service name
	of		open-flags
	om		open-mode
	argv		array of arguments
	envv		attay of environment
	to		time-out

	Returns:
	>=0		file-descriptor
	<0		error (system-return)

	Notes:
	Facility intercepts are implemented with loadable shared-object
	files.  Each service has a file of the same name as the
	intercept name itself.  The file is a shared-object with a
	global symbol of a callable subroutine with the name
	'openint_<int>' where <int> is the intercept name.  The
	subroutine looks like:

	int openint_<int>(cc *pr,cc *dn,cc *bn,cc *prn,int of,mode_t om,
		mainv argv,mainv envv,int to) noex

	Notes:
	Multiple intercepts can be actually implemented in the same
	shared-object.  But the actual file of that object should
	be linked to other files, each with the filename of an
	intercept to be implemented.  These links are required
	because this code only searches for intercepts by searching
	for files with the names of the intercepts.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<dlfcn.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ids.h>			/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<mkx.h>			/* LIBUC |mksofname(3uc)| */
#include	<mkpathx.h>		/* LIBUC */
#include	<permx.h>		/* LIBUC */
#include	<isoneof.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

import openxsvc ;

/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_openisvc(cc *pr,cc *prn,cc *svc,int of,mode_t om,
		mainv argv,mainv envv,int to) noex {
    	openxsvc	os(pr,prn,svc,of,om,argv,envv,to) ;
	os.prefix = "openint" ;
	return uc_openxsvc(&os) ;
} /* end subroutine (uc_openisvc) */


