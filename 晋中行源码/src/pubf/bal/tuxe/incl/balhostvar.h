/******************************************************************************\
 * BAL Cryptography Server    Author: Fu, Song Date: 1998-11-11
 * File: balhostdef.h
 * Host Variables Definiton
 *
\******************************************************************************/
/***
EXEC SQL include datetime;
****/
EXEC SQL BEGIN DECLARE SECTION;
	struct TAGHV_BALWRKKEY{
		char	branch_no	[ 5 + 1];
		char	tty			[10 + 1];
		char	ip			[ 8 + 1];
		char	pid			[ 5 + 1];
		char	workkey		[32 + 1];
		char	mackey		[32 + 1];
		char	time		[10 + 1];
	} g_hv_balwrkkey;

	struct TAGHV_BALCRYPTOKEY{
		int		index;
		char	type		[ 1 + 1];
		char	begin_time	[ 8 + 1];
		char	key1		[16 + 1];
		char	key2		[16 + 1];
	} g_hv_balcryptokey;

EXEC SQL END DECLARE SECTION;
