# Minecraft portable

A portable version of minecraft 1.19.2.

#### [Download](https://github.com/chtibizoux/minecraft-portable/releases/download/v1.0.0/MinecraftLauncher.exe)

## Files description ([All files](https://github.com/chtibizoux/minecraft-portable/releases/tag/v1.0.0)):

[minecraft.jar](https://github.com/chtibizoux/minecraft-portable/releases/download/v1.0.0/minecraft.jar): Minecraft 1.19.2 compressed in a single jar file.

[jdk.zip](https://github.com/chtibizoux/minecraft-portable/releases/download/v1.0.0/jdk.zip): The Oracle JDK 19 shink to take up less space.

[MinecraftLauncher.cmd](https://github.com/chtibizoux/minecraft-portable/releases/download/v1.0.0/MinecraftLauncher.cmd): A script file to launch minecraft in a terminal with logs.

[MinecraftLauncher.vbs](https://github.com/chtibizoux/minecraft-portable/releases/download/v1.0.0/MinecraftLauncher.vbs): A script file to launch minecraft with a popup and without logs.

[OfflineLauncher.exe](https://github.com/chtibizoux/minecraft-portable/releases/download/v1.0.0/OfflineLauncher.exe): A launcher to automatically install minecraft without internet connexion (Minecraft files are already in .exe file) and launch it.

[MinecraftLauncher.exe](https://github.com/chtibizoux/minecraft-portable/releases/download/v1.0.0/MinecraftLauncher.exe): A launcher to automatically download and install minecraft from this repository ([minecraft.jar](https://github.com/chtibizoux/minecraft-portable/releases/download/v1.0.0/minecraft.jar) and [jdk.zip](https://github.com/chtibizoux/minecraft-portable/releases/download/v1.0.0/jdk.zip)) and launch it.

`.exe` launchers can show minecraft logs and have a minimize to tray feature (to brainfuck your teacher):

[CppLauncher.exe](https://github.com/chtibizoux/minecraft-portable/releases/download/v1.0.0/CppLauncher.exe): A launcher writen in c++ (size less) to launch minecraft or fabric with `java.exe` or `javaw.exe` in [jdk](https://github.com/chtibizoux/minecraft-portable/releases/download/v1.0.0/jdk.zip), it can show logs but don't install minecraft files automaticaly.

[CppJniLauncher.exe](https://github.com/chtibizoux/minecraft-portable/releases/download/v1.0.0/CppJniLauncher.exe): A launcher writen in c++ (size less) to launch minecraft or fabric with `jni` so you can use a sizeless jdk: [jdk-jni.zip](https://github.com/chtibizoux/minecraft-portable/releases/download/v1.0.0/jdk-jni.zip), it can't show logs and don't install minecraft files automaticaly.

[jdk-jni.zip](https://github.com/chtibizoux/minecraft-portable/releases/download/v1.0.0/jdk-jni.zip): The Oracle JDK 19 shink to take up less space, it can only be used by [CppJniLauncher.exe](https://github.com/chtibizoux/minecraft-portable/releases/download/v1.0.0/CppJniLauncher.exe). 

![](https://github.com/chtibizoux/minecraft-portable/raw/main/minimizeToTray.gif)

#### If you want to use a proxy with `.exe` launchers create a `proxy.txt` file next to the `exe` file and write `<proxy host>:<proxy port>`

#### If you want to launch java with other arguments create a `args.txt` file and write your args like `-Xms1024M -Xmx2G`

## Use fabric mods

#### Download [FabricLauncher.vbs](https://github.com/chtibizoux/minecraft-portable/releases/download/v1.0.0/FabricLauncher.vbs) and [fabric.jar](https://github.com/chtibizoux/minecraft-portable/releases/download/v1.0.0/fabric.jar) in same directory of minecraft files and [fabric-api.jar](https://github.com/chtibizoux/minecraft-portable/releases/download/v1.0.0/fabric-api.jar) in `mods` directory (create one or launch fabric) and start fabric with `FabricLauncher.vbs`.

If you are using `.exe` launchers start the launcher one time before to install minecraft files.

[FabricLauncher.vbs](https://github.com/chtibizoux/minecraft-portable/releases/download/v1.0.0/FabricLauncher.vbs): A script file to launch fabric with a popup and without logs.

[FabricLauncher.cmd](https://github.com/chtibizoux/minecraft-portable/releases/download/v1.0.0/FabricLauncher.cmd): A script file to launch fabric in a terminal with logs.

[FabricLauncherTweak.vbs](https://github.com/chtibizoux/minecraft-portable/releases/download/v1.0.0/FabricLauncherTweak.vbs): A script file to launch fabric without [minecraft.jar](https://github.com/chtibizoux/minecraft-portable/releases/download/v1.0.0/minecraft.jar) with a popup and without logs, to use it launch [FabricLauncher.vbs](https://github.com/chtibizoux/minecraft-portable/releases/download/v1.0.0/FabricLauncher.vbs) a first time with [minecraft.jar](https://github.com/chtibizoux/minecraft-portable/releases/download/v1.0.0/minecraft.jar) (you remove them after) or download [frabric-remmaped-minecraft.jar](https://github.com/chtibizoux/minecraft-portable/releases/download/v1.0.0/frabric-remmaped-minecraft.jar) in `.fabric/remappedJars/minecraft-1.19.2-0.14.10/` (create these folder) and rename it to `client-intermediary.jar`.

[FabricLauncherTweak.cmd](https://github.com/chtibizoux/minecraft-portable/releases/download/v1.0.0/FabricLauncherTweak.cmd): A script file to launch fabric without [minecraft.jar](https://github.com/chtibizoux/minecraft-portable/releases/download/v1.0.0/minecraft.jar) in a terminal with logs, to use it launch [FabricLauncher.cmd](https://github.com/chtibizoux/minecraft-portable/releases/download/v1.0.0/FabricLauncher.cmd) a first time with [minecraft.jar](https://github.com/chtibizoux/minecraft-portable/releases/download/v1.0.0/minecraft.jar) (you remove them after) or download [frabric-remmaped-minecraft.jar](https://github.com/chtibizoux/minecraft-portable/releases/download/v1.0.0/frabric-remmaped-minecraft.jar) in `.fabric/remappedJars/minecraft-1.19.2-0.14.10/` (create these folder) and rename it to `client-intermediary.jar`.

[fabric.jar](https://github.com/chtibizoux/minecraft-portable/releases/download/v1.0.0/fabric.jar): Fabricmc compressed in a single jar file.

[fabric-api.jar](https://github.com/chtibizoux/minecraft-portable/releases/download/v1.0.0/fabric-api.jar): The fabric api without minimum JAVA_17 version verification due to bad version detection.

[frabric-remmaped-minecraft.jar](https://github.com/chtibizoux/minecraft-portable/releases/download/v1.0.0/frabric-remmaped-minecraft.jar): [minecraft.jar](https://github.com/chtibizoux/minecraft-portable/releases/download/v1.0.0/minecraft.jar) remmaped by fabric to use it create folders `.fabric/remappedJars/minecraft-1.19.2-0.14.10/` and download it inside and rename it to `client-intermediary.jar`.
