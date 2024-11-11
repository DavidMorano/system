/* varnames SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* this is a database of commonly used environment variable names */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object contains various commonly used environment variable
	names.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>

#include	"varnames.hh"


const char varnames::logid[] =		"LOGID" ;
const char varnames::logname[] =	"LOGNAME" ;
const char varnames::logline[] =	"LOGLINE" ;
const char varnames::loghost[] =	"LOGHOST" ;
const char varnames::utmpid[] =		"UTMPID" ;
const char varnames::utmpname[] =	"UTMPNAME" ;
const char varnames::utmpline[] =	"UTMPLINE" ;
const char varnames::utmphost[] =	"UTMPHOST" ;
const char varnames::username[] =	"USERNAME" ;
const char varnames::groupname[] =	"GROUPNAME" ;
const char varnames::projname[] =	"PROJNAME" ;
const char varnames::user[] =		"USER" ;
const char varnames::group[] =		"GROUP" ;
const char varnames::cdpath[] =		"CDPATH" ;
const char varnames::path[] =		"PATH" ;
const char varnames::fpath[] =		"FPATH" ;
const char varnames::binpath[] =	"BINPATH" ;
const char varnames::libpath[] =	"LIBPATH" ;
const char varnames::incpath[] =	"INCPATH" ;
const char varnames::manpath[] =	"MANPATH" ;
const char varnames::manxpath[] =	"MANXPATH" ;
const char varnames::infopath[] =	"INFOPATH" ;
const char varnames::architecture[] =	"ARCHITECTURE" ;
const char varnames::sysname[] =	"SYSNAME" ;
const char varnames::release[] =	"RELEASE" ;
const char varnames::version[] =	"VERSION" ;
const char varnames::machine[] =	"MACHINE" ;
const char varnames::osname[] =		"OSNAME" ;
const char varnames::ostype[] =		"OSTYPE" ;
const char varnames::osrelease[] =	"OSRELEASE" ;
const char varnames::osnum[] =		"OSNUM" ;
const char varnames::osrel[] =		"OSREL" ;
const char varnames::osvers[] =		"OSVERS" ;
const char varnames::mail[] =		"MAIL" ;
const char varnames::node[] =		"NODE" ;
const char varnames::cluster[] =	"CLUSTER" ;
const char varnames::domain[] =		"DOMAIN" ;
const char varnames::localdomain[] =	"LOCALDOMAIN" ;	/* what is this? */
const char varnames::lang[] =		"LANG" ;
const char varnames::shell[] =		"SHELL" ;
const char varnames::shlvl[] =		"SHLVL" ;
const char varnames::home[] =		"HOME" ;
const char varnames::pwd[] =		"PWD" ;
const char varnames::tmpdir[] =		"TMPDIR" ;
const char varnames::maildir[] =	"MAILDIR" ;
const char varnames::maildirs[] =	"MAILDIRS" ;
const char varnames::uucppublic[] =	"UUCPPUBLIC" ;
const char varnames::lines[] =		"LINES" ;
const char varnames::columns[] =	"COLUMNS" ;
const char varnames::display[] =	"DISPLAY" ;
const char varnames::term[] =		"TERM" ;
const char varnames::termprogram[] =	"TERM_PROGRAM" ;
const char varnames::termdev[] =	"TERMDEV" ;
const char varnames::tz[] =		"TZ" ;
const char varnames::printer[] =	"PRINTER" ;
const char varnames::printerbin[] =	"PRINTERBIN" ;
const char varnames::pager[] =		"PAGER" ;
const char varnames::organization[] =	"ORGANIZATION" ;
const char varnames::orgloc[] =		"ORGLOC" ;
const char varnames::orgcode[] =	"ORGCODE" ;
const char varnames::office[] =		"OFFICE" ;
const char varnames::name[] =		"NAME" ;
const char varnames::fullname[] =	"FULLNAME" ;
const char varnames::mailname[] =	"MAILNAME" ;
const char varnames::tmout[] =		"TMOUT" ;
const char varnames::editor[] =		"EDITOR" ;
const char varnames::visual[] =		"VISUAL" ;
const char varnames::random[] =		"RANDOM" ;
const char varnames::hz[] =		"HZ" ;
const char varnames::ncpu[] =		"NCPU" ;
const char varnames::nisdomain[] =	"NISDOMAIN" ;


