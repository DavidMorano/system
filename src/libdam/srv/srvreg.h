/* srvreg HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SRVREG_INCLUDE
#define	SRVREG_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<netdb.h>
#include	<time.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>

#include	<srvrege.h>


/* object defines */

#define	SRVREG_MAGIC		1092837456
#define	SRVREG			struct srvreg_head
#define	SRVREG_FL		struct srvreg_flags
#define	SRVREG_FILE		struct srvreg_filehead
#define	SRVREG_BUF		struct srvreg_buffer
#define	SRVREG_CUR		struct srvreg_cursor
#define	SRVREG_ENT		SRVREGE_ALL

#define	SRVREG_FILEPATH		"/tmp/srvreg"

#define	SRVREG_FILEMAGIC	"SRVREG"
#define	SRVREG_FILEMAGICLEN	lenstr(SRVREG_FILEMAGIC)
#define	SRVREG_FILEVERSION	0
#define	SRVREG_ENDIAN		1

#define	SRVREG_TAGLEN		SRVREGE_LTAG
#define	SRVREG_SVCLEN		SRVREGE_LSVC
#define	SRVREG_SSLEN		SRVREGE_LSS
#define	SRVREG_HOSTLEN		SRVREGE_LHOST


/* file buffer state */
struct srvreg_buffer {
	char		*buf ;		/* fixed buffer */
	uint		size ;		/* fixed buffer size */
	uint		len ;		/* length of valid area */
	uint		off ;		/* file offset of valid area */
} ;

/* decoded file magic */
struct srvreg_filemagic {
	char		magic[16] ;
	uchar		vetu[4] ;
} ;

/* decoded file header values */
struct srvreg_filehead {
	uint		wcount ;
	uint		wtime ;
	uint		nentries ;
} ;

struct srvreg_flags {
	uint		fileinit:1 ;		/* file init'ed */
	uint		writable:1 ;
	uint		readlocked:1 ;
	uint		writelocked:1 ;
	uint		cursorlockbroken:1 ;	/* cursor lock broken */
	uint		cursoracc:1 ;		/* accessed while cursored? */
	uint		remote:1 ;		/* remote mounted file */
} ;

struct srvreg_head {
	cchar		*fname ;
	time_t		opentime ;		/* file open time */
	time_t		accesstime ;		/* file access time */
	time_t		mtime ;			/* file modification time */
	SRVREG_FILE	h ;
	SVCREG_BUF	b ;	/* file buffer */
	SRVREG_FL	fl ;
	uint		magic ;
	int		oflags ;
	int		operm ;
	int		pagesize ;
	int		filesize ;
	int		fd ;
	int		cursors ;
} ;

struct srvreg_cursor {
	int		i ;
} ;

typedef	SRVREG			srvreg ;
typedef	SRVREG_FL		srvreg_fl ;
typedef	SRVREG_FILE		srvreg_file ;
typedef	SRVREG_BUF		srvreg_buf ;
typedef	SRVREG_CUR		srvreg_cur ;
typedef	SRVREG_ENT		srvreg_ent ;

EXTERNC_begin

extern int srvreg_open(srvreg *,cchar *,int,int) noex ;
extern int srvreg_check(srvreg *,time_t) noex ;
extern int srvreg_close(srvreg *) noex ;
extern int srvreg_curbegin(srvreg *,srvreg_cur *) noex ;
extern int srvreg_curend(srvreg *,srvreg_cur *) noex ;
extern int srvreg_enum(srvreg *,srvreg_cur *,srvreg_ent *) noex ;
extern int srvreg_fetchsvc(srvreg *,cchar *,srvreg_cur *,srvreg_ent *) noex ;
extern int srvreg_write(srvreg *,int,srvreg_ent *) noex ;

EXTERNC_end


#endif /* SRVREG_INCLUDE */


