
#Declare the name of this package:
atlas_subdir( OffSM4topsTool None )

# Temporary directory for XGBoost build results:
set( _xgboostBuildDir ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/XGBoostBuild )

# Set up the variables that the users can pick up XGBoost with:
set( XGBOOST_INCLUDE_DIRS
  $<BUILD_INTERFACE:${_xgboostBuildDir}/include>
  $<INSTALL_INTERFACE:include> )
set( XGBOOST_LIBRARIES
  ${_xgboostBuildDir}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}xgboost${CMAKE_SHARED_LIBRARY_SUFFIX}
  ${_xgboostBuildDir}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}rabit${CMAKE_STATIC_LIBRARY_SUFFIX}
  ${_xgboostBuildDir}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}dmlc${CMAKE_STATIC_LIBRARY_SUFFIX} )

# Set up the build of XGBoost for the build area:
ExternalProject_Add( XGBoost
  PREFIX ${CMAKE_BINARY_DIR}
  INSTALL_DIR ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}
  GIT_REPOSITORY https://github.com/dmlc/xgboost.git
  GIT_TAG 7387866
  BUILD_IN_SOURCE 1
  BUILD_COMMAND $(MAKE)
  INSTALL_COMMAND
  ${CMAKE_COMMAND} -E make_directory ${_xgboostBuildDir} &&
  ${CMAKE_COMMAND} -E make_directory ${_xgboostBuildDir}/lib &&
  ${CMAKE_COMMAND} -E make_directory ${_xgboostBuildDir}/include &&
  ${CMAKE_COMMAND} -E create_symlink ${CMAKE_BINARY_DIR}/src/XGBoost/include/xgboost ${_xgboostBuildDir}/include/xgboost &&
  ${CMAKE_COMMAND} -E create_symlink ${CMAKE_BINARY_DIR}/src/XGBoost/rabit/include/rabit ${_xgboostBuildDir}/include/rabit &&
  ${CMAKE_COMMAND} -E create_symlink ${CMAKE_BINARY_DIR}/src/XGBoost/dmlc-core/include/dmlc ${_xgboostBuildDir}/include/dmlc &&
  ${CMAKE_COMMAND} -E copy_if_different lib/${CMAKE_SHARED_LIBRARY_PREFIX}xgboost${CMAKE_SHARED_LIBRARY_SUFFIX} ${_xgboostBuildDir}/lib &&
  ${CMAKE_COMMAND} -E copy_if_different rabit/${CMAKE_STATIC_LIBRARY_PREFIX}rabit${CMAKE_STATIC_LIBRARY_SUFFIX} ${_xgboostBuildDir}/lib &&
  ${CMAKE_COMMAND} -E copy_if_different dmlc-core/${CMAKE_STATIC_LIBRARY_PREFIX}dmlc${CMAKE_STATIC_LIBRARY_SUFFIX} ${_xgboostBuildDir}/lib &&
  ${CMAKE_COMMAND} -E copy_directory ${_xgboostBuildDir}/ <INSTALL_DIR>
  BUILD_BYPRODUCTS ${XGBOOST_LIBRARIES} )

# Install XGBoost
install( DIRECTORY ${_xgboostBuildDir}/
  DESTINATION . USE_SOURCE_PERMISSIONS )

# Clean up:
unset( _xgboostBuildDir )


#This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
												  xAODBTaggingEfficiency
												  CalibrationDataInterface
												  TTHbbObjects
												  TTHbbConfiguration
												  TTHbbToolManager
												  TTHbbClassification
                          OfflineTTHbb
                          TtTRF)

#This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf Physics )

#Generate a CINT dictionary source file:
atlas_add_root_dictionary( OffSM4topsTool _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

#Build a library that other components can link against:
atlas_add_library( OffSM4topsTool Root/*.cxx Root/*.h Root/*.icc
                   OffSM4topsTool/*.h OffSM4topsTool/*.icc OffSM4topsTool/*/*.h
                   OffSM4topsTool/*/*.icc ${_cintDictSource}
                   PUBLIC_HEADERS OffSM4topsTool
                   LINK_LIBRARIES PUBLIC
								   AsgTools
								   TTHbbObjects
								   xAODBTaggingEfficiencyLib
					   		   CalibrationDataInterfaceLib
								   TTHbbConfiguration
								   TTHbbToolManager
								   TTHbbClassification
                   TtTRF
				  			   ${ROOT_LIBRARIES}
									 ${XGBOOST_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${XGBOOST_INCLUDE_DIRS})

add_dependencies( OffSM4topsTool XGBoost )

# Build the executables of the package:
atlas_add_executable( SM4t-offline
                      util/SM4t-offline.cxx
                      LINK_LIBRARIES TopDataPreparation
								      xAODBTaggingEfficiencyLib
								      CalibrationDataInterfaceLib
								      TTHbbObjects
                      TTHbbConfiguration
                      TTHbbToolManager
				     					TTHbbClassification
                      ${ROOT_LIBRARIES}
											${XGBOOST_LIBRARIES}
                      OfflineTTHbb
                      OffSM4topsTool )

atlas_install_data( share/* )
