//
//  AddressBook.m
//
//
#import "KCPinyinHelper.h"
#import "AddressBook.h"

@implementation AddressBook
- (id)init
{
    self = [super init];
    if (self) {
        self.phones = [NSMutableDictionary dictionary];
        self.others = [NSMutableDictionary dictionary];
    }
    return self;
}

- (void)setName:(NSString *)name
{
    _name = name;
    _firstLetter = [[KCPinyinHelper quickConvert:name] uppercaseString];
}


- (UIImage *)head{
    return (_head)?(_head):([UIImage imageNamed:@"默认头像"]);
}

@end
