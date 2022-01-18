
rc GUI\\resources\\resource.rc


cl main.cpp GUI\\resources\\resource.res /W4


mt.exe -manifest main.exe.manifest -outputresource:main.exe;1

del GUI\\resources\\resource.res
del main.exe.manifest
del main.obj

