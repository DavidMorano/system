/* usys_snx SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the file-name (file-path) that was used to 'exec' this program */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-16, David A­D­ Morano
	This subroutine was written to get some commonality across
	our most used operating systems. 

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	execname

	Description:
	This subroutine returns the name (file-path) by which the
	current program was 'exec'ed.

	Synopsis:
	cchar *execname() noex

	Returns:
	-		the exec-name pointer or NULL (if not found)

	Notes:
	1. This subroutine (the subroutine that needs emulation by
	some operating systems, which will remain nameless like
	Apple-Darwin®) is already native to Solaris® (SunOS), and its
	declaration on that plarform is accessed though the include
	file |cstdlib|.
	2. This subroutine is Thread-Safe but only Fork-Semi-Safe.
	That is: it is as fork-safe as any general C++ piece of
	code.  The C++ language itself only creates code (in the
	strictest sense) that is only Fork-Semi-Safe.  Actually,
	there are many pieces of code throughout everywhere (when
	written in C++) that are only Fork-Semi-Safe (if you did
	not already know).  The reason is due to the fact that
	executing over statically declared variables in block scape
	that are dynamically initialized by subroutine calls is not
	strictly Fork-Safe in the C++ language.  Of course, the C
	language does not even allow for that, so it is not an issue
	in pure C-language environments.  Executing over statically
	declared variables in namespace (file) scope initialized
	dynamically might also be fork-unsafe in some weirdo
	circumstantes, but that is usually very unlikely since the
	initialization happens at process load time when there is
	not likely to be multiple threads executing at the same
	time and one of them executing over the static variable
	initialization while a fork happens in another thread,
	Further the unlikiness is helped because the loadable module
	image got laoded by the dynamic linker, and then a fork
	happening at the very same time elsewhere is very quite
	unlikely.  This is probably why the ISO C++ Committee did
	not really care about strict fork-safety in dynamically
	initialized static variables.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* |getprogname(3c)| |execname(3c) */
#include	<cstring>		/* CSTD |strncpy(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<ulogerror.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"usys_snx.h"


namespace usys {
} /* end namespace (usys) */


