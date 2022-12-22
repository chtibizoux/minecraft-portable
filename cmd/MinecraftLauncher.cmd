@echo off
set /p username=Nom d'utilisateur: 
jdk\bin\java.exe -jar minecraft.jar --username %username% --version 1.19.2 --accessToken 00000000-0000-0000-0000-000000000000
pause