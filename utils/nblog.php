<?
/**
   This is a quite simpel tool to log information into a file, while using simple
   PHP buildin functions. This utility make sure that the log format is keept in
   the nb++ format, and will therefor coorperate with other nb++ log tools.

   The nb++ logfile definition tool, will also be able to provide definitions for
   in php format.

   This small code can either be included in directly the php code, or be included
   automiticly using the "auto_prepend_file" ini option.

   php_admin_value auto_prepend_file /path/to/file/lib/nblog.php
*/

define( "LOGPID", FALSE );
define( "LOGFILE", "/tmp/nbinfo.log" );

/**
   If arg1 is a numeric code, arg2 must be a string, else code is assumed to be
   0.
*/
function nblog( $arg1, $arg2 = NULL ) {
	$log_line = strftime( "%Y%m%dT%H%M%S;" );

	if( is_numeric( $arg1 ))
		$log_line .= $arg1.";";
	else
		$log_line .= "0;";

	if( LOGPID )
		$log_line .= "PID=".getmypid().";";

	if( $arg2 == NULL ) 
		$log_line .= $arg1;
	else
		$log_line .= $arg2;

	error_log ( $log_line."\n", 3, LOGFILE );
}


// nblog( "Simpel test" );
// nblog( 10, "ID=10;NAME=test" );

?>