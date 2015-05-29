//
//  AddressInfo.m
//  ROM Explorer
//
//  Created by Steven Frank on 5/28/15.
//  Copyright (c) 2015 Steven Frank. All rights reserved.
//

#import "AddressInfo.h"

@implementation AddressInfo

- (NSString *)description
{
	return [NSString stringWithFormat:@"%@ %@ %08x %08x", self.symbol, self.demangledCPPSymbol, self.address, self.value];
}

@end
