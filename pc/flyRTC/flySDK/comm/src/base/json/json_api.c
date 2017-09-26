#include <stdlib.h>
#include <string.h>
#include "json_api.h"
#include "json.h"


char* jsonapi_format_string(char* str)
{
	return json_format_string(str);
}

json_t* jsonapi_new_value (const ejson_type type, const char* value)

{
	json_t* js = NULL;

	switch(type)
	{
		case JSON_OBJECT:
		case JSON_ARRAY:
		case JSON_TRUE:
		case JSON_FALSE:
		case JSON_NULL:
			js = json_new_value(type);
			break;
		case JSON_STRING:
			js = json_new_string((char*)value);
			break;
		case JSON_NUMBER:
			js = json_new_number((char*)value);
			break;
		default:
			break;
	}

	return js;	
}

void jsonapi_delete_value (json_t ** value)
{
	json_free_value(value);
}


ejson_res jsonapi_value_to_string (json_t* js_obj, char **str)
{
	return json_tree_to_string(js_obj, str);
}

ejson_res jsonapi_string_to_value(json_t **root, char *str)
{
	return json_parse_document(root, str);
}

ejson_res jsonapi_append_value (json_t* js_obj, const char *name, json_t* value)
{
	ejson_res res = JSON_OK;

	if (js_obj->type == JSON_ARRAY)
	{
		res = json_insert_child(js_obj, value);
	}
	else if (js_obj->type == JSON_OBJECT)
	{
		res = json_insert_pair_into_object (js_obj, name, value);
	}
	
	return res;
}


ejson_res jsonapi_parser_number(json_t* js_root, const char *name, int* number)
{
	ejson_res res = JSON_OK;
	json_t* jtmp = NULL;
	json_t* jchild = NULL;
		
	jtmp = json_find_first_label (js_root, name);
	if (jtmp == NULL)
		return JSON_ILLEGAL_CHARACTER;

	jchild = jtmp->child;
	if ((jchild == NULL ) || (jchild->type != JSON_NUMBER))
		return JSON_BAD_TREE_STRUCTURE;

	*number = atoi(jchild->text);

	return res;
}


ejson_res jsonapi_parser_string(json_t* js_root, const char *name, char* str)
{
	ejson_res res = JSON_OK;
	json_t* jtmp = NULL;
	json_t* jchild = NULL;

	if (name != NULL)
	{
		jtmp = json_find_first_label (js_root, name);
		if (jtmp == NULL)
			return JSON_ILLEGAL_CHARACTER;

		jchild = jtmp->child;
		if ((jchild == NULL ) || (jchild->type != JSON_STRING))
			return JSON_BAD_TREE_STRUCTURE;

		strcpy(str, jchild->text);
	}
	else if (js_root != NULL)
	{
		strcpy(str, js_root->text);
	}
	
	return res;
}

ejson_res jsonapi_parser_value(json_t* js_root, const char *name, json_t** value)
{
	ejson_res res = JSON_OK;
	json_t* jtmp = NULL;
	json_t* jchild = NULL;
		
	jtmp = json_find_first_label (js_root, name);
	if (jtmp == NULL)
		return JSON_ILLEGAL_CHARACTER;

	jchild = jtmp->child;
	if ((jchild == NULL) 
		|| (jchild->type != JSON_OBJECT && jchild->type != JSON_ARRAY))
		return JSON_BAD_TREE_STRUCTURE;

	*value = jchild;

	return res;
}

void jsonapi_print_value(json_t* js_obj)
{
	json_render_tree(js_obj);
}





