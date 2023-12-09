/* writen */


/* external subroutines */

extern int	uc_writen(int,void *,int) noex ;


/* exported subroutines */

int writen(int fd,char *buf,int len) noex {
	return uc_writen(fd,buf,len) ;
}


