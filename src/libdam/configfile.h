/* configfile HEADER (Configuration File) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* parse a configuration file */
/* version %I% last-modified %G% */


/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */


#ifndef	CONFIGFILE_INCLUDE
#define	CONFIGFILE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vecstr.h>		/* LIBUC */


#define	CONFIGFILE		struct configfile_head
#define	CONFIGFILE_FL		struct configfile_flags
#define	CONFIGFILE_MAGIC	0x04311633


struct configfile_flags {
    	uint		dlp:1 ;
    	uint		elp:1 ;
    	uint		ulp:1 ;
} ; /* end struct */

struct configfile_head {
	void		*dlp ;		/* list-pointer variables defined */
	void		*elp ;		/* list-pointer variables ENV set */
	void		*ulp ;		/* list-pointer variables ENV un-set */
	cchar	*root ;			/* program root */
	cchar	*tmpdir ;		/* environment variable */
	cchar	*logfname ;		/* log file name */
	cchar	*workdir ;		/* working directory */
	cchar	*directory ;		/* directory */
	cchar	*user ;			/* default username */
	cchar	*group ;		/* default groupname */
	cchar	*pidfname ;		/* traditionally hold PID */
	cchar	*lockfname ;		/* lock file */
	cchar	*interrupt ;
	cchar	*polltime ;
	cchar	*filetime ;
	cchar	*port ;			/* port to listen on */
	cchar	*userpass ;		/* user password file */
	cchar	*machpass ;		/* machine password file */
	cchar	*srvtab ;		/* SRVTAB */
	cchar	*sendmail ;		/* SENDMAIL program path */
	cchar	*envfname ;		/* environment file */
	cchar	*pathfname ;		/* PATH file */
	cchar	*devicefname ;		/* daemon device file path */
	cchar	*seedfname ;		/* seed file path */
	cchar	*logsize ;		/* default target log length */
	cchar	*organization ;
	cchar	*timeout ;
	cchar	*removemul ;		/* remove multiplier */
	cchar	*acctab ;		/* access table file */
	cchar	*paramfname ;		/* parameter file */
	cchar	*nrecips ;		/* number of recips at a time */
	cchar	*helpfname ;
	cchar	*statfname ;		/* status file name */
	cchar	*passfname ;		/* pass-FD file name */
	cchar	*eigenfname ;
	cchar	*options ;
	cchar	*interval ;		/* poll interval */
	cchar	*stampdir ;		/* timestamp directory */
	cchar	*maxjobs ;		/* maximum jobs */
	CONFIGFILE_FL	fl ;
	uint		magval ;	/* magic number */
	int	badline ;		/* line number of bad thing */
	int	srs ;			/* secondary return status */
	int	loglen ;		/* log file length */
	int	minwordlen ;
	int	maxwordlen ;
	int	keys ;
} ; /* end struct (configfile_head) */

typedef	CONFIGFILE	configfile ;

EXTERNC_begin

extern int configfile_start(configfile *,cchar *) noex ;
extern int configfile_finish(configfile *) noex ;

EXTERNC_end


#endif /* CONFIGFILE_INCLUDE */


