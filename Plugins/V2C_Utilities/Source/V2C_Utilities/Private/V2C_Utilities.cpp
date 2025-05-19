
#include "V2C_Utilities.h"

#define LOCTEXT_NAMESPACE "FV2C_UtilitiesModule"

void FV2C_UtilitiesModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FV2C_UtilitiesModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FV2C_UtilitiesModule, V2C_Utilities)