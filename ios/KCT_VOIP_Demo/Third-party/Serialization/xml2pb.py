from xml.dom.minidom import parse, parseString
import sys
import re

buildtintype ='''

message SKBuiltinInt32_t {
    required uint32 iVal = 1;
}

message SKBuiltinUint32_t {
    required uint32 uiVal = 1;
}

message SKBuiltinChar_t {
    required int32  iVal = 1;
}

message SKBuiltinUchar_t {
    required uint32 uiVal = 1;
}

message SKBuiltinInt8_t {
    required int32 iVal = 1;
}

message SKBuiltinUint8_t {
    required uint32 uiVal = 1;
}

message SKBuiltinInt16_t {
    required int32 iVal = 1;
}

message SKBuiltinUint16_t {
    required uint32 uiVal = 1;
}

message SKBuiltinInt64_t {
    required int64 llVal = 1;
}

message SKBuiltinUint64_t {
    required uint64 ullVal = 1;
}

message SKBuiltinFloat32_t {
    required float fVal = 1;
}

message SKBuiltinDouble64_t {
    required double dVal = 1;
}

message SKBuiltinBuffer_t {
    required uint32 iLen = 1;
    optional bytes Buffer = 2;
}

message SKBuiltinString_t {
    optional string String = 1;
}

message EchoInfo {
    required uint32 EchoLen = 1;
    required bytes EchoStr = 2;
}
''';

baserepeatedtype = ["*uint16","*int16","*uint32","*int32","*uint64","*int64","*float32","*double64", "*sint32", "*sint64" ];

def createstruct( ):
	print buildtintype

def xml2pb( path ):

	xmldom = parse( path )

	for st in xmldom.getElementsByTagName( "struct" ):
		##print "struct %s" % ( st.getAttribute( "name" ) )

		print "message %s {" % ( st.getAttribute( "name" ) )

		for fld in st.getElementsByTagName( "field" ):
			#print "\tfield name [%s], \ttype [%s], \tarraysize [%s], \treferto [%s]" % \
			#       ( fld.getAttribute( "name" ), fld.getAttribute( "type" ), \
			#       fld.getAttribute( "arraysize" ), fld.getAttribute( "referto" ) )

			strfld = "required"
			stroption = "";
						
			if "0" == fld.getAttribute( "required" ):
				strfld = "optional"

			arraysize = fld.getAttribute( "arraysize" )
			referto = fld.getAttribute( "referto" )
			type = fld.getAttribute( "type" )
			name = fld.getAttribute( "name" )
			id = fld.getAttribute( "id" )

			if ( not type.endswith( "char" ) ) and ( len( arraysize ) > 0 or len( referto ) > 0 ):
				strfld = "repeated"

			stdtype = ""

			if "*char" == type or "char" == type:
						stdtype = "string"

			elif type.endswith( "uint32" ) or type.endswith( "uint16" ) or type.endswith( "uint8" ):
				stdtype = "uint32"
			elif type.endswith( "int32" ) or type.endswith( "int16" ) or type.endswith( "int*" ):
				stdtype = "int32"
			elif type.endswith( "sint32" ) or type.endswith( "sint16" ) or type.endswith( "sint8" ):
				stdtype = "sint32"
			elif type.endswith( "double64" ):
				stdtype = "double"
			elif type.endswith( "float32" ):
				stdtype = "float"
			else:
				if "*" == type[0]: stdtype = type[ 1 : ]
				else: stdtype = type

			if ( len( arraysize ) > 0 or len( referto ) > 0 ) \
					and ( type.endswith( "uint8" ) or type.endswith( "int8" ) ):
				strfld = "required"
				stdtype = "bytes"

			if( ( len( arraysize ) > 0 or ( len( referto ) > 0 and  referto <> "strlen" ) )
					and ( type.endswith( "char" ) or type.endswith( "uchar" ) ) ):
				strfld = "required"
				stdtype = "bytes"

			if "*char" == type or "*uint8" == type or "*int8" == type:
				strfld = "optional"
		
			if type in baserepeatedtype:
				stroption = "[packed=true]";

			strfld = strfld + " " + stdtype + " " + name + " = " + id + stroption + ";"

			print "\t%s" % ( strfld )

		print "}"

if __name__ == "__main__":

	if len( sys.argv ) < 2:
		print "Usage: %s <xml file>\n" % ( sys.argv[0] )
		sys.exit( -1 )

	hdr = """package micromsg;
			option java_package = "com.tencent.micromsg";
			option java_outer_classname = "MicroMsg";
			"""

	hdr = hdr.replace( "\t", "" )

	print hdr

	createstruct();
	print ("\n");	

	path = sys.argv[1]
	xml2pb( path )


