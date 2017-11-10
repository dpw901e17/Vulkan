@echo off
echo compiling!

C:/VulkanSDK/1.0.57.0/Bin32/glslangValidator.exe -V shader.vert
C:/VulkanSDK/1.0.57.0/Bin32/glslangValidator.exe -V shader.frag

xcopy vert.spv "C:\Users\roger_000\Documents\Visual Studio 2015\Projects\VulkanSpike\x64\Debug\shaders\vert.spv"
xcopy frag.spv "C:\Users\roger_000\Documents\Visual Studio 2015\Projects\VulkanSpike\x64\Debug\shaders\frag.spv"
xcopy frag.spv "C:\Users\roger_000\Documents\Visual Studio 2015\Projects\VulkanSpike\x64\Release\shaders\vert.spv"
xcopy frag.spv "C:\Users\roger_000\Documents\Visual Studio 2015\Projects\VulkanSpike\x64\Release\shaders\frag.spv"

pause