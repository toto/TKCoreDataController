# TKCoreDataController

Core Data Controller to simplify setting up a Core Data stack. 

## Installation with CocoaPods

Add 

```
pod 'TKCoreDataController'
```

to your Podfile.

## Managed object contexts

For convinience `TKCoreDataController` has `mainObjectContext` set up in `NSMainQueueConcurrencyType` on it's persistent store coordinator.

If you need more contexts adding should be done like this:

```
TKCoreDataController *controller = // ...
NSMangedObjectContext *context = [[NSMangedObjectContext alloc] initWithConcurrencyType:NSPrivateQueueConcurrencyType];
[context performBlock:^{
	context.persistentStoreCoordinator = controller.persistentStoreCoordinator;
}];

```

## Migration handling

As migrations can possibly run a long time you should be careful when adding them, to not block the UI. This is especially problematic during application startup, as iOS will kill your app if it takes to long to start.

Typically the flow looks like this (error handling omnited):

```
TKCoreDataController *controller = // ...
NSURL *storeURL = // ...

if ([controller  isMigrationRequiredForAddingStoreAtURL:storeURL error:NULL]) {
	[controller addPersistentStoreAtURL:storeURL
					  withConfiguration:nil
		   					    options:nil
		   					      queue:NULL
		   			   migrationHandler:^(BOOL migrationRequired, NSError *error) {
		   			     if (migrationRequired) {
		   			     	// show migration URL
		   			     }
		   			   }
		   			   resultHandler:^(NSPersistentStore *store, NSError *error){
			   			   // hide migration UI, show app UI
		   			   }];
} else {
	[controller addPersistentStoreAtURL:storeURL
	                  configurationName:nil
	                            options:nil
  	                              error:NULL];
}
```

# BSD License

Copyright © 2014, Thomas Kollbach

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
Neither the name of TKCoreDataController nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

