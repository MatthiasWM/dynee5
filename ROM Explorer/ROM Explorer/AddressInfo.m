//
//  AddressInfo.m
//  ROM Explorer
//
//  Created by Steven Frank on 5/28/15.
//  Copyright (c) 2015 Steven Frank. All rights reserved.
//

#import "AddressInfo.h"

@implementation AddressInfo

- (instancetype)copyWithZone:(NSZone *)zone
{
	AddressInfo *output = [[AddressInfo alloc] init];
	
	output.symbol = self.symbol;
	output.value = self.value;
	output.demangledCPPSymbol = self.demangledCPPSymbol;
	output.address = self.address;
	
	return output;
}

- (NSString *)description
{
	return [NSString stringWithFormat:@"%@ %@ %08x %08x", self.symbol, self.demangledCPPSymbol, self.address, self.value];
}

@end
