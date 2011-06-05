/**
   This module impliments a php plugin that gives the same output as the
   other nblog utilities, but in a php way.

   Every time one need to log something in php, one can call the "nblog" function.
   This function may take a integer as the first argument, and a string as second, 
   or just a string. If no number is provided, 0 is used.

   The number is the code as defined by the log code file converter "logdef_cnv", 
   and it is possible to use constant symbol names, made by this converter.

   All strings send to nblog, is advised to be in the format name=value seperated 
   by simi colons (;). This function automaticly addes a timestamp and a log code, 
   and an optional pid.

   The log output is written to the log file defined by the nblog_file entry in the 
   settings.
*/

#include <main/php.h>
#include <zend_ini.h>
#include <zend_operators.h>
#include "ext/standard/info.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>

ZEND_INI_BEGIN()
	ZEND_INI_ENTRY( "nblog_file", "/tmp/nblog_php.log", ZEND_INI_ALL, NULL )
	ZEND_INI_ENTRY( "nblog_add_pid", "yes", ZEND_INI_ALL, NULL )
	ZEND_INI_ENTRY( "nblog_append", "", ZEND_INI_ALL, NULL )
ZEND_INI_END()

/* We may need to write out some runtime status on connections, cache etc.*/
ZEND_MINFO_FUNCTION( nblog )
{
  php_info_print_table_start();
  php_info_print_table_row(2, "nb++ php log utility", "Write log files in the nb++ format from php");
  php_info_print_table_row(2, "Version", "0.1" );
  php_info_print_table_end();
}

int g_nNeedPid = 0;
FILE *g_pFileLog = NULL;
char *g_pszAppend = NULL;
int g_nLineCount = 0;

void nblog_open( void )
{
	char *pszLogFile;

	pszLogFile = INI_STR( "nblog_file" );
	// printf( "Log file name : %s\n", pszLogFile );

	g_nNeedPid = (strcmp( INI_STR( "nblog_add_pid" ), "yes" ) == 0) ? 1 : 0;
	// printf( "Need pid : %d (%s)\n", g_nNeedPid, INI_STR( "nblog_add_pid" ) );

	g_pszAppend = INI_STR( "nblog_append" );
	// printf( "What to append : %s\n", g_pszAppend );

	g_pszAppend = (strlen( g_pszAppend ) > 0) ? g_pszAppend : NULL;
	if( NULL == (g_pFileLog = fopen( pszLogFile, "a" )))
		fprintf(stderr, "Can't open %s\n", pszLogFile );
}

void nblog_close( void )
{
	if( g_pFileLog ) {
		fclose( g_pFileLog );
		g_pFileLog = NULL;
	}
}

void nblog_append( int nCode, char *pszLogData )
{
	char szTimestamp[ 20 ];
	time_t t = time( NULL );
	struct tm tm_struct;
	localtime_r( &t, &tm_struct );
	strftime( szTimestamp, sizeof( szTimestamp ), "%Y%m%dT%H%M%S", &tm_struct );

	if( !g_pFileLog ) {
		nblog_open();
		if( g_pFileLog == NULL )
			return;
	}

	fprintf( g_pFileLog, "%s;%d", szTimestamp, nCode );
	if( g_nNeedPid )
		fprintf( g_pFileLog, ";PID=%d", getpid());

	if( pszLogData != NULL )
		fprintf( g_pFileLog, ";%s", pszLogData );
	
	if( g_pszAppend )
		fprintf( g_pFileLog, ";%s", g_pszAppend );

	fprintf( g_pFileLog, "\n" );

	if((++g_nLineCount) % 50 == 0 )
		nblog_close();
}

ZEND_FUNCTION( nblog )
{
	zval **ppArg1, **ppData;

	if(ZEND_NUM_ARGS() == 0 && ZEND_NUM_ARGS() > 2 ) 
		WRONG_PARAM_COUNT;

	if( ZEND_NUM_ARGS() == 1 ) {
		if( zend_get_parameters_ex( 1, &ppArg1 ) == FAILURE )
			WRONG_PARAM_COUNT;
	}
	else {
		if( zend_get_parameters_ex( 2, &ppArg1, &ppData ) == FAILURE )
			WRONG_PARAM_COUNT;

		if((*ppArg1)->type != IS_LONG )
			WRONG_PARAM_COUNT;
	}
			
	if((*ppArg1)->type == IS_STRING ) 
		nblog_append( 0, Z_STRVAL_PP( ppArg1 ));
	else {
		if((*ppArg1)->type == IS_LONG )
			nblog_append( Z_LVAL_PP( ppArg1 ), Z_STRVAL_PP( ppData ));
		else
			zend_error( E_NOTICE, "Wrong first argument" );
	}
} 

zend_function_entry nblog_functions[] =
{
    ZEND_FE( nblog, NULL)
    {NULL, NULL, NULL}
};

ZEND_RSHUTDOWN_FUNCTION(nblog)
{
	if( g_pFileLog )
		fflush( g_pFileLog );

	return SUCCESS;
}

ZEND_MINIT_FUNCTION( nblog )
{
	REGISTER_INI_ENTRIES();

	return SUCCESS;
}

ZEND_MSHUTDOWN_FUNCTION( nblog )
{
	UNREGISTER_INI_ENTRIES();

	nblog_close();

	return SUCCESS;
}

#if COMPILE_DL_FIRST_MODULE
ZEND_GET_MODULE(nblogmod)
#endif

zend_module_entry nblog_module_entry =
{
    STANDARD_MODULE_HEADER,
    "nblog module",
    nblog_functions,
    ZEND_MINIT(nblog), 
    ZEND_MSHUTDOWN(nblog), 
    NULL, 
    ZEND_RSHUTDOWN(nblog), 
    ZEND_MINFO(nblog),
    NO_VERSION_YET,
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL
    ZEND_GET_MODULE( nblog )
#endif
