Set WshShell = WScript.CreateObject("WScript.Shell")
username = InputBox("Nom d'utilisateur:", "Minecraft Launcher")
WshShell.Run "jdk\bin\java.exe -jar minecraft.jar --username " &  username & " --version 1.19.2 --accessToken 00000000-0000-0000-0000-000000000000", 0, false