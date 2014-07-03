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


