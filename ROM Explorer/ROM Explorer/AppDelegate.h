//
//  AppDelegate.h
//  ROM Explorer
//
//  Created by Steven Frank on 5/28/15.
//  Copyright (c) 2015 Steven Frank. All rights reserved.
//

#import <Cocoa/Cocoa.h>

extern int disarm_no_comments(char *dst, unsigned int addr, unsigned int cmd);

@interface AppDelegate : NSObject <NSApplicationDelegate, NSTableViewDelegate, NSTableViewDataSource>

@property (assign) IBOutlet NSTableView *tableView;

@end

