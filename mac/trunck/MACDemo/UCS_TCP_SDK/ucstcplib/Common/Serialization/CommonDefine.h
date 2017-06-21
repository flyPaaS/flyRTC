/*
 *  CommonDefine.h
 *  NoteDemo
 *
 *  Created by andyguan on 10-9-26.
 *
 */

/// 满足条件时返回特定值
#define ON_LESS_RETURN(statement,borderval,retval) if ((statement) < (borderval)) return (retval);
#define ON_GTR_RETURN(statement,borderval,retval) if ((statement) > (borderval)) return (retval);
#define ON_EQU_RETURN(statement,borderval,retval) if ((statement) == (borderval)) return (retval);
#define ON_NEQU_RETURN(statement,borderval,retval) if ((statement) != (borderval)) return (retval);

/// 满足条件时执行指定语句，然后返回特定值
#define ON_LESS_DO_RETURN(statement,borderval,beforeret,retval) if ((statement) < (borderval)) { {beforeret;} return (retval);}
#define ON_GTR_DO_RETURN(statement,borderval,beforeret,retval) if ((statement) > (borderval)) { {beforeret;} return (retval);}
#define ON_EQU_DO_RETURN(statement,borderval,beforeret,retval) if ((statement) == (borderval)) { {beforeret;} return (retval);}
#define ON_NEQU_DO_RETURN(statement,borderval,beforeret,retval) if ((statement) != (borderval)) { {beforeret;} return (retval);}


