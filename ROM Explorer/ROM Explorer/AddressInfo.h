//
//  AddressInfo.h
//  ROM Explorer
//
//  Created by Steven Frank on 5/28/15.
//  Copyright (c) 2015 Steven Frank. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface AddressInfo : NSObject

@property (copy) NSString *symbol;
@property (copy) NSString *demangledCPPSymbol;
@property (assign) UInt32 address;
@property (assign) UInt32 value;

@end
