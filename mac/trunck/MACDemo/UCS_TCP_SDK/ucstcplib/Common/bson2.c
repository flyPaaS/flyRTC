/* bson.c */

/*    Copyright 2009, 2010 10gen Inc.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "bson2.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

const int initialBufferSize2 = 128;

/* only need one of these */
static const int zero = 0;

/* ----------------------------
   READING
   ------------------------------ */

bson * bson_empty2(bson * obj){
    static char * data = "\005\0\0\0\0";
    return bson_init2(obj, data, 0);
}

void bson_copy2(bson* out, const bson* in){
    if (!out) return;
    out->data = bson_malloc2(bson_size2(in));
    out->owned = 1;
    memcpy(out->data, in->data, bson_size2(in));
}

bson * bson_from_buffer2(bson * b, bson_buffer * buf){
    return bson_init2(b, bson_buffer_finish2(buf), 1);
}

bson * bson_init2( bson * b , char * data , bson_bool_t mine ){
    b->data = data;
    b->owned = mine;
    return b;
}
int bson_size2(const bson * b ){
    int i;
    if ( ! b || ! b->data )
        return 0;
    bson_little_endian32(&i, b->data);
    return i;
}
void bson_destroy2( bson * b ){
    if ( b->owned && b->data )
        free( b->data );
    b->data = 0;
    b->owned = 0;
}

static char hexbyte2(char hex){
    switch (hex){
        case '0': return 0x0;
        case '1': return 0x1;
        case '2': return 0x2;
        case '3': return 0x3;
        case '4': return 0x4;
        case '5': return 0x5;
        case '6': return 0x6;
        case '7': return 0x7;
        case '8': return 0x8;
        case '9': return 0x9;
        case 'a': 
        case 'A': return 0xa;
        case 'b':
        case 'B': return 0xb;
        case 'c':
        case 'C': return 0xc;
        case 'd':
        case 'D': return 0xd;
        case 'e':
        case 'E': return 0xe;
        case 'f':
        case 'F': return 0xf;
        default: return 0x0; /* something smarter? */
    }
}

void bson_oid_from_string2(bson_oid_t* oid, const char* str){
    int i;
    for (i=0; i<12; i++){
        oid->bytes[i] = (hexbyte2(str[2*i]) << 4) | hexbyte2(str[2*i + 1]);
    }
}
void bson_oid_to_string2(const bson_oid_t* oid, char* str){
    static const char hex[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
    int i;
    for (i=0; i<12; i++){
        str[2*i]     = hex[(oid->bytes[i] & 0xf0) >> 4];
        str[2*i + 1] = hex[ oid->bytes[i] & 0x0f      ];
    }
    str[24] = '\0';
}
void bson_oid_gen2(bson_oid_t* oid){
    static int incr = 0;
    static int fuzz = 0;
    int i = incr++; /*TODO make atomic*/
    int t = time(NULL);

    /* TODO rand sucks. find something better */
    if (!fuzz){
        srand(t);
        fuzz = rand();
    }
    
    bson_big_endian32(&oid->ints[0], &t);
    oid->ints[1] = fuzz;
    bson_big_endian32(&oid->ints[2], &i);
}

time_t bson_oid_generated_time2(bson_oid_t* oid){
    time_t out;
    bson_big_endian32(&out, &oid->ints[0]);
    return out;
}

void bson_print2( bson * b ){
    bson_print_raw2( b->data , 0 );
}

void bson_print_raw2( const char * data , int depth ){
    bson_iterator i;
    const char * key;
    int temp;
    char oidhex[25];
    bson_iterator_init2( &i , data );

    while ( bson_iterator_next2( &i ) ){
        bson_type t = bson_iterator_type2( &i );
        if ( t == 0 )
            break;
        key = bson_iterator_key2( &i );
        
        for ( temp=0; temp<=depth; temp++ )
            printf( "\t" );
        printf( "%s : %d \t " , key , t );
        switch ( t ){
        case bson_int: printf( "%d" , bson_iterator_int2( &i ) ); break;
        case bson_double: printf( "%f" , bson_iterator_double2( &i ) ); break;
        case bson_bool: printf( "%s" , bson_iterator_bool2( &i ) ? "true" : "false" ); break;
        case bson_string: printf( "%s" , bson_iterator_string2( &i ) ); break;
        case bson_null: printf( "null" ); break;
        case bson_oid: bson_oid_to_string2(bson_iterator_oid2(&i), oidhex); printf( "%s" , oidhex ); break;
        case bson_object:
        case bson_array:
            printf( "\n" );
            bson_print_raw2( bson_iterator_value2( &i ) , depth + 1 );
            break;
        default:
            fprintf( stderr , "can't print type : %d\n" , t );
        }
        printf( "\n" );
    }
}

/* ----------------------------
   ITERATOR
   ------------------------------ */

void bson_iterator_init2( bson_iterator * i , const char * bson ){
    i->cur = bson + 4;
    i->first = 1;
}

bson_type bson_find2(bson_iterator* it, const bson* obj, const char* name){
    bson_iterator_init2(it, obj->data);
    while(bson_iterator_next2(it)){
        if (strcmp(name, bson_iterator_key2(it)) == 0)
            break;
    }
    return bson_iterator_type2(it);
}

bson_bool_t bson_iterator_more2( const bson_iterator * i ){
    return *(i->cur);
}

bson_type bson_iterator_next2( bson_iterator * i ){
    int ds;

    if ( i->first ){
        i->first = 0;
        return (bson_type)(*i->cur);
    }
    
    switch ( bson_iterator_type2(i) ){
    case bson_eoo: return bson_eoo; /* don't advance */
    case bson_undefined:
    case bson_null: ds = 0; break;
    case bson_bool: ds = 1; break;
    case bson_int: ds = 4; break;
    case bson_long:
    case bson_double:
    case bson_timestamp:
    case bson_date: ds = 8; break;
    case bson_oid: ds = 12; break;
    case bson_string:
    case bson_symbol:
    case bson_code: ds = 4 + bson_iterator_int_raw2(i); break;
    case bson_bindata: ds = 5 + bson_iterator_int_raw2(i); break;
    case bson_object:
    case bson_array:
    case bson_codewscope: ds = bson_iterator_int_raw2(i); break;
    case bson_dbref: ds = 4+12 + bson_iterator_int_raw2(i); break;
    case bson_regex:
        {
            const char * s = bson_iterator_value2(i);
            const char * p = s;
            p += strlen(p)+1;
            p += strlen(p)+1;
            ds = p-s;
            break;
        }

    default: 
        {
            char msg[] = "unknown type: 000000000000";
            bson_numstr2(msg+14, (unsigned)(i->cur[0]));
            bson_fatal_msg2(0, msg);
            return 0;
        }
    }
    
    i->cur += 1 + strlen( i->cur + 1 ) + 1 + ds;

    return (bson_type)(*i->cur);
}

bson_type bson_iterator_type2( const bson_iterator * i ){
    return (bson_type)i->cur[0];
}
const char * bson_iterator_key2( const bson_iterator * i ){
    return i->cur + 1;
}
const char * bson_iterator_value2( const bson_iterator * i ){
    const char * t = i->cur + 1;
    t += strlen( t ) + 1;
    return t;
}

/* types */

int bson_iterator_int_raw2( const bson_iterator * i ){
    int out;
    bson_little_endian32(&out, bson_iterator_value2( i ));
    return out;
}
double bson_iterator_double_raw2( const bson_iterator * i ){
    double out;
    bson_little_endian64(&out, bson_iterator_value2( i ));
    return out;
}
int64_t bson_iterator_long_raw2( const bson_iterator * i ){
    int64_t out;
    bson_little_endian64(&out, bson_iterator_value2( i ));
    return out;
}

bson_bool_t bson_iterator_bool_raw2( const bson_iterator * i ){
    return bson_iterator_value2( i )[0];
}

bson_oid_t * bson_iterator_oid2( const bson_iterator * i ){
    return (bson_oid_t*)bson_iterator_value2(i);
}

int bson_iterator_int2( const bson_iterator * i ){
    switch (bson_iterator_type2(i)){
        case bson_int: return bson_iterator_int_raw2(i);
        case bson_long: return bson_iterator_long_raw2(i);
        case bson_double: return bson_iterator_double_raw2(i);
        default: return 0;
    }
}
double bson_iterator_double2( const bson_iterator * i ){
    switch (bson_iterator_type2(i)){
        case bson_int: return bson_iterator_int_raw2(i);
        case bson_long: return bson_iterator_long_raw2(i);
        case bson_double: return bson_iterator_double_raw2(i);
        default: return 0;
    }
}
int64_t bson_iterator_long2( const bson_iterator * i ){
    switch (bson_iterator_type2(i)){
        case bson_int: return bson_iterator_int_raw2(i);
        case bson_long: return bson_iterator_long_raw2(i);
        case bson_double: return bson_iterator_double_raw2(i);
        default: return 0;
    }
}

bson_bool_t bson_iterator_bool2( const bson_iterator * i ){
    switch (bson_iterator_type2(i)){
        case bson_bool: return bson_iterator_bool_raw2(i);
        case bson_int: return bson_iterator_int_raw2(i) != 0;
        case bson_long: return bson_iterator_long_raw2(i) != 0;
        case bson_double: return bson_iterator_double_raw2(i) != 0;
        case bson_eoo:
        case bson_null: return 0;
        default: return 1;
    }
}

const char * bson_iterator_string2( const bson_iterator * i ){
    return bson_iterator_value2( i ) + 4;
}
int bson_iterator_string_len2( const bson_iterator * i ){
    return bson_iterator_int_raw2( i );
}

const char * bson_iterator_code2( const bson_iterator * i ){
    switch (bson_iterator_type2(i)){
        case bson_string:
        case bson_code: return bson_iterator_value2(i) + 4;
        case bson_codewscope: return bson_iterator_value2(i) + 8;
        default: return NULL;
    }
}

void bson_iterator_code_scope2(const bson_iterator * i, bson * scope){
    if (bson_iterator_type2(i) == bson_codewscope){
        int code_len;
        bson_little_endian32(&code_len, bson_iterator_value2(i)+4);
        bson_init2(scope, (void*)(bson_iterator_value2(i)+8+code_len), 0);
    }else{
        bson_empty2(scope);
    }
}

bson_date_t bson_iterator_date2(const bson_iterator * i){
    return bson_iterator_long_raw2(i);
}

time_t bson_iterator_time_t2(const bson_iterator * i){
    return bson_iterator_date2(i) / 1000;
}

int bson_iterator_bin_len2( const bson_iterator * i ){
    return bson_iterator_int_raw2( i );
}

char bson_iterator_bin_type2( const bson_iterator * i ){
    return bson_iterator_value2(i)[4];
}
const char * bson_iterator_bin_data2( const bson_iterator * i ){
    return bson_iterator_value2( i ) + 5;
}

const char * bson_iterator_regex2( const bson_iterator * i ){
    return bson_iterator_value2( i );
}
const char * bson_iterator_regex_opts2( const bson_iterator * i ){
    const char* p = bson_iterator_value2( i );
    return p + strlen(p) + 1;

}

void bson_iterator_subobject2(const bson_iterator * i, bson * sub){
    bson_init2(sub, (char*)bson_iterator_value2(i), 0);
}
void bson_iterator_subiterator2(const bson_iterator * i, bson_iterator * sub){
    bson_iterator_init2(sub, bson_iterator_value2(i));
}

/* ----------------------------
   BUILDING
   ------------------------------ */

bson_buffer * bson_buffer_init2( bson_buffer * b ){
    b->buf = (char*)bson_malloc2( initialBufferSize2 );
    b->bufSize = initialBufferSize2;
    b->cur = b->buf + 4;
    b->finished = 0;
    b->stackPos = 0;
    return b;
}

void bson_append_byte( bson_buffer * b , char c ){
    b->cur[0] = c;
    b->cur++;
}
void bson_append( bson_buffer * b , const void * data , int len ){
    memcpy( b->cur , data , len );
    b->cur += len;
}
void bson_append32(bson_buffer * b, const void * data){
    bson_little_endian32(b->cur, data);
    b->cur += 4;
}
void bson_append64(bson_buffer * b, const void * data){
    bson_little_endian64(b->cur, data);
    b->cur += 8;
}

bson_buffer * bson_ensure_space2( bson_buffer * b , const int bytesNeeded ){
    int pos = b->cur - b->buf;
    char * orig = b->buf;
    int new_size;

    if (b->finished)
        bson_fatal_msg2(!!b->buf, "trying to append to finished buffer");

    if (pos + bytesNeeded <= b->bufSize)
        return b;

    new_size = 1.5 * (b->bufSize + bytesNeeded);
    b->buf = realloc(b->buf, new_size);
    if (!b->buf)
        bson_fatal_msg2(!!b->buf, "realloc() failed");

    b->bufSize = new_size;
    b->cur += b->buf - orig;

    return b;
}

char * bson_buffer_finish2( bson_buffer * b ){
    int i;
    if ( ! b->finished ){
        if ( ! bson_ensure_space2( b , 1 ) ) return 0;
        bson_append_byte( b , 0 );
        i = b->cur - b->buf;
        bson_little_endian32(b->buf, &i);
        b->finished = 1;
    }
    return b->buf;
}

void bson_buffer_destroy2( bson_buffer * b ){
    free( b->buf );
    b->buf = 0;
    b->cur = 0;
    b->finished = 1;
}

static bson_buffer * bson_append_estart2( bson_buffer * b , int type , const char * name , const int dataSize ){
    const int sl = strlen(name) + 1;
    if ( ! bson_ensure_space2( b , 1 + sl + dataSize ) )
        return 0;
    bson_append_byte( b , (char)type );
    bson_append( b , name , sl );
    return b;
}

/* ----------------------------
   BUILDING TYPES
   ------------------------------ */

bson_buffer * bson_append_int2( bson_buffer * b , const char * name , const int i ){
    if ( ! bson_append_estart2( b , bson_int , name , 4 ) ) return 0;
    bson_append32( b , &i );
    return b;
}
bson_buffer * bson_append_long2( bson_buffer * b , const char * name , const int64_t i ){
    if ( ! bson_append_estart2( b , bson_long , name , 8 ) ) return 0;
    bson_append64( b , &i );
    return b;
}
bson_buffer * bson_append_double2( bson_buffer * b , const char * name , const double d ){
    if ( ! bson_append_estart2( b , bson_double , name , 8 ) ) return 0;
    bson_append64( b , &d );
    return b;
}
bson_buffer * bson_append_bool2( bson_buffer * b , const char * name , const bson_bool_t i ){
    if ( ! bson_append_estart2( b , bson_bool , name , 1 ) ) return 0;
    bson_append_byte( b , i != 0 );
    return b;
}
bson_buffer * bson_append_null2( bson_buffer * b , const char * name ){
    if ( ! bson_append_estart2( b , bson_null , name , 0 ) ) return 0;
    return b;
}
bson_buffer * bson_append_undefined2( bson_buffer * b , const char * name ){
    if ( ! bson_append_estart2( b , bson_undefined , name , 0 ) ) return 0;
    return b;
}
bson_buffer * bson_append_string_base( bson_buffer * b , const char * name , const char * value , bson_type type){
    int sl = strlen( value ) + 1;
    if ( ! bson_append_estart2( b , type , name , 4 + sl ) ) return 0;
    bson_append32( b , &sl);
    bson_append( b , value , sl );
    return b;
}
bson_buffer * bson_append_string2( bson_buffer * b , const char * name , const char * value ){
    return bson_append_string_base(b, name, value, bson_string);
}
bson_buffer * bson_append_symbol2( bson_buffer * b , const char * name , const char * value ){
    return bson_append_string_base(b, name, value, bson_symbol);
}
bson_buffer * bson_append_code2( bson_buffer * b , const char * name , const char * value ){
    return bson_append_string_base(b, name, value, bson_code);
}

bson_buffer * bson_append_code_w_scope2( bson_buffer * b , const char * name , const char * code , const bson * scope){
    int sl = strlen(code) + 1;
    int size = 4 + 4 + sl + bson_size2(scope);
    if (!bson_append_estart2(b, bson_codewscope, name, size)) return 0;
    bson_append32(b, &size);
    bson_append32(b, &sl);
    bson_append(b, code, sl);
    bson_append(b, scope->data, bson_size2(scope));
    return b;
}

bson_buffer * bson_append_binary2( bson_buffer * b, const char * name, char type, const char * str, int len ){
    if ( ! bson_append_estart2( b , bson_bindata , name , 4+1+len ) ) return 0;
    bson_append32(b, &len);
    bson_append_byte(b, type);
    bson_append(b, str, len);
    return b;
}
bson_buffer * bson_append_oid2( bson_buffer * b , const char * name , const bson_oid_t * oid ){
    if ( ! bson_append_estart2( b , bson_oid , name , 12 ) ) return 0;
    bson_append( b , oid , 12 );
    return b;
}
bson_buffer * bson_append_new_oid2( bson_buffer * b , const char * name ){
    bson_oid_t oid;
    bson_oid_gen2(&oid);
    return bson_append_oid2(b, name, &oid);
}

bson_buffer * bson_append_regex2( bson_buffer * b , const char * name , const char * pattern, const char * opts ){
    const int plen = strlen(pattern)+1;
    const int olen = strlen(opts)+1;
    if ( ! bson_append_estart2( b , bson_regex , name , plen + olen ) ) return 0;
    bson_append( b , pattern , plen );
    bson_append( b , opts , olen );
    return b;
}

bson_buffer * bson_append_bson2( bson_buffer * b , const char * name , const bson* bson){
    if ( ! bson_append_estart2( b , bson_object , name , bson_size2(bson) ) ) return 0;
    bson_append( b , bson->data , bson_size2(bson) );
    return b;
}

bson_buffer * bson_append_element2( bson_buffer * b, const char * name_or_null, const bson_iterator* elem){
    bson_iterator next = *elem;
    int size;

    bson_iterator_next2(&next);
    size = next.cur - elem->cur;

    if (name_or_null == NULL){
        bson_ensure_space2(b, size);
        bson_append(b, elem->cur, size);
    }else{
        int data_size = size - 1 - strlen(bson_iterator_key2(elem));
        bson_append_estart2(b, elem->cur[0], name_or_null, data_size);
        bson_append(b, name_or_null, strlen(name_or_null));
        bson_append(b, bson_iterator_value2(elem), data_size);
    }

    return b;
}

bson_buffer * bson_append_date2( bson_buffer * b , const char * name , bson_date_t millis ){
    if ( ! bson_append_estart2( b , bson_date , name , 8 ) ) return 0;
    bson_append64( b , &millis );
    return b;
}

bson_buffer * bson_append_time_t2( bson_buffer * b , const char * name , time_t secs){
    return bson_append_date2(b, name, (bson_date_t)secs * 1000);
}

bson_buffer * bson_append_start_object2( bson_buffer * b , const char * name ){
    if ( ! bson_append_estart2( b , bson_object , name , 5 ) ) return 0;
    b->stack[ b->stackPos++ ] = b->cur - b->buf;
    bson_append32( b , &zero );
    return b;
}

bson_buffer * bson_append_start_array2( bson_buffer * b , const char * name ){
    if ( ! bson_append_estart2( b , bson_array , name , 5 ) ) return 0;
    b->stack[ b->stackPos++ ] = b->cur - b->buf;
    bson_append32( b , &zero );
    return b;
}

bson_buffer * bson_append_finish_object2( bson_buffer * b ){
    char * start;
    int i;
    if ( ! bson_ensure_space2( b , 1 ) ) return 0;
    bson_append_byte( b , 0 );
    
    start = b->buf + b->stack[ --b->stackPos ];
    i = b->cur - start;
    bson_little_endian32(start, &i);

    return b;
}

void* bson_malloc2(int size){
    void* p = malloc(size);
    bson_fatal_msg2(!!p, "malloc() failed");
    return p;
}

static bson_err_handler err_handler = NULL;

bson_err_handler set_bson_err_handler2(bson_err_handler func){
    bson_err_handler old = err_handler;
    err_handler = func;
    return old;
}

void bson_fatal2( int ok ){
    bson_fatal_msg2(ok, "");
}

void bson_fatal_msg2( int ok , const char* msg){
    if (ok)
        return;

    if (err_handler){
        err_handler(msg);
    }

    fprintf( stderr , "error: %s\n" , msg );
    exit(-5);
}

extern const char bson_numstrs[1000][4];
void bson_numstr2(char* str, int i){
    if(i < 1000)
        memcpy(str, bson_numstrs[i], 4);
    else
        sprintf(str,"%d", i);
}
