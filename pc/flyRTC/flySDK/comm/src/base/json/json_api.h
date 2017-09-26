/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : pcp_cfg.h
  Version       : Initial Draft
  Author        : gonghuojin
  Created       : 2013/9/23
  Last Modified :
  Description   : json api header file
  Function List :
  History       :
  1.Date        : 2013/9/23
    Author      : gonghuojin
    Modification: Created file

******************************************************************************/
#ifndef JSON_API_H
#define JSON_API_H

#ifdef __cplusplus
extern "C"
{
#endif


/**
The descriptions of the json_value node type
**/
	enum json_value_type
	{ 
		JSON_STRING = 0, 
		JSON_NUMBER, 
		JSON_OBJECT, 
		JSON_ARRAY, 
		JSON_TRUE, 
		JSON_FALSE, 
		JSON_NULL 
	};
	typedef enum json_value_type ejson_type;

/**
The error messages produced by the JSON parsers
**/
	enum json_error
	{
		JSON_OK = 1,	/*!< everything went smoothly */
		JSON_INCOMPLETE_DOCUMENT,	/*!< the parsed document didn't ended */
		JSON_WAITING_FOR_EOF,	/*!< A complete JSON document tree was already finished but needs to get to EOF. Other characters beyond whitespaces produce errors */
		JSON_MALFORMED_DOCUMENT,	/* the JSON document which was fed to this parser is malformed */
		JSON_INCOMPATIBLE_TYPE,	/*!< the currently parsed type does not belong here */
		JSON_MEMORY,	/*!< an error occurred when allocating memory */
		JSON_ILLEGAL_CHARACTER,	/*!< the currently parsed character does not belong here */
		JSON_BAD_TREE_STRUCTURE,	/*!< the document tree structure is malformed */
		JSON_MAXIMUM_LENGTH,	/*!< the parsed string reached the maximum allowed size */
		JSON_UNKNOWN_PROBLEM	/*!< some random, unaccounted problem occurred */
	};
	typedef enum json_error  ejson_res;

/**
The JSON document tree node, which is a basic JSON type
**/
	struct json_value
	{
		enum json_value_type type;	/*!< the type of node */
		char *text;	/*!< The text stored by the node. It stores UTF-8 strings and is used exclusively by the JSON_STRING and JSON_NUMBER node types */

		/* FIFO queue data */
		struct json_value *next;	/*!< The pointer pointing to the next element in the FIFO sibling list */
		struct json_value *previous;	/*!< The pointer pointing to the previous element in the FIFO sibling list */
		struct json_value *parent;	/*!< The pointer pointing to the parent node in the document tree */
		struct json_value *child;	/*!< The pointer pointing to the first child node in the document tree */
		struct json_value *child_end;	/*!< The pointer pointing to the last child node in the document tree */
	};


	typedef struct json_value json_t;


char* jsonapi_format_string(char* str);

json_t* jsonapi_new_value (const ejson_type type, const char* value);

void jsonapi_delete_value (json_t ** value);

ejson_res jsonapi_value_to_string (json_t* js_obj, char **str); 

ejson_res jsonapi_string_to_value(json_t **root, char *str);

ejson_res jsonapi_append_value (json_t* js_obj, const char *name, json_t* value); 

ejson_res jsonapi_parser_value(json_t* js_root, const char *name, json_t** value);

ejson_res jsonapi_parser_string(json_t* js_root, const char *name, char* str);

ejson_res jsonapi_parser_number(json_t* js_root, const char *name, int* number);


void jsonapi_print_value(json_t* js_obj);



#ifdef __cplusplus
}
#endif

#endif


