//
//  AddressBookManager.m
//

#import "KCPinyinHelper.h"
#import "AddressBookManager.h"
//#import "RegexKitLite.h"
#import <UIKit/UIKit.h>
#import "KCTVOIPViewEngine.h"
#import "ClientConvert.h"


BOOL accessAddressBook(ABAddressBookRef addressBook)
{
    __block BOOL accessGranted = NO;
    if (ABAddressBookRequestAccessWithCompletion != NULL)
    {
        dispatch_semaphore_t sema = dispatch_semaphore_create(0);
        ABAddressBookRequestAccessWithCompletion(addressBook, ^(bool granted, CFErrorRef error)
                                                 {
                                                     accessGranted = granted;
                                                     dispatch_semaphore_signal(sema);
                                                 });
        dispatch_semaphore_wait(sema, DISPATCH_TIME_FOREVER);
        dispatch_release(sema);
    }
    else
        accessGranted = YES;
    return accessGranted;
}

@interface AddressBookManager ()

@property (nonatomic, strong) NSMutableArray *allContactArray;
@property (nonatomic, strong) NSMutableDictionary *allContactDictionary;
@end

@implementation NSString (Regex)

- (BOOL)isMobileNumber
{
    //modify by wenqinglin
    return YES;
    //return [self isMatchedByRegex:@"^1\\d{10}$"];
}

@end

@implementation AddressBookManager
SYNTHESIZE_SINGLETON_FOR_CLASS(AddressBookManager);

- (id)init
{
    self = [super init];
    if (self) {

    }
    return self;
}

- (void)dealloc
{
    [self setAllContactArray:nil];
    [self setAllContactDictionary:nil];
    [super dealloc];
}

- (void)clearAllData
{
    //清除缓存  即使修改了头像，头像链接地址任然不变，如果地址不变会读取缓存，一直都不会重新下载,这里进行缓存清理。
    
    self.allContactArray = nil;
    self.allContactDictionary = nil;
}

//读取本地通讯录此处返回数据已经存在VoIP信息
- (NSMutableArray *)allContacts
{
    if (self.allContactArray) {
        return self.allContactArray;
    }
    
    NSMutableArray *listArray = [NSMutableArray array];
    ABAddressBookRef addressBook = nil;
    if ([[UIDevice currentDevice].systemVersion floatValue] >= 6.0){
        addressBook = ABAddressBookCreateWithOptions(NULL, NULL);
        //等待同意后向下执行
        dispatch_semaphore_t sema = dispatch_semaphore_create(0);
        ABAddressBookRequestAccessWithCompletion(addressBook, ^(bool granted, CFErrorRef error)
                                                 {
                                                     dispatch_semaphore_signal(sema);
                                                 });
        dispatch_semaphore_wait(sema, DISPATCH_TIME_FOREVER); 
        dispatch_release(sema); 
    }else{
        addressBook = ABAddressBookCreate(); 
    }
    
    NSArray *array = (__bridge NSArray *)ABAddressBookCopyArrayOfAllPeople(addressBook);
    if(array == nil){
        return nil;
    }
    for (id friend in array) {
        AddressBook *book = [[[AddressBook alloc] init] autorelease];
        // 用户名
        NSString *firstName = (__bridge NSString *)ABRecordCopyValue((__bridge ABRecordRef)(friend), kABPersonFirstNameProperty);
        NSString *lastName = (__bridge NSString *)ABRecordCopyValue((__bridge ABRecordRef)(friend), kABPersonLastNameProperty);
        if (KCNSSTRING_ISEMPTY(lastName)) {
            book.name = firstName;
        }else{
            if (KCNSSTRING_ISEMPTY(firstName)) {
                book.name = lastName;
            }else{
                book.name = [lastName stringByAppendingString:firstName];
            }
        }
        [firstName release];
        [lastName release];
        // 用户名是否为空
        if (KCNSSTRING_ISEMPTY(book.name)) {
            continue;
        }
        // 用户头像
        CFDataRef dataRef = ABPersonCopyImageData((__bridge ABRecordRef)(friend));
        if (dataRef) {
            book.head = [UIImage imageWithData:(__bridge NSData *)dataRef];
            CFRelease(dataRef);
        }else{
            book.head = [UIImage imageNamed:@"friend_default_head"];
        }
        //读取电话多值
        ABMultiValueRef phone = ABRecordCopyValue(friend, kABPersonPhoneProperty);
        for (int k = 0; k<ABMultiValueGetCount(phone); k++)
        {
            NSString * phoneNO = nil;
            NSString * phoneTitle = nil;
            CFStringRef cfphoneNO = ABMultiValueCopyValueAtIndex(phone, k);
            if (cfphoneNO) {
                phoneNO = [(NSString*)cfphoneNO stringByReplacingOccurrencesOfString:@"-" withString:@""];
                CFRelease(cfphoneNO);
            }
            
            CFStringRef cfphoneTitle = ABMultiValueCopyLabelAtIndex(phone, k);
            if (cfphoneTitle) {
                CFStringRef loctitle = ABAddressBookCopyLocalizedLabel(cfphoneTitle);
                if (loctitle) {
                    phoneTitle = [NSString stringWithString:(NSString*)loctitle];
                    CFRelease(loctitle);
                }
                CFRelease(cfphoneTitle);
            }else{
                phoneTitle = @"电话";
            }
            if (KCNSSTRING_ISEMPTY(phoneNO) || KCNSSTRING_ISEMPTY(phoneTitle)) {
                continue;
            }
            
            //sinldoCode 2015.01.25 不规则电话处理一下
            phoneNO = [phoneNO stringByReplacingOccurrencesOfString:@" " withString:@""];
            phoneNO = [phoneNO stringByReplacingOccurrencesOfString:@"-" withString:@""];
            phoneNO = [phoneNO stringByReplacingOccurrencesOfString:@"+86" withString:@""];
            phoneNO=[phoneNO stringByReplacingOccurrencesOfString:@"+086" withString:@""];
            // 去掉两端的空格
            phoneNO=[phoneNO stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
            phoneNO = [phoneNO stringByReplacingOccurrencesOfString:@"+" withString:@""];
            phoneNO = [phoneNO stringByReplacingOccurrencesOfString:@"=" withString:@""];
            phoneNO = [phoneNO stringByReplacingOccurrencesOfString:@"*" withString:@""];
            phoneNO = [phoneNO stringByReplacingOccurrencesOfString:@"," withString:@""];
            phoneNO = [phoneNO stringByReplacingOccurrencesOfString:@";" withString:@""];
            phoneNO = [phoneNO stringByReplacingOccurrencesOfString:@" " withString:@""];
            NSCharacterSet *doNotWant = [NSCharacterSet characterSetWithCharactersInString:@",;[]{}（#%-*+=_）\\|~(＜＞$%^&*)_+ "];
            phoneNO = [[phoneNO componentsSeparatedByCharactersInSet: doNotWant]componentsJoinedByString: @""];
            //end sinldoCode
            if (![phoneNO isMobileNumber]) {
                continue;
            }
            [book.phones setObject:phoneNO forKey:phoneTitle];
        }
        CFRelease(phone);
        if (book.phones.count <= 0) {
            continue;
        }
        // 手机号码
        [listArray addObject:book];
    }
    CFRelease(addressBook);
    self.allContactArray = listArray;
    [array release];
    
    
    /**
     @author WLS, 15-12-19 11:12:04
     
     当获取到通讯录之后 设置给Voip。
     */
    [[KCTVOIPViewEngine getInstance].addressListArray removeAllObjects];
    [KCTVOIPViewEngine getInstance].addressListArray = [NSMutableArray arrayWithArray:listArray];
    
    
    return listArray;
}



//增加账号而修改的

- (AddressBook*)checkAddressBook:(NSString*)phone
{
    NSMutableArray *arr = [self allContacts];
    
    //加3个账号判断
    NSMutableArray *tempArray = [NSMutableArray array];
    NSArray * testArray = [[[ClientConvert alloc] init] contactTestArray];
    [tempArray addObjectsFromArray:testArray];
    [tempArray addObjectsFromArray:arr];

    
    for (int i = 0; i < tempArray.count; i++) {
        AddressBook* address = [tempArray objectAtIndex:i];
        NSArray* phones = [address.phones allValues];
        for (int k = 0; k < phones.count; k++) {
            
            NSString *tePhone = [phones objectAtIndex:k];
            if ([tePhone isEqualToString:phone]) {
                return address;
                break;
            }
        }
    }
    return nil;
}




//- (AddressBook*)checkAddressBook:(NSString*)phone
//{
//    NSMutableArray *arr = [self allContacts];
//    
//    for (int i = 0; i < arr.count; i++) {
//        AddressBook* address = [arr objectAtIndex:i];
//        NSArray* phones = [address.phones allValues];
//        for (int k = 0; k < phones.count; k++) {
//            
//            NSString *tePhone = [phones objectAtIndex:k];
//            if ([tePhone isEqualToString:phone]) {
//                return address;
//                break;
//            }
//        }
//    }
//    return nil;
//}

- (NSMutableDictionary *)allContactsBySorted
{
    if (self.allContactDictionary) {
        return self.allContactDictionary;
    }
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    NSMutableArray *array = [self allContacts];

    for (AddressBook *book in array) {
        NSMutableArray *subArray = [dict objectForKey:book.firstLetter];
        if (!subArray) {
            subArray = [NSMutableArray array];
            [dict setObject:subArray forKey:book.firstLetter];
        }
        [subArray addObject:book];
    }
    self.allContactDictionary = dict;
    
    return dict;
}

-(NSMutableDictionary *)NewallContactsBySorted{

    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    NSMutableArray *array = [NSMutableArray arrayWithArray:[self allContacts]];

    for (AddressBook *book in array) {
        NSMutableArray *subArray = [dict objectForKey:book.firstLetter];
        if (!subArray) {
            subArray = [NSMutableArray array];
            [dict setObject:subArray forKey:book.firstLetter];
        }
        [subArray addObject:book];
    }
    self.allContactDictionary = dict;
    
    return dict;
}
@end
