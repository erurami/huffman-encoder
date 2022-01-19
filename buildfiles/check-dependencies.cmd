
@echo off

set abletobuild=true


REM call rc >nul 2>&1
REM 
REM if errorlevel 1 (
REM     echo rc not installed
REM     set abletobuild=false
REM     goto EOF
REM )

call rc >nul 2>&1

if errorlevel 1 (
    echo rc not installed
    set abletobuild=false
    goto EOF
)


call cl >nul 2>&1

if errorlevel 1 (
    echo cl not installed
    set abletobuild=false
    goto EOF
)


call mt >nul 2>&1

if errorlevel 1 (
    echo mt not installed
    set abletobuild=false
    goto EOF
)



:EOF

