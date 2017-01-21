//
//  AddressBook.h
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@interface AddressBook : NSObject

#pragma mark- 通讯录信息
@property (nonatomic, copy) NSString *name;
@property (nonatomic, strong) UIImage *head;
@property (nonatomic, strong) NSMutableDictionary *phones;
@property (nonatomic, strong) NSMutableDictionary *others;

/**
 *  名字头字母
 */
@property (nonatomic, readonly) NSString *firstLetter;

@end
