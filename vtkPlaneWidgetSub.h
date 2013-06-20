/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPlaneWidgetSub.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkPlaneWidgetSub_h
#define __vtkPlaneWidgetSub_h

#include "vtkPlaneWidget.h"

//  vtk includes
#include <vtkActor.h>
#include <vtkLineSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

class vtkPlaneWidgetSub : public vtkPlaneWidget
{
public:
  static vtkPlaneWidgetSub *New();
  vtkTypeMacro(vtkPlaneWidgetSub, vtkPlaneWidget);

  vtkPlaneWidgetSub();
  ~vtkPlaneWidgetSub();

public:
  void SetHandleHint();

protected:
  vtkProperty *HintHandleProperty;
};

#endif