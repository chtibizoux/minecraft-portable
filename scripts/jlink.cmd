@echo off

set /p path=Minecraft remmaped jar or directory (use fabric remmaped jar, that work great): 

%JAVA_HOME%\bin\jdeps.exe -s --multi-release 19 %path%

echo ajouter jdk.zipfs à la fin (par ce que ce fdp ne le détecte pas)
set /p modules=Modules(séparer par une virgule): 
@Rem set modules="java.base,java.compiler,java.datatransfer,java.desktop,java.logging,java.management,java.naming,java.rmi,java.scripting,java.security.jgss,java.sql,java.xml,jdk.jfr,jdk.unsupported,jdk.zipfs"

%JAVA_HOME%\bin\jlink.exe --module-path %modules% "%JAVA_HOME%\jmods" --add-modules --output jdk

@Rem TODO: Remove unused files
echo Please remove unused files after (not already implemented)

pause