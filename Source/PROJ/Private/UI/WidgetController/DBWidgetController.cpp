// © 2024 Choklad4Life. All rights reserved.


#include "UI/WidgetController/DBWidgetController.h"

void UDBWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UDBWidgetController::BroadcastInitialValues()
{
	// Override this function in child classes
}

void UDBWidgetController::BindCallbacksToDependencies()
{
	
}
