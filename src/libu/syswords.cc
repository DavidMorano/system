/* syswords SUPPORT */
/* charset=ISO8859-1 */
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


cchar syswords::w_export[]		= "export" ;
cchar syswords::w_path[]		= "path" ;
cchar syswords::w_fpath[]		= "fpath" ;
cchar syswords::w_cdpath[]		= "cdpath" ;
cchar syswords::w_libpath[]	= "libpath" ;
cchar syswords::w_manpath[]	= "manpath" ;
cchar syswords::w_incpath[]	= "incpath" ;
cchar syswords::w_infopath[]	= "infopath" ;

cchar syswords::w_etc[]		= "etc" ;
cchar syswords::w_lib[]		= "lib" ;
cchar syswords::w_man[]		= "man" ;
cchar syswords::w_var[]		= "var" ;
cchar syswords::w_share[]		= "share" ;
cchar syswords::w_info[]		= "info" ;
cchar syswords::w_help[]		= "help" ;
cchar syswords::w_users[]		= "users" ;

cchar syswords::w_bindir[]		= "/bin" ;
cchar syswords::w_sbindir[]	= "/sbin" ;
cchar syswords::w_usrdir[]		= "/usr" ;
cchar syswords::w_etcdir[]		= "/etc" ;
cchar syswords::w_tmpdir[]		= "/tmp" ;
cchar syswords::w_devdir[]		= "/dev" ;
cchar syswords::w_vardir[]		= "/var" ;
cchar syswords::w_procdir[]	= "/proc" ;
cchar syswords::w_sysdbdir[]	= "/sysdb" ;

cchar syswords::w_devstdin[] 	= "/dev/stdin" ;
cchar syswords::w_devstdout[]	= "/dev/stdout" ;
cchar syswords::w_devstderr[]	= "/dev/stderr" ;
cchar syswords::w_devstdlog[]	= "/dev/stdlog" ;
cchar syswords::w_devnull[]	= "/dev/null" ;
cchar syswords::w_devzero[]	= "/dev/zero" ;
cchar syswords::w_devrandom[]	= "/dev/urandom" ;
cchar syswords::w_maildir[]	= "/var/mail" ;
cchar syswords::w_vartmpdir[]	= "/var/tmp" ;
cchar syswords::w_usrlocaldir[]	= "/usr/local" ;
cchar syswords::w_usrextradir[]	= "/usr/extra" ;

cchar syswords::w_blanks[]		= "        " ;
cchar syswords::w_defprovider[]	= "Rightcore Network Services" ;
cchar syswords::w_localhost[]	= "localhost" ;
cchar syswords::w_mailhost[]	= "mailhost" ;
cchar syswords::w_loghost[]	= "loghost" ;


