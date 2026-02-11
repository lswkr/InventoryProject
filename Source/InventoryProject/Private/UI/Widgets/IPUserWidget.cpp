// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/IPUserWidget.h"

void UIPUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
