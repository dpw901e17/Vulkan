@echo off
echo compiling!

C:/VulkanSDK/1.0.65.0/Bin32/glslangValidator.exe -V shader.vert
C:/VulkanSDK/1.0.65.0/Bin32/glslangValidator.exe -V shader.frag

xcopy /Y .\vert.spv ..\x64\Debug\shaders\vert.spv*
xcopy /Y .\frag.spv ..\x64\Debug\shaders\frag.spv*
xcopy /Y .\vert.spv ..\x64\Release\shaders\vert.spv*
xcopy /Y .\frag.spv ..\x64\Release\shaders\frag.spv*
xcopy /Y .\sample_image.jpg ..\x64\Debug\textures\sample_image.jpg*
xcopy /Y .\sample_image.jpg ..\x64\Release\textures\sample_image.jpg*