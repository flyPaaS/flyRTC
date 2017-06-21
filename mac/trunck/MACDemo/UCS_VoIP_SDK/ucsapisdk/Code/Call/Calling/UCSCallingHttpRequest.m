//
//  UCSCallingHttpRequest.m
//  UXinClient
//
//  Created by FengXing on 13-11-15.
//  Copyright (c) 2013年 UXIN CO. All rights reserved.
//

#import "UCSCallingHttpRequest.h"
#import "UCSHttpRequest.h"
#import "SoftphoneManager.h"
#import "UCSPublicDef.h"


 


@implementation UCSCallingHttpRequest {
    NSString * _callId;
    
	CallRequestBlock _successBlock;
	CallRequestBlock _failedBlock;
	UCSHttpRequest *_httpRequest;
	NSMutableDictionary *_emodel;
    BOOL _isEmodel;
}

- (id)init
{
	if (self = [super init]) {
        _httpRequest = [[UCSHttpRequest alloc] initWithRequestStyle:NO isWillReturnJson:YES];
        _callBackRequestQueue = [[NSMutableArray alloc] init];
	}
	return self;
}

- (void)dealloc
{
    
    _callId = nil;
	[_httpRequest release];
	[_emodel release];
    [_callBackRequestQueue release];
	[super dealloc];
}

- (void)callbackRequest:(NSString *)PhoneNumber showFromNum:(NSString*)fromNum showToNum:(NSString*)toNum onSuccess:(CallRequestBlock)success onFailed:(CallRequestBlock)failed
{
    //------------------------------------------------------------------------------------------------
    
    
    
    int nSnRandNum = rand()+[PhoneNumber integerValue];//为防止随机码重复故回拨时加随机码＋被叫手机号。
    NSString *strnSnRandNum = [NSString stringWithFormat:@"%.0f",fabs(nSnRandNum)];
    
    //callid
    NSString *createCallid  = [[SoftphoneManager instance]createCallidByClientNumber:[UserDefaultManager GetUserID]];
    
    _callId = createCallid;
    
    //请求参数名称，请求参数的值
    NSMutableDictionary *argu = [[[NSMutableDictionary alloc]init] autorelease];
    [argu setObject: PhoneNumber forKey:@"called"];
    [argu setObject: ucsBrandid forKey:@"brandid"];
    [argu setObject: ucsP forKey:@"p"];
    [argu setObject: ucsPV forKey:@"pv"];
    [argu setObject: ucsSecurityver forKey:@"securityver"];
    [argu setObject: strnSnRandNum forKey:@"sn"];
    [argu setObject: ucsU forKey:@"u"];
    [argu setObject: [UserDefaultManager GetclientNumber] forKey:@"uid"];
    [argu setObject: ucsV forKey:@"v"];
    [argu setObject: createCallid forKey:@"callid"];
    //    [argu setObject: PUBLIC_KEY_NEW forKey:@"zzPUBLIC_KEY_NEW"]; //用token 或者 clientPassword替换掉
    [argu setObject:[UserDefaultManager GetUserID] forKey:@"userid"];
    [argu setObject:[NSString stringWithFormat:@"%d",[UserDefaultManager GetUserAtype]] forKey:@"atype"];
    
    
    
    
    
    NSLog(@"NSMutableDictionary argu:%@",argu);
    
    NSString *tempstrSign = [NSString stringWithFormat:@"%@",@""];//[NSString stringWithFormat:@"%@%@%@%@%@%@%@%@%@%@%@%@",ucsBrandid,PhoneNumber,fromNum,ucsP,ucsPV,ucsSecurityver,strnSnRandNum,toNum,ucsU,[UserDefaultManager GetUserID],ucsV,PUBLIC_KEY_NEW];
    
    
    //     v2/callback?brandid=yzx&called=18576616761&fromsernum=18576616769&p=UCS&pv=pc&securityver=1&sn=4dcf5253-7225-4c77-b892-72c1970d433c&tosernum=18576616768&u=5&uid=77691000000228&v=2.10.0&sign=16284b5e043d929544c7fc73942c80db6fcb62e7
    
    //签名参数
    NSMutableArray *arraySign = [[NSMutableArray alloc] initWithObjects:@"called",@"brandid",@"p",@"pv",@"securityver",@"sn",@"u",@"uid",@"v",@"callid",@"userid",@"atype",nil];
    
    
    
    //     字段 &fromsernum &tosernum= 暂时不加入签名 add by kucky
    
    if (fromNum!=nil && [fromNum length]>0) {
        
        
        [arraySign addObject:@"fromsernum"];
        [argu setObject: fromNum forKey:@"fromsernum"];
    }
    
    if (toNum!=nil && [toNum length]>0) {
        [arraySign addObject:@"tosernum"];
        [argu setObject: toNum forKey:@"tosernum"];
        
    }
    
    //    NSLog(@"排序前sorted array:%@",arraySign);
    
    //签名参数需根据从a-z 进行排序
    [arraySign sortUsingSelector:@selector(compare:)];
    
    //    NSLog(@"排序后sorted array:%@",arraySign);
    
    //根据a-z 进行排序以后进行拼接
    for (NSString *arg in arraySign) {
        NSString * object = [argu objectForKey:arg];
        
        tempstrSign = [tempstrSign stringByAppendingString:[argu objectForKey:arg]];
        
        
    }
    
    
    tempstrSign = [tempstrSign stringByAppendingString:[UserDefaultManager GetIMSSID]];
    
    
    /*
     SDK签名生成方式：
     ➢	老开发者使用client的密码明文登录，登录成功后SDK保存client密码；
     sign=SHA1(排序后所有参数 + client密码)
     
     ➢	新开发者使用的token登录，登录成功后SDK保存token；
     sign=SHA1(排序后所有参数 + token)
     */
    
    
    
    
    NSLog(@"sorted array NSString:%@",[arraySign componentsJoinedByString:@""]);
    
    
    
    
    //签名
    NSString *strSign = [[[UxinUtility shareUtility] getShaString:tempstrSign] lowercaseString];
    
    
    
    
    NSMutableDictionary *itemDic = [NSMutableDictionary dictionary];
    
    [itemDic setObject:createCallid forKey:PhoneNumber];
    
    @synchronized(_callBackRequestQueue)
    {
        [_callBackRequestQueue addObject:itemDic];//将回拨请求添加到队列
    }
    
    
    _isEmodel = NO;
    
    NSString *tempurlString = nil;
    
    if ([UCSPublicFunc getIsUseTestServer] && [[UCSPublicFunc getIsUseTestServer] isEqualToString:@"TESTMODE"])//测试
    {
        tempurlString = [NSString stringWithFormat:UCS_URL_CALLBACK_TEST,ucsBrandid,strnSnRandNum,PhoneNumber,ucsSecurityver,[UserDefaultManager GetclientNumber],ucsP,ucsPV,ucsV,ucsU,createCallid,strSign,[UserDefaultManager GetUserID],[NSString stringWithFormat:@"%d",[UserDefaultManager GetUserAtype]]];//,createCallid
    }
    else if ([UCSPublicFunc getIsUseTestServer] && [[UCSPublicFunc getIsUseTestServer] isEqualToString:@"DEVMODE"])//DEV 模式
    {
        tempurlString = [NSString stringWithFormat:UCS_URL_CALLBACK_DEV,ucsBrandid,strnSnRandNum,PhoneNumber,ucsSecurityver,[UserDefaultManager GetclientNumber],ucsP,ucsPV,ucsV,ucsU,strSign,createCallid,[UserDefaultManager GetUserID],[NSString stringWithFormat:@"%d",[UserDefaultManager GetUserAtype]]];
    }
    else //正式
    {
        tempurlString = [NSString stringWithFormat:UCS_URL_CALLBACK,ucsBrandid,strnSnRandNum,PhoneNumber,ucsSecurityver,[UserDefaultManager GetclientNumber],ucsP,ucsPV,ucsV,ucsU,createCallid,strSign,[UserDefaultManager GetUserID],[NSString stringWithFormat:@"%d",[UserDefaultManager GetUserAtype]]];//,createCallid
    }
    
    
    
    
    
    if (fromNum!=nil && [fromNum length]>0) {
        
        tempurlString =  [tempurlString stringByAppendingString:[NSString stringWithFormat:@"%@%@",@"&fromsernum=",fromNum]];
    }
    
    if (toNum!=nil && [toNum length]>0) {
        tempurlString = [tempurlString stringByAppendingString:[NSString stringWithFormat:@"%@%@",@"&tosernum=",toNum]];
    }
    
    //     tempurlString = [tempurlString stringByAppendingString:[NSString stringWithFormat:@"%@%@",@"&sign=",strSign]];
    
    
    
    
    NSString *urlString = tempurlString;
    
    
    
    
    
    [_httpRequest setURLAndDelegate:urlString withDelegate:self];
    self.successBlock = success;
    self.failedBlock = failed;
}

-(NSString*)createSignBy:(NSString*)callid withPhoneNumber:(NSString*)PhoneNumber withSnRandNum:(NSString*)strSnRandNum
{
    
    
    
    
    //请求参数名称，请求参数的值
    NSMutableDictionary *argu = [[[NSMutableDictionary alloc]init] autorelease];
    
    [argu setObject: ucsBrandid forKey:@"brandid"];
    [argu setObject: PhoneNumber forKey:@"called"];
    [argu setObject:strSnRandNum forKey:@"sn"];
    [argu setObject: [UserDefaultManager GetclientNumber] forKey:@"uid"];
    [argu setObject: callid forKey:@"callid"];
    //    [argu setObject: PUBLIC_KEY_NEW forKey:@"zzPUBLIC_KEY_NEW"];
    [argu setObject:[UserDefaultManager GetUserID] forKey:@"userid"];
    [argu setObject:[NSString stringWithFormat:@"%d",[UserDefaultManager GetUserAtype]] forKey:@"atype"];
    
    
    
    NSLog(@"NSMutableDictionary argu:%@",argu);
    
    NSString *tempstrSign = [NSString stringWithFormat:@"%@",@""];
    
    //签名参数
    NSMutableArray *arraySign = [[NSMutableArray alloc] initWithObjects:@"called",@"brandid",@"sn",@"uid",@"callid",@"userid",@"atype",nil];
    
    
    
    
    //    NSLog(@"排序前sorted array:%@",arraySign);
    
    //签名参数需根据从a-z 进行排序
    [arraySign sortUsingSelector:@selector(compare:)];
    
    //    NSLog(@"排序后sorted array:%@",arraySign);
    
    //根据a-z 进行排序以后进行拼接
    for (NSString *arg in arraySign) {
        tempstrSign = [tempstrSign stringByAppendingString:[argu objectForKey:arg]];
    }
    
    tempstrSign = [tempstrSign stringByAppendingString:[UserDefaultManager GetIMSSID]];
    
    
    /*
     SDK签名生成方式：
     ➢	老开发者使用client的密码明文登录，登录成功后SDK保存client密码；
     sign=SHA1(排序后所有参数 + client密码)
     
     ➢	新开发者使用的token登录，登录成功后SDK保存token；
     sign=SHA1(排序后所有参数 + token)
     */
    
    NSLog(@"sorted array NSString:%@",[arraySign componentsJoinedByString:@""]);
    
    
    
    
    //签名
    NSString *strSign = [[[UxinUtility shareUtility] getShaString:tempstrSign] lowercaseString];
    
    
    return strSign;

}


- (void)cancelCallbackRequest:(NSString *)PhoneNumber onSuccess:(CallRequestBlock)success onFailed:(CallRequestBlock)failed
{
    _isEmodel = NO;
    
    
    
    
    
    //如果队列数大于1条则进行取消
    if(_callBackRequestQueue && [_callBackRequestQueue count]>0)
    {
        int time = 0;
        
        for(NSMutableDictionary* createCallid_dic in _callBackRequestQueue)
        {
            time++;
            
            int nSnRandNum = rand()+[PhoneNumber integerValue];//为防止随机码重复故回拨时加随机码＋被叫手机号。
            NSString *strSnRandNum = [NSString stringWithFormat:@"%.0f",fabs(nSnRandNum)];
            
            
            NSString *createCallid = [createCallid_dic objectForKey:PhoneNumber];
            
            if (createCallid == nil) {
                
                continue;
            }
            
            //签名
            NSString *strSign =  [self createSignBy:createCallid withPhoneNumber:PhoneNumber withSnRandNum:strSnRandNum];
            
            
            
            NSString *tempurlString = nil;
            
            if ([UCSPublicFunc getIsUseTestServer] && [[UCSPublicFunc getIsUseTestServer] isEqualToString:@"TESTMODE"])//测试
            {
                tempurlString = [NSString stringWithFormat:UCS_URL_CANCELCALLBACK_TEST,ucsBrandid,strSnRandNum,PhoneNumber,[UserDefaultManager GetclientNumber],createCallid,strSign,[UserDefaultManager GetUserID],[NSString stringWithFormat:@"%d",[UserDefaultManager GetUserAtype]]];
            }
            else if ([UCSPublicFunc getIsUseTestServer] && [[UCSPublicFunc getIsUseTestServer] isEqualToString:@"DEVMODE"])//DEV 模式
            {
                tempurlString = [NSString stringWithFormat:UCS_URL_CANCELCALLBACK_DEV,ucsBrandid,strSnRandNum,PhoneNumber,[UserDefaultManager GetclientNumber],createCallid,strSign,[UserDefaultManager GetUserID],[NSString stringWithFormat:@"%d",[UserDefaultManager GetUserAtype]]];
                
            }
            else //正式
            {
                tempurlString = [NSString stringWithFormat:UCS_URL_CANCELCALLBACK,ucsBrandid,strSnRandNum,PhoneNumber,[UserDefaultManager GetclientNumber],createCallid,strSign,[UserDefaultManager GetUserID],[NSString stringWithFormat:@"%d",[UserDefaultManager GetUserAtype]]];
            }
            
            
            
            NSString *urlString = tempurlString;
            
            
            
            [_httpRequest setURLAndDelegate:urlString withDelegate:self];
            
            sleep(1/time);
            
            [_httpRequest startAsyncRequest];
            
            
            
            
            
        }
        
        
        
        
        
        
        @synchronized(_callBackRequestQueue)
        {
            [_callBackRequestQueue removeAllObjects];//移除所有队列中的回拨请求
        }
        
    }
    
    
    
    
    
    
    
    
    
    self.successBlock = success;
    self.failedBlock = failed;
}

- (void)emodelReauest:(NSString *)cphone caller:(NSString *)cuid
{
    _isEmodel = YES;
	if (_emodel == nil) {
		_emodel = [[NSMutableDictionary alloc] initWithCapacity:20];
	}
    _emodel[@"brand_id"] = @"youxin";
    _emodel[@"calltype"] = @"p2p";
    _emodel[@"routetype"] = @"transit";
    _emodel[@"pv"] = @"ios";
    _emodel[@"sp_ver"] = [[SoftphoneManager instance] getSoftphoneVersion];
    if ([cuid isEqualToString:[UserDefaultManager GetclientNumber]]) {
    	if (cphone) _emodel[@"callee"] = cphone;
    	_emodel[@"caller_no"] = [UserDefaultManager GetUserMobile];
        _emodel[@"caller_uid"] = [UserDefaultManager GetclientNumber];
	    _emodel[@"R"] = @"offer";
    } else {
        _emodel[@"caller_uid"] = cuid;
        _emodel[@"callee"] = [UserDefaultManager GetUserMobile];
        [_emodel removeObjectForKey:@"caller_no"];
        _emodel[@"R"] = @"answer";
    }

    PhoneNetType nt = [PublicFunc GetCurrentPhoneNetType];
    if (nt == PNT_WIFI)
        _emodel[@"netmode"] = @"wifi";
    else if (nt == PNT_3G)
        _emodel[@"netmode"] = @"3G";
    else if (nt == PNT_4G)
        _emodel[@"netmode"] = @"4G";
    else
        _emodel[@"netmode"] = @"ethernet";
}

- (void)startAsyncRequest
{
    if (_isEmodel) {
        if ([self reportEmodel] == NO)
            return;
    }
	[_httpRequest startAsyncRequest];
}

- (void) onUCSHttpRequestDidFinishLoading:(UCSHttpRequest *) pUCSHttpRequest withData:(NSData *) data withResult:(int) nResult
{
	id json = [NSJSONSerialization JSONObjectWithData:data options:0 error:nil];
    
	if ([json isKindOfClass:[NSDictionary class]] && _successBlock) {
		_successBlock(json,_callId);
	} else {
        if (_failedBlock) _failedBlock(nil,nil);
    }
    _successBlock = _failedBlock = nil;
    
   
}

- (void) onUCSHttpRequestDidFailWithError:(UCSHttpRequest *) pUCSHttpRequest withError:(NSError *) error
{
    if (_failedBlock) {
        _failedBlock(nil,nil);
    }
    _successBlock = _failedBlock = nil;
}

- (BOOL)reportEmodel
{
//    NSDictionary *dic = [[SoftphoneManager instance] emodelValue];
//    if (dic == nil || [dic count] == 0) {
//        return NO;
//    }
//    [_emodel addEntriesFromDictionary:dic];
//    int sn = rand();
//    NSString *emodelString = [_emodel JSONRepresentation];
//    NSString *sign = [[UxinUtility shareUtility] md5String:[NSString stringWithFormat:@"%d%@%@", sn, emodelString, PUBLIC_KEY]];
//    NSString *urlString = [NSString stringWithFormat:URL_EMODEL_REPORT, sn, sign, emodelString];
//    NSString *url = [urlString stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
//
//    [_httpRequest setURLAndDelegate:url withDelegate:self];
    return YES;
    
}
@end
