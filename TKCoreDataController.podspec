Pod::Spec.new do |s|
  s.name         = "TKCoreDataController"
  s.version      = "0.0.7"
  s.summary      = "Controller to simpify settin up a Core Data stack. E.g. asynchronous adding of persistent stores."
  s.homepage     = "https://github.com/toto/TKCoreDataController"
  s.license      = "MIT"
  s.author       = { "Thomas Kollbach" => "toto@nxtbgthng.com" }
  s.source       = { :git => "https://github.com/toto/TKCoreDataController.git", :tag => "#{s.version}" }
  s.source_files = 'TKCoreDataController/*.{h,m}'
  s.framework  = 'CoreData'
  s.requires_arc = true
  s.ios.deployment_target = '6.0'
  s.osx.deployment_target = '10.8'
  s.watchos.deployment_target = '2.0'  
  # s.tvos.deployment_target = '9.0'
end
