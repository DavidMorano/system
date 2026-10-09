/* filesize_config HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* FILESIZE program */
/* version %I% last-modified %G% */

#ifndef	FILESIZECONFIG_INCLUDE
#define	FILESIZECONFIG_INCLUDE


#define	VERSION		"0a"
#define	WHATINFO	"@(#)FILESIZE "

#define	PROGRAMROOTVAR1	"FILESIZE_PROGRAMROOT"
#define	PROGRAMROOTVAR2	"LOCAL"
#define	PROGRAMROOTVAR3	"PROGRAMROOT"

#ifndef	PROGRAMROOT
#define	PROGRAMROOT	"/usr/add-on/local"
#endif

#define	SEARCHNAME	"filesize"

#define	CONFIGFNAME	"etc/filesize/conf"
#define	LOGFNAME	"log/filesize"

#define	TMPDIR		"/tmp"

#define	DEBUGFDVAR1	"FILESIZE_DEBUGFD"
#define	DEBUGFDVAR2	"DEBUGFD"

#ifndef	PI
#define	PI		proginfo
#endif


#endif /* FILESIZECONFIG_INCLUDE */



