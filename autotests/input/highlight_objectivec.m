#import <Fondation/Fondation.h>

// Simple class
@interface MyClass : Object {
    // instance variables
    int myVariable
}
// Class methods
+ myClassMethod;
+ (int)myClassMethod2;
+ (int)myClassMethod3:(NSString *)parameter;

// Instance methods
- (int)myInstanceMethod:(NSString *)text;
- (NSString *)myInstanceMethod2:(int)parameter
                              withText:(NSString *)text;
@end

@implementation MyClass
+ (int)classMethod {
  return [self myVariable];
}
- (NSString *)instanceMethod {
    NSString *string = [[NSString alloc]initWithUTF8String:"Good string ©"];
    NSLog(@"String:%@",string);
    return string;
}
@end


// Protocol
@protocol MyKeyProtocol
- (void)lock;
- (void)unlock;
@end

// Class that implements a protocol
@interface MyClass2 : Object <MyKeyProtocol> {

}
// Protocol methods
- (void)lock;
- (void)unlock;
@end
