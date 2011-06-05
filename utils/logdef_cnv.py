#!/usr/bin/python 
# Python script to convert the error descriptions to a C header file
#
# Format :
# :n     : Set the code base number
# Symbol name, Type, "Description"
#
# Symbol name = the symbol name of the log entry
# Type = enum of (ERR, INFO, WARNING)
# Description = Quoted text description of error and interpolated log params 
#
from string import *
import sys
import re

class Generator:
	def header( self ):
		return """
/* Auto generated log code definitions. */

#ifndef __ERROR_CODE_H__
#define __ERROR_CODE_H__

"""

	def line( self, symbol, code ):
		return "#define %s %d\n" % (symbol, code)

	def tail( self ):
		return "\n#endif\n" # Terminate the output file

class PhpGenerator( Generator ):
	def header( self ):
		return "<?\n// Auto generated log code definitions\n"

	def line( self, symbol, code ):
		return "define( \"%s\", %d );\n" % (symbol, code )

	def tail( self ):
		return "?>\n";

if len( sys.argv ) == 1:
	print "Usage: logdef_cnv.py logdef_cnv.py inputfile [-base=base_id] [-type=php|cpp]"
	sys.exit( 1 )

gen = Generator();

base_id = 100
fname = ""
for i in range( 1, len(sys.argv)):
	if sys.argv[ i ][ 0 ] == "-":
		g = re.match( "-base=(\d+)", sys.argv[ i ] )
		if g:
			base_id = int( g.groups()[0])
			sys.stderr.write( "Notice: base id number is %d\n" % base_id )
			continue
		
		g = re.match( "-type=(php|cpp)", sys.argv[ i ] )
		if g:
			if g.groups()[0] == "php":
				gen = PhpGenerator()
			continue

		sys.stderr.write( "Wrong option : %s \n" % sys.argv[ i ] )
		sys.exit( 1 )
	else:
		fname = sys.argv[ i ]
			
fd = None
try:
	fd = open( fname )
except IndexError, IOError:
	sys.stderr.write( "Error while opening : %s\n" % fname )
	sys.exit( -1 )
			
res = gen.header();

try:
	id_val = base_id
	line_cnt = 0
	while( fd ):
		line = fd.readline()
		if( not line ):
		    break
		else:
		    line = strip( line )
		    if len( line ) == 0:
			continue
		    
		# get new base value
		if line[0] == ':':
			id_val = atoi( line[ 1: ] )
			sys.stderr.write( "Notice: new base id number is set to %d by input file\n" % id_val )
			continue
		
		items = split( line, ',' )
		name = strip( items[ 0 ] )
		if( len( name )):
			res = res + gen.line(name, id_val)
			id_val = id_val + 1
			line_cnt = line_cnt + 1
    
	res = res + gen.tail()

	print res 
except IOError, IndexError:
    sys.stderr.write( "Error in %s at line %d" % (args[ 1 ], line_cnt))

sys.exit( 0 )
