/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#include "itkVXLVideoIOFactory.h"
#include "itkVXLVideoIO.h"
#include "itkVersion.h"

namespace itk
{
VXLVideoIOFactory::VXLVideoIOFactory()
{
  this->RegisterOverride("itkVideoIOBase", "itkVXLVideoIO", "VXL Video IO", 1, CreateObjectFunction<VXLVideoIO>::New());
}

VXLVideoIOFactory::~VXLVideoIOFactory() {}

const char *
VXLVideoIOFactory::GetITKSourceVersion() const
{
  return ITK_SOURCE_VERSION;
}

const char *
VXLVideoIOFactory::GetDescription() const
{
  return "VXL VideoIO Factory, allows the loading of AVI videos into Insight";
}

// Undocumented API used to register during static initialization.
// DO NOT CALL DIRECTLY.
void
VXLVideoIOFactoryRegister__Private()
{
  ObjectFactoryBase::RegisterInternalFactoryOnce<VXLVideoIOFactory>();
}

} // end namespace itk
