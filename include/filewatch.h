/* filewatch HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* watch a file for changes and report */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	FILEWATCH_INCLUDE
#define	FILEWATCH_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system types */
#include	<usystem.h>
#include	<bfile.h>
#include	<linefilter.h>
#include	<localmisc.h>


/* object defines */
#define	FILEWATCH		struct filewatch_head
#define	FILEWATCH_FL		struct filewatch_flags
#define	FILEWATCH_AR		struct filewatch_args
/* options */
#define	FILEWATCH_MCARRIAGE	0x0001
#define	FILEWATCH_MCLEAN	0x0002


struct filewatch_flags {
	uint		open:1 ;
	uint		carriage:1 ;
	uint		clean:1 ;
} ;

struct filewatch_args {
	int		interval ;
	int		cut ;
	int		columns ;
	int		indent ;
	int		opts ;
} ;

struct filewatch_head {
	cchar		*fname ;
	char		*lp ;
	char		*buf ;
	STRFILTER	*sfp ;
	FILEWATCH_FL	fl ;
	bfile		wfile ;
	time_t		lastcheck ;
	time_t		lastchange ;
	time_t		opentime ;
	ino_t		ino ;
	dev_t		dev ;
	uint		offset ;
	int		interval ;
	int		cut ;
	int		columns ;
	int		indent ;
	int		opts ;
	int		bi ;
	int		ll ;
} ;

typedef	FILEWATCH		filewatch ;
typedef	FILEWATCH_FL		filewatch_fl ;
typedef	FILEWATCH_AR		filewatch_ar ;

EXTERNC_begin

extern int filewatch_start(filewatch *,filewatch_ar *,linefilter *,
		cchar *) noex ;
extern int filewatch_check(filewatch *,time_t,bfile *) noex ;
extern int filewatch_readline(filewatch *,time_t,char *,int) noex ;
extern int filewatch_finish(filewatch *) noex ;

EXTERNC_end


#endif /* FILEWATCH_INCLUDE */



