// © 2024 Choklad4Life. All rights reserved.


#include "UI/Widget/DBUserWidget.h"

void UDBUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
