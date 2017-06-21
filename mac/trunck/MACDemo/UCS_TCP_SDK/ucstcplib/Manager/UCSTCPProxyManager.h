//
//  UCSTCPProxyManager.h
//  Tcplib
//
//  Created by KCT on 16/2/29.
//  Copyright © 2017年 KCTMAC. All rights reserved.
//

#import <Foundation/Foundation.h>


typedef void(^ProxyBlock)(NSString *ip, BOOL end, NSError * error) ;


/**
 *  这个类用来请求proxy，管理proxy
 */
@interface UCSTCPProxyManager : NSObject

@property (nonatomic, assign)BOOL isEnd;  //判断当前ip是不是取完了。可以用来判断登陆的时候返回给调用类；也可以用来判断重连的时候，是否需要向服务器请求新的ip列表



+(instancetype)sharedInstance;

/**
*  获取proxy
*  @param isLoged 登陆状态
*  @param proxy   回调proxy的block
*/
- (void)loadProxyWithHttp:(BOOL)isLoged
                       iP:(ProxyBlock) proxy;


/**
 *  连接成功后,交换ip地址
 */
- (void)exchangeProxy;

/**
 *  还原现场。便于下一次发起请求
 */
- (void)revert;


@end
