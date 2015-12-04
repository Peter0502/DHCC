#ifndef _BALFTP_H
#define _BALFTP_H

#define CSLFileName           128
#define BAL_LINE_MAX   			  1024
#define RPT_LINE_MAX   			  1024

/** for BALFTP: request type   **/
#define BALFTP_FILEINFO          '0'
#define BALFTP_GETFILE           '1'
#define BALFTP_PUTFILE           '2'

/** flag definitions  ****/
/** When putting file into remote machine, if the file exists, and if flag is
	BFLAG_NORMAL, file will not be overwritten, if flag is BFLAG_OVERWRITE,
	file will be overwritten.                                            ***/

#define BFLAG_NORMAL                '0' 
#define BFLAG_OVERWRITE             '1'

/** BALFTP error code **/
#define EBALFTP_INVALID_REQUEST     -401
#define EBALFTP_UNKNOWN_REQUEST     -402
#define EBALFTP_NOTACCESS			-403
#define EBALFTP_NOTEXIST            -404
#define EBALFTP_SYSTEM              -405
#define EBALFTP_FILE_CHANGED        -406
#define EBALFTP_ALLOC_ERROR         -407
#define EBALFTP_FILEOPEN            -408
#define EBALFTP_FILESEEK            -409
#define EBALFTP_FILEREAD            -410
#define EBALFTP_FILEWRITE           -411
#define EBALFTP_PERM				-412
#define EBALFTP_INVALID_FILE		-413
#define EBALFTP_FILE_EXIST			-414

typedef struct TAG_BALFTPHEAD{
	char		mesgtype;               /* request type                     */
	char		name[CSLFileName + 1];  /* file name                        */
	int		filesize;               /* file size                        */
	int        modtime;                /* file modification time           */
	int        offset;                 /* offset for putting or getting    */
	int        datasize;               /* the size of data                 */
	char        flag;                   /* flag                             */
} T_BFHD;

typedef struct TAG_BALFTP {
	T_BFHD		head;
	char        data[1];
} T_BFR;

#endif

