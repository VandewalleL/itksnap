/*=========================================================================

  Program:   ITK-SNAP
  Module:    $RCSfile: RestrictedImageIOWizardLogic.txx,v $
  Language:  C++
  Date:      $Date: 2009/02/05 22:19:10 $
  Version:   $Revision: 1.4 $
  Copyright (c) 2007 Paul A. Yushkevich
  
  This file is part of ITK-SNAP 

  ITK-SNAP is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  -----

  Copyright (c) 2003 Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information. 

=========================================================================*/

#include "RestrictedImageIOWizardLogic.h"

template<class TPixel>
bool
RestrictedImageIOWizardLogic<TPixel>
::DisplayInputWizard(const char *file)
{
  // Make sure there is a grey image as a reference
  assert(m_GreyImage);

  // Get the size and spacing of the grey image
  SizeType requiredSize = m_GreyImage->GetBufferedRegion().GetSize();

  const double *requiredSpacing = m_GreyImage->GetSpacing().GetDataPointer();

  // Prepare the header page of the wizard UI
  this->m_InHeaderPageDimX->value(requiredSize[0]);
  this->m_InHeaderPageDimY->value(requiredSize[1]);
  this->m_InHeaderPageDimZ->value(requiredSize[2]);
  this->m_InHeaderPageSpacingX->value(requiredSpacing[0]);
  this->m_InHeaderPageSpacingY->value(requiredSpacing[1]);
  this->m_InHeaderPageSpacingZ->value(requiredSpacing[2]);

  // Call the parent's method
  return Superclass::DisplayInputWizard(file);    
}

template<class TPixel>
bool
RestrictedImageIOWizardLogic<TPixel>
::CheckImageValidity()
{
  // TODO: Move this code to IRISApplications and call from here and from
  // command-line loading.
  SizeType requiredSize = m_GreyImage->GetBufferedRegion().GetSize();
  SizeType loadedSize = this->m_Image->GetBufferedRegion().GetSize();

  // Check whether or not the image size matches the 'forced' image size
  if(!(requiredSize == loadedSize))
    {
    // Bark at the user
    fl_alert(
      "The size of the image you are attempting to load does not match "
      "the size of the 'grey' image already loaded.");

    return false;
    }

  // Check if there is a discrepancy in the header fields. This will not
  // preclude the user from loading the image, but it will generate a 
  // warning, hopefully leading users to adopt more flexible file formats
  bool match_spacing = true, match_origin = true, match_direction = true;
  for(size_t i = 0; i < 3; i++)
    {
    if(m_GreyImage->GetSpacing()[i] != this->m_Image->GetSpacing()[i])
      match_spacing = false;

    if(m_GreyImage->GetOrigin()[i] != this->m_Image->GetOrigin()[i])
      match_origin = false;

    for(size_t j = 0; j < 3; j++)
      {
      double diff = fabs(m_GreyImage->GetDirection()(i,j) - this->m_Image->GetDirection()(i,j));
      if(diff > 1.0e-4)
        match_direction = false;
      }
    }

  if(!match_spacing || !match_origin || !match_direction)
    {
    // Come up with a warning message
    std::string object, verb;
    if(!match_spacing && !match_origin && !match_direction)
      { object = "spacing, origin and orientation"; }
    else if (!match_spacing && !match_origin)
      { object = "spacing and origin"; }
    else if (!match_spacing && !match_direction)
      { object = "spacing and orientation"; }
    else if (!match_origin && !match_direction)
      { object = "origin and orientation";}
    else if (!match_spacing)
      { object = "spacing"; }
    else if (!match_direction)
      { object = "orientation";}
    else if (!match_origin)
      { object = "origin"; }

    // Create an alert box
    fl_choice(
      "There is a mismatch between the header of the image that you are\n"
      "loading and the header of the 'grey' image currently open in SNAP.\n\n"
      "The images have different %s. \n\n"
      "SNAP will ignore the header information in the image you are loading.\n",
      "Ok", NULL, NULL,
      object.c_str());

    return true;
    }


  else
    return true;
}
