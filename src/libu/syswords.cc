/* syswords SUPPORT */
/* lang=C++20 */

/* this is a database of commonly used system words */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	syswords

	Description:
	This object contains various commonly used system-related
	words.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>

#include	"syswords.hh"


const char syswords::w_export[] =	"export" ;
const char syswords::w_path[] =		"path" ;
const char syswords::w_fpath[] =	"fpath" ;
const char syswords::w_cdpath[] =	"cdpath" ;
const char syswords::w_libpath[] =	"libpath" ;
const char syswords::w_manpath[] =	"manpath" ;
const char syswords::w_incpath[] =	"incpath" ;
const char syswords::w_infopath[] =	"infopath" ;

const char syswords::w_users[] =	"users" ;
const char syswords::w_help[] =		"help" ;
const char syswords::w_lib[] =		"lib" ;

const char syswords::w_etcdir[] =	"/etc" ;
const char syswords::w_tmpdir[] =	"/tmp" ;
const char syswords::w_devdir[] =	"/dev" ;
const char syswords::w_vardir[] =	"/var" ;
const char syswords::w_procdir[] =	"/proc" ;
const char syswords::w_sysdbdir[] =	"/sysdb" ;
const char syswords::w_maildir[] =	"/var/mail" ;
const char syswords::w_usrlocaldir[] =	"/usr/local" ;

const char syswords::w_digtab[] = "0123456789ABCDEFGHIJKLMNOPQRSTUV"
		"abcdefghijklmnopqrstuvwxyz" "Øø" ;
const char syswords::w_blanks[] = "        " ;


