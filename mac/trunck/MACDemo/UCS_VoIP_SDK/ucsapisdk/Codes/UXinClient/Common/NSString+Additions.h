/**
 * @brief 为NSString添加自定义方法。
 */

@interface NSString (Acani)

/**
 * @brief 参数 [NSCharacterSet whitespaceAndNewlineCharacterSet]  删除字符串左侧的空格和换行符 
 *        or 参数 [NSCharacterSet whitespaceCharacterSet]         不删除换左侧行符和新行，只去除空格
 */
- (NSString *)stringByTrimmingLeadingCharactersInSet:(NSCharacterSet *)characterSet;

/**
 * @brief  删除字符串左侧的空格和换行符
 */
- (NSString *)stringByTrimmingLeadingWhitespaceAndNewlineCharacters;

/**
 * @brief 参数 [NSCharacterSet whitespaceAndNewlineCharacterSet]  删除字符串右侧的空格和换行符 
 *        or 参数 [NSCharacterSet whitespaceCharacterSet]         不删除换右侧行符和新行，只去除空格
 */
- (NSString *)stringByTrimmingTrailingCharactersInSet:(NSCharacterSet *)characterSet;
/**
 * @brief  删除字符串右侧的空格和换行符
 */
- (NSString *)stringByTrimmingTrailingWhitespaceAndNewlineCharacters;
/**
 * @brief  是否包含字串
 */
- (BOOL) isContainString:(NSString *) str;

@end
