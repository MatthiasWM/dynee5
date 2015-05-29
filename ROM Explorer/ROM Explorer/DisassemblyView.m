//
//  DisassemblyView.m
//  ROM Explorer
//
//  Created by Steven Frank on 5/28/15.
//  Copyright (c) 2015 Steven Frank. All rights reserved.
//

#import "DisassemblyView.h"

@implementation DisassemblyView

- (void)drawRect:(NSRect)dirtyRect
{
	[super drawRect:dirtyRect];
	
	[[NSColor blackColor] set];
	NSRectFill(dirtyRect);
}

@end
