/************************************************************************************

Filename    :   Util_GL_Blitter.h
Content     :   GL implementation for blitting, supporting scaling & rotation
Created     :   February 24, 2015
Authors     :   Reza Nourai

Copyright   :   Copyright (c) Facebook Technologies, LLC and its affiliates. All rights reserved.

Licensed under the Oculus Master SDK License Version 1.0 (the "License");
you may not use the Oculus VR Rift SDK except in compliance with the License,
which is provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

You may obtain a copy of the License at

https://developer.oculus.com/licenses/oculusmastersdk-1.0

Unless required by applicable law or agreed to in writing, the Oculus VR SDK
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

************************************************************************************/

#ifndef OVR_Util_GL_Blitter_h
#define OVR_Util_GL_Blitter_h

#include "Kernel/OVR_RefCount.h"
#include "Kernel/OVR_Win32_IncludeWindows.h"

#include "GL/CAPI_GLE.h"

#if defined(OVR_OS_WIN32)
#include <gl/GL.h>
#elif defined(__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

namespace OVR {
namespace GLUtil {

//-------------------------------------------------------------------------------------
// ***** CAPI::Blitter

// D3D11 implementation of blitter

class Blitter : public RefCountBase<Blitter> {
 public:
  Blitter();
  ~Blitter();

  bool Initialize();

  // Blit sourceTexture to the active frame buffer
  bool Blt(GLuint sourceTexId);
  bool
  Blt(GLuint sourceTexId, uint32_t topLeftX, uint32_t topLeftY, uint32_t width, uint32_t height);
  bool BltCubemap(GLuint sourceTexId, GLuint tempTexId, uint32_t cubeMapSize);

 private:
  GLuint ReadFBO;
};
} // namespace GLUtil
} // namespace OVR

#endif // OVR_Util_GL_Blitter_h
