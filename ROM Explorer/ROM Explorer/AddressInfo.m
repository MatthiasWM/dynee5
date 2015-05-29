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
	
	output.address = self.address;
	output.assembly = self.assembly;
	output.demangledCPPSymbol = self.demangledCPPSymbol;
	output.fourChar = self.fourChar;
	output.symbol = self.symbol;
	output.type = self.type;
	output.value = self.value;
	
	return output;
}

- (NSString *)description
{
	return [NSString stringWithFormat:@"%@ %@ %08x %08x", self.symbol, self.demangledCPPSymbol, self.address, self.value];
}

@end
