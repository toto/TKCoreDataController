//
//  CoreDataController.h
//  CoreDataController
//
//  Created by Thomas Kollbach on 22.02.14.
//  Copyright (c) 2014 Thomas Kollbach. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreData/CoreData.h>


@interface TKCoreDataController : NSObject

NS_ASSUME_NONNULL_BEGIN

@property (nonatomic, readonly) NSManagedObjectContext *mainObjectContext;
@property (nonatomic, readonly) NSManagedObjectModel *managedObjectModel;
@property (nonatomic, readonly) NSPersistentStoreCoordinator *persistentStoreCoordinator;

#pragma mark Lifecycle
/*!
 @abstract Initalize TKCoreDataController with an explicit model object.
 @param model The managed object model. This is required, passing nil will cause an exception.
 @discussion If you use -[TKCoreDataController init] it will search the main bundle for 
 a Core Data model file. @see +[NSManagedObjectModel mergedModelFromBundles:]
 @return A new instance of TKCoreDataController

 */
- (instancetype)initWithModel:(NSManagedObjectModel *)model;

#pragma mark Core Data Persistent Store Managment

/*!
 @abstract Returns YES if the persistentStoreCoordinator has at least one persistent store.
 Otherwise returns NO. Can be Key-Value-Observed.
 */
@property (nonatomic, readonly) BOOL hasPersistentStore;

/*!
 @abstract Checks if a migration is required for a persistent store file.
 @discussion An error can ocurr, e.g. if the file cannot be opened, is corrupted or is no
             Core Data compatible persistent store, etc.
 @return YES if a migration is required, otherwise NO. If an error occurs NO is returned as well.
 @param persistentStoreURL The URL of the persistent store file that should be checked.
 @param migrationCheckError Error parameter
 */
- (BOOL)isMigrationRequiredForAddingStoreAtURL:(NSURL *)persistentStoreURL
                                         error:(NSError ** __nullable)migrationCheckError;

/*!
 @abstract Syncronously adds a persistent store on the current thread.
 @discussion If no URL is given, an in-memory store is added. Otherwise a SQLite store is used. 
 Should a store be present at the URL automatic migration will be performed. Note that his happens
 in a synchronous manner on the calling thread. Therefore you should check if a migration is required
 first.
 By default NSPersistentStoreTimeoutOption = 5.0, NSMigratePersistentStoresAutomaticallyOption = YES
 and NSInferMappingModelAutomaticallyOption = YES will be passed as options. 
 You can change the behaviour by passing your own options.  If you pass nil or do not overwrite an option listed above the default will be used.
 
 @see isMigrationRequiredForAddingStoreAtURL:error:.
 @see -[NSPersistentStoreCoordinator addPersistentStoreWithType:configuration:URL:options:error:]
 @see addPersistentStoreAtURL:withConfiguration:queue:migrationHandler:resultHandler:
 
 @return The newly added store, if successfull. Nil otherwise.
 @param persistentStoreURLOrNil If a SQLite file should be added an URL to the file. Nil if an 
        in-memory store should be added.
 @param configurationNameOrNil An optional name of a configuration name.
 @param options Options that will be used when adding the persistent store.
 @param error An optional error parameter.
 */
- (NSPersistentStore * __nullable)addPersistentStoreAtURL:(NSURL * __nullable)persistentStoreURLOrNil
                                        configurationName:(NSString * __nullable)configurationNameOrNil
                                                  options:(NSDictionary * __nullable)optionsOrNil
                                                    error:(NSError ** __nullable)error;

/*!
 @abstract Removes a store from the persistent store coordinator
 @discussion Pass nil for the URL to remove a in-memory store. Call it multiple times with nil to 
 remove many in-memory stores. To remove a SQLite store pass in a URL.
 Note that his happens in a synchronous manner on the calling thread.
 @return A reference to the removed store, is one was removed.
 @param persistentStoreURL The URL of the persistent store file that should be checked.
 @param migrationCheckError Error parameter
 @see removePersistentStoreAtURL:queue:resultHandler:
 */
- (NSPersistentStore * __nullable)removePersistentStoreAtURL:(NSURL * __nullable)persistentStoreURL
                                                       error:(NSError ** __nullable)error;


#pragma mark Core Data Persistent Store Managment - Async

/*!
 @abstract Asyncronously adds a persistent store
 @discussion Adding (and a possible migration) is performed on a private queue of the TKCoreDataController.
 If migration handler is given it is called on the queue passed (or the main queue if queue is nil). 
 It is alwyas called, even if no migration is required.
 The result handler is also called on the passed queue (or the main queue if queue is nil) and passed a 
 reference to the added store or an error.
 @param persistentStoreURL persistentStoreURLOrNil If a SQLite file should be added an URL to the file. Nil if an
 in-memory store should be added.
 @param configurationNameOrNil An optional name of a configuration name.
 @param options Options that will be used when adding the persistent store. 
 @param queue The queue the passed handlers should be called on. If nil is passed handers will be called on the main queue.
 @param migrationHandler Block to be called before the store is added. The block is passed a bool that indicates if a migration is required or not.
 @param resultHandler Block to be called after the store has been added (or adding has failed)
 
 */
- (void)addPersistentStoreAtURL:(NSURL * __nullable)persistentStoreURL
              withConfiguration:(NSString * __nullable)configurationNameOrNil
                        options:(NSDictionary * __nullable)options
                          queue:(dispatch_queue_t __nullable)queue
               migrationHandler:(void(^ __nullable)(BOOL, NSError * __nullable))migrationHandler
                  resultHandler:(void(^ __nullable)(NSPersistentStore * __nullable, NSError * __nullable))resultHandler;

/*!
 @abstract Asyncronously removes a persistent store
 @discussion Removing is performed on a private queue of the TKCoreDataController.
 The result handler is also called on the passed queue (or the main queue if queue is nil) and passed a
 reference to the added store or an error.
 @param persistentStoreURL persistentStoreURLOrNil If a file store should be removed. Nil if an
 in-memory store should be removed.
 @param queue The queue the passed handlers should be called on. If nil is passed handers will be called on the main queue.
 @param resultHandler Block to be called after the store has been removed (or adding has failed)
 
 */
- (void)removePersistentStoreAtURL:(NSURL * __nullable)persistentStoreURL
                             queue:(dispatch_queue_t)queue
                     resultHandler:(void(^)(NSPersistentStore * __nullable, NSError * __nullable))resultHandler;

NS_ASSUME_NONNULL_END

@end
