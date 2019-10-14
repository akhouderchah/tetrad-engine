#define PACKAGE_MODIFY
#include "core/Package.h"

namespace tetrad
{

int main()
{
  Package package;

  char subHeader[] = {'S', 'B', 'H', 'D', 'R'};

#ifdef CREATE
  package.CreatePackage("test.cpk");
  package.AddElement("../../assets/textures/City.tga", "CityTexture", subHeader,
                     sizeof(subHeader), PackageFormat::DataType_t::TEXTURE);
  package.AddElement("../../assets/textures/Background.tga", "BackgroundTexture",
                     subHeader, sizeof(subHeader), PackageFormat::DataType_t::TEXTURE);
#else
  package.Load("test.cpk");
  package.AddElement("../../assets/textures/PauseGradient.tga", "PauseGradient",
                     subHeader, sizeof(subHeader), PackageFormat::DataType_t::TEXTURE);
#endif
  package.Unload();
}

}  // namespace tetrad
