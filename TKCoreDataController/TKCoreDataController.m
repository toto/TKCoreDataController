//
//  CoreDataController.m
//  CoreDataController
//
//  Created by Thomas Kollbach on 22.02.14.
//  Copyright (c) 2014 Thomas Kollbach. All rights reserved.
//

#import "TKCoreDataController.h"

@interface TKCoreDataController ()

@property (nonatomic, readwrite) dispatch_queue_t peristentStoreAddRemoveQueue;

#pragma mark Core Data Stack

@property (nonatomic, readwrite) NSManagedObjectContext *mainObjectContext;
@property (nonatomic, readwrite) NSManagedObjectModel *managedObjectModel;
@property (nonatomic, readwrite) NSPersistentStoreCoordinator *persistentStoreCoordinator;

@property (nonatomic, readwrite) BOOL hasPersistentStore;

@end

@implementation TKCoreDataController

- (id)init;
{
    NSManagedObjectModel *model = [NSManagedObjectModel mergedModelFromBundles:@[ [NSBundle mainBundle] ]];
    NSAssert(model != nil, @"No managed object model could be found in main bundle. Use -initWithModel: to specify one or check your target configuration.");

    return [self initWithModel:model];
}

- (instancetype)initWithModel:(NSManagedObjectModel *)model;
{
    NSParameterAssert(model);
    self = [super init];

    if (self) {
        _peristentStoreAddRemoveQueue = dispatch_queue_create("ch.kollba.TKCoreDataController.persistentStore", DISPATCH_QUEUE_SERIAL);
        _managedObjectModel = model;
        _persistentStoreCoordinator = [[NSPersistentStoreCoordinator alloc] initWithManagedObjectModel:_managedObjectModel];
        _mainObjectContext = [[NSManagedObjectContext alloc] initWithConcurrencyType:NSMainQueueConcurrencyType];
        [_mainObjectContext performBlockAndWait:^{
            _mainObjectContext.persistentStoreCoordinator = _persistentStoreCoordinator;
        }];
        [_persistentStoreCoordinator addObserver:self
                                      forKeyPath:@"persistentStores"
                                         options:NSKeyValueObservingOptionNew
                                         context:NULL];
    }

    return self;
}

- (void)dealloc
{
    [_persistentStoreCoordinator removeObserver:self
                                     forKeyPath:@"persistentStores"];
}

#pragma mark Peristent Store

- (BOOL)isMigrationRequiredForAddingStoreAtURL:(NSURL *)persistentStoreURL error:(NSError **)migrationCheckError;
{
    // first check if a migration is required
    BOOL migrationNeeded = NO;

    if ([[NSFileManager defaultManager] fileExistsAtPath:persistentStoreURL.path]) {
        NSDictionary *sourceMetadata = [NSPersistentStoreCoordinator metadataForPersistentStoreOfType:NSSQLiteStoreType
                                                                                                  URL:persistentStoreURL
                                                                                                error:migrationCheckError];
        if (*migrationCheckError) {
            return NO;
        }
        NSManagedObjectModel *destinationModel = [self.persistentStoreCoordinator managedObjectModel];
        migrationNeeded = ![destinationModel isConfiguration:nil compatibleWithStoreMetadata:sourceMetadata];
    }

    return migrationNeeded;
}

- (NSPersistentStore *)addPersistentStoreAtURL:(NSURL *)persistentStoreURL
                             configurationName:(NSString *)configurationNameOrNil
                                       options:(NSDictionary *)options
                                         error:(NSError **)error;
{
    NSMutableDictionary *finalOptions = [NSMutableDictionary dictionaryWithDictionary:@{
                                                        NSPersistentStoreTimeoutOption: @(5.0),
                                                        NSMigratePersistentStoresAutomaticallyOption: @YES,
                                                        NSInferMappingModelAutomaticallyOption: @YES }];

    if (options) {
        [finalOptions addEntriesFromDictionary:options];
    }
    NSPersistentStore *store = [self.persistentStoreCoordinator addPersistentStoreWithType:persistentStoreURL ? NSSQLiteStoreType : NSInMemoryStoreType
                                                                             configuration:configurationNameOrNil
                                                                                       URL:persistentStoreURL
                                                                                   options:[finalOptions copy]
                                                                                     error:error];
    
    return store;
}

- (NSPersistentStore *)removePersistentStoreAtURL:(NSURL *)persistentStoreURL error:(NSError **)error
{
    NSPersistentStore *store = nil;
    if (persistentStoreURL) {
        store = [self.persistentStoreCoordinator persistentStoreForURL:persistentStoreURL];
    } else {
        for (NSPersistentStore *persistentStore in self.persistentStoreCoordinator.persistentStores) {
            if ([persistentStore.type isEqualToString:NSInMemoryStoreType]) {
                store = persistentStore;
                break;
            }
        }
    }

    if ([self.persistentStoreCoordinator removePersistentStore:store error:error]) {
        return store;
    } else {
        return nil;
    }
}

- (void)addPersistentStoreAtURL:(NSURL *)persistentStoreURL
              withConfiguration:(NSString *)configurationNameOrNil
                        options:(NSDictionary *)options
                          queue:(dispatch_queue_t)queue
               migrationHandler:(void(^)(BOOL migrationRequired, NSError *error))migrationHandler
                  resultHandler:(void(^)(NSPersistentStore *store, NSError *error))resultHandler;
{

    if (persistentStoreURL) {
        NSError *migrationCheckError = nil;
        BOOL migrationNeeded = [self isMigrationRequiredForAddingStoreAtURL:persistentStoreURL
                                                                      error:&migrationCheckError];

        dispatch_async(queue ?: dispatch_get_main_queue(), ^{
            if (migrationHandler != NULL) {
                migrationHandler(migrationNeeded, migrationCheckError);
            }
        });
    }

    dispatch_async(self.peristentStoreAddRemoveQueue, ^{
        NSError *error = nil;
        
        NSPersistentStore *store = [self addPersistentStoreAtURL:persistentStoreURL
                                               configurationName:configurationNameOrNil
                                                         options:options
                                                           error:&error];
        
        if (resultHandler) {
            dispatch_async(queue ?: dispatch_get_main_queue(), ^{
                resultHandler(store, error);
            });
        }
    });
    
}

- (void)removePersistentStoreAtURL:(NSURL *)persistentStoreURL
                             queue:(dispatch_queue_t)queue
                     resultHandler:(void(^)(NSPersistentStore *store, NSError *error))resultHandler;
{
    dispatch_async(self.peristentStoreAddRemoveQueue, ^{
        NSPersistentStore *store = nil;
        
        NSError *error = nil;
        store = [self removePersistentStoreAtURL:persistentStoreURL error:&error];
        
        if (resultHandler) {
            dispatch_async(queue ?: dispatch_get_main_queue(), ^{
                resultHandler(store, error);
            });
        }
    });
}

#pragma mark KVO

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
    if (object == self.persistentStoreCoordinator && [keyPath isEqualToString:@"persistentStores"]) {
        self.hasPersistentStore = self.persistentStoreCoordinator.persistentStores.count > 0;
    }
}

@end
