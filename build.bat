
@echo off



call %cd%\\buildfiles\\check-dependencies.cmd

if "%abletobuild%" == "false" (
    goto EOF
)


call rc GUI\\resources\\resource.rc

if errorlevel 1 (
    goto EOF
)


call cl main.cpp GUI\\resources\\resource.res /W4

if errorlevel 1 (
    goto EOF
)


call mt.exe -manifest main.exe.manifest -outputresource:main.exe;1

if errorlevel 1 (
    goto EOF
)

del GUI\\resources\\resource.res
del main.exe.manifest
del main.obj

:EOF

