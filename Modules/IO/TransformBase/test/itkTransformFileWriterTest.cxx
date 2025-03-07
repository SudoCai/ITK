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


#include "itkTransformFileWriter.h"
#include "itkTestingMacros.h"

int
itkTransformFileWriterTest(int, char *[])
{
  using TransformWriterType = itk::TransformFileWriter;


  auto transformWriter = TransformWriterType::New();

  std::cout << "Writer class = " << transformWriter->GetNameOfClass() << "Writer base = "
            << dynamic_cast<TransformWriterType::Superclass *>(transformWriter.GetPointer())->GetNameOfClass()
            << std::endl;

  try
  {
    // trigger empty write exception
    transformWriter->Update();
  }
  catch (const itk::ExceptionObject & excp)
  {
    std::cerr << "Expected exception (no filename)" << std::endl << excp << std::endl;
  }
  transformWriter->SetFileName("transform.garbage");
  try
  {
    // trigger exception for transformio not found
    transformWriter->Update();
  }
  catch (const itk::ExceptionObject & excp)
  {

    std::cerr << "Expected exception (no transformio that can write garbage and no transformio should be registered)"
              << excp << std::endl;
  }


  std::cout << "Test PASSED!" << std::endl;

  return EXIT_SUCCESS;
}
