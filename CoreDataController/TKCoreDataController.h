//
//  CoreDataController.h
//  CoreDataController
//
//  Created by Thomas Kollbach on 22.02.14.
//  Copyright (c) 2014 nxtbgthng GmbH. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreData/CoreData.h>


@interface TKCoreDataController : NSObject

@property (nonatomic, readonly) NSManagedObjectContext *mainObjectContext;
@property (nonatomic, readonly) NSManagedObjectModel *managedObjectModel;
@property (nonatomic, readonly) NSPersistentStoreCoordinator *persistentStoreCoordinator;

#pragma mark Lifecycle

- (instancetype)initWithModel:(NSManagedObjectModel *)model;

#pragma mark Core Data Persistent Store Managment

- (BOOL)hasPersistentStore;

- (BOOL)isMigrationRequiredForAddingStoreAtURL:(NSURL *)persistentStoreURL
                                         error:(NSError **)migrationCheckError;

- (NSPersistentStore *)addPersistentStoreAtURL:(NSURL *)persistentStoreURLOrNil
                             configurationName:(NSString *)configurationNameOrNil
                                       options:(NSDictionary *)optionsOrNil
                                         error:(NSError **)error;

- (NSPersistentStore *)removePersistentStoreAtURL:(NSURL *)persistentStoreURL
                                            error:(NSError **)error;


#pragma mark Core Data Persistent Store Managment - Async

- (void)addPersistentStoreAtURL:(NSURL *)persistentStoreURL
              withConfiguration:(NSString *)configurationNameOrNil
                          queue:(dispatch_queue_t)queue
               migrationHandler:(void(^)(BOOL migrationRequired, NSError *error))migrationHandler
                  resultHandler:(void(^)(NSPersistentStore *store, NSError *error))resultHandler;

- (void)removePersistentStoreAtURL:(NSURL *)persistentStoreURL
                             queue:(dispatch_queue_t)queue
                     resultHandler:(void(^)(NSPersistentStore *store, NSError *error))resultHandler;


@end
