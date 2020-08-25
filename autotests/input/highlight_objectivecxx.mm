#include <cstdio>

#import <Fondation/Fondation.h>

void cxx_method(const std::string &myText)
{
	NSString *string = [NSString stringWithCString:myText.c_str()
                                 		  encoding:[NSString defaultCStringEncoding]];
	NSLog(@"String:%@",string);
}
