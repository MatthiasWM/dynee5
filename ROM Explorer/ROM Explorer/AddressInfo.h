//
//  AddressInfo.h
//  ROM Explorer
//
//  Created by Steven Frank on 5/28/15.
//  Copyright (c) 2015 Steven Frank. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface AddressInfo : NSObject <NSCopying>

@property (assign) UInt32 address;
@property (copy) NSString *assembly;
@property (copy) NSString *demangledCPPSymbol;
@property (copy) NSString *fourChar;
@property (copy) NSString *symbol;
@property (assign) UInt32 type;
@property (assign) UInt32 value;

@end
