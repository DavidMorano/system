/* msgid HEADER */
/* lang=C20 */

/* manage message-id storage */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-12-10, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

#ifndef	MSGID_INCLUDE
#define	MSGID_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<localmisc.h>

#include	"msgide.h"


#define	MSGID_MAGIC		1092847456
#define	MSGID			struct msgid_head
#define	MSGID_KEY		struct msgid_k
#define	MSGID_CUR		struct msgid_c
#define	MSGID_BUF		struct msgid_buffer
#define	MSGID_FM		struct msgid_filemagic
#define	MSGID_FH		struct msgid_filehead
#define	MSGID_FL		struct msgid_flags
#define	MSGID_ENT		MSGIDE_ALL

/* other defines */

#define	MSGID_FILEPATH		"/tmp/msgid"

#define	MSGID_FILEVERSION	0
#define	MSGID_ENDIAN		1	/* big endian */


struct msgid_buffer {
	char		*buf ;		/* fixed buffer */
	uint		size ;		/* fixed buffer size */
	uint		off ;		/* file offset of valid area */
	uint		len ;		/* length of valid area */
} ;

/* decoded file magic */
struct msgid_filemagic {
	char		magic[16] ;
	uchar		vetu[4] ;
} ;

/* decoded file header values */
struct msgid_filehead {
	uint		wcount ;
	uint		wtime ;
	uint		nentries ;
} ;

struct msgid_flags {
	uint		fileinit:1 ;		/* file init'ed */
	uint		writable:1 ;
	uint		readlocked:1 ;
	uint		writelocked:1 ;
	uint		cursorlockbroken:1 ;	/* cursor lock broken */
	uint		cursoracc:1 ;		/* accessed while cursored? */
	uint		remote:1 ;		/* remote mounted file */
} ;

struct msgid_head {
	cchar		*fname ;
	MSGID_FL	f ;
	MSGID_FH	h ;
	MSGID_BUF	b ;		/* file buffer */
	time_t		opentime ;	/* file open time */
	time_t		accesstime ;	/* file access time */
	time_t		mtime ;		/* file modification time */
	mode_t		operm ;
	uint		magic ;
	int		oflags ;
	int		maxentry ;
	int		pagesize ;
	int		filesize ;
	int		fd ;
	int		cursors ;
	int		ebs ;		/* entry buffer size */
} ;

struct msgid_c {
	int		i ;
} ;

struct msgid_k {
	cchar		*recip ;
	cchar		*from ;
	cchar		*mid ;
	time_t		mtime ;
	int		reciplen ;
	int		midlen ;
} ;

typedef MSGID		msgid ;

EXTERNC_begin

extern int msgid_open(MSGID *,cchar *,int,mode_t,int) noex ;
extern int msgid_check(MSGID *,time_t) noex ;
extern int msgid_close(MSGID *) noex ;
extern int msgid_curbegin(MSGID *,MSGID_CUR *) noex ;
extern int msgid_curend(MSGID *,MSGID_CUR *) noex ;
extern int msgid_enum(MSGID *,MSGID_CUR *,MSGID_ENT *) noex ;
extern int msgid_match(MSGID *,time_t,MSGID_KEY *,MSGID_ENT *) noex ;
extern int msgid_update(MSGID *,time_t,MSGID_KEY *,MSGID_ENT *) noex ;
extern int msgid_matchid(MSGID *,time_t,cchar *,int,MSGID_ENT *) noex ;
extern int msgid_write(MSGID *,int,MSGID_ENT *) noex ;

#ifdef	COMMENT
extern int msgid_txbegin(MSGID *) noex ;
extern int msgid_txabort(MSGID *,int) noex ;
extern int msgid_txcommit(MSGID *,int) noex ;
extern int msgid_fetchsvc(MSGID *,cchar *,MSGID_CUR *,MSGID_ENT *) noex ;
extern int msgid_fetchhostsvc(MSGID *,uint,cc *,MSGID_CUR *,MSGID_ENT *) noex ;
extern int msgid_fetchhostpid(MSGID *,uint,int,MSGID_CUR *,MSGID_ENT *) noex ;
#endif /* COMMENT */

EXTERNC_end


#endif /* MSGID_INCLUDE */


