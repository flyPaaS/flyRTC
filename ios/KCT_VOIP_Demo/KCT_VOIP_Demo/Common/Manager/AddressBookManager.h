//
//  AddressBookManager.h
//

#import "AddressBook.h"
#import "SynthesizeSingleton.h"
#import <Foundation/Foundation.h>
#import <AddressBook/ABPerson.h>
#import <AddressBook/ABMultiValue.h>
#import <AddressBook/ABAddressBook.h>

#define KCNSSTRING_ISEMPTY(str) (str == nil || [str isEqual:[NSNull null]] || str.length <= 0)

@interface AddressBookManager : NSObject

/**
 *  清空所有的数据
 */
- (void)clearAllData;

/**
 *  未排序的联系人列表
 *
 *  @return
 */
- (NSMutableArray *)allContacts;

/**
 *  按照字母A-Z排序的列表
 *
 *  @return
 */
- (NSMutableDictionary *)allContactsBySorted;
//
-(NSMutableDictionary *)NewallContactsBySorted;

- (AddressBook*)checkAddressBook:(NSString *)phone;

SYNTHESIZE_SINGLETON_FOR_CLASS_HEADER(AddressBookManager);
@end
