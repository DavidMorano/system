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
#include	<time.h>		/* |time_t| */
#include	<usystem.h>
#include	<bfile.h>
#include	<linefilter.h>
#include	<linecleanopt.h>	/* line-cleanint options */


/* objects */
#define	FILEWATCH		struct filewatch_head
#define	FILEWATCH_FL		struct filewatch_flags
#define	FILEWATCH_AR		struct filewatch_args
#define	FILEWATCH_MAGIC		0x97868573

enum filewatchos {
    	filewatcho_carriage,
    	filewatcho_clean,
    	filewatcho_overlast
} ; /* end enum (filewatchos) */

struct filewatchms {
    	static cint	carriage ;
    	static cint	clean ;
} ; /* end struct (filewatchms) */

/* options */
#define	FILEWATCH_MCARRIAGE	(1 << filewatcho_carriage)
#define	FILEWATCH_MCLEAN	(1 << filewatcho_clean)

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
	char		*bufp ;
	linefilter	*lfp ;
	bfile		*wfp ;
	time_t		lastcheck ;
	time_t		lastchange ;
	time_t		opentime ;
	off_t		offset ;
	ino_t		ino ;
	dev_t		dev ;
	FILEWATCH_FL	fl ;
	uint		magic ;
	int		bufl ;
	int		interval ;
	int		cut ;
	int		columns ;
	int		indent ;
	int		opts ;
	int		bi ;
	int		ll ;
} ; /* end struct (filewatch_head) */

typedef	FILEWATCH_FL		filewatch_fl ;
typedef	FILEWATCH_AR		filewatch_ar ;

#ifdef	__cplusplus
enum filewatchmems {
	filewatchmem_finish,
	filewatchmem_overlast
} ;
struct filewatch ;
struct filewatch_co {
	filewatch	*op = nullptr ;
	int		w = -1 ;
	void operator () (filewatch *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (filewatch_co) */
struct filewatch : filewatch_head {
	filewatch_co	finish ;
	filewatch() noex {
	    finish	(this,filewatchmem_finish) ;
	    magic = 0 ;
	} ; /* end ctor */
	int start(filewatch_ar *,linefilter *,cchar *) noex ;
	int check(time_t,bfile *) noex ;
	int readln(time_t,char *,int) noex ;
	void dtor() noex ;
	destruct filewatch() {
	    if (magic) dtor() ;
	} ;
} ; /* end struct (filewatch) */
#else
typedef	FILEWATCH	filewatch ;
#endif /* __cplusplus */

EXTERNC_begin

extern int filewatch_start(filewatch *,filewatch_ar *,linefilter *,
		cchar *) noex ;
extern int filewatch_check(filewatch *,time_t,bfile *) noex ;
extern int filewatch_readln(filewatch *,time_t,char *,int) noex ;
extern int filewatch_finish(filewatch *) noex ;

EXTERNC_end


#ifdef	__cplusplus

extern const filewatchms	filewatchm ;

#endif /* __cplusplus */


#endif /* FILEWATCH_INCLUDE */



