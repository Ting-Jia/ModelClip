/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPlaneWidgetSUb.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkPlaneWidgetSub.h"

#include <vtkObjectFactory.h>
#include <vtkProperty.h>
#include <vtkSphereSource.h>

vtkStandardNewMacro(vtkPlaneWidgetSub);

vtkPlaneWidgetSub::vtkPlaneWidgetSub() : vtkPlaneWidget()
{
  this->HintHandleProperty = vtkProperty::New();
  this->HintHandleProperty->SetColor(0, 1, 1);
}

vtkPlaneWidgetSub::~vtkPlaneWidgetSub()
{
  this->HintHandleProperty->Delete();
}

void vtkPlaneWidgetSub::SetHandleHint()
{
  this->Handle[2]->SetProperty(this->HintHandleProperty);
  this->Handle[3]->SetProperty(this->HintHandleProperty);
}
