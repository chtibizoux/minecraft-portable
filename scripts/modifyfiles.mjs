import fs from 'fs';

(async () => {
    var file = fs.readFileSync("jars/tmp/META-INF/MANIFEST.MF").toString();
    file = file.replace("net.minecraft.client.Main", "net.minecraft.client.main.Main\nMulti-Release: True");
    fs.writeFileSync("jars/tmp/META-INF/MANIFEST.MF", file);

    if (fs.existsSync("jars/tmp/META-INF/MOJANGCS.RSA")) {
        fs.rmSync("jars/tmp/META-INF/MOJANGCS.RSA");
    }
    if (fs.existsSync("jars/tmp/META-INF/MOJANGCS.SF")) {
        fs.rmSync("jars/tmp/META-INF/MOJANGCS.SF");
    }
    if (fs.existsSync("jars/tmp/META-INF/org/apache/logging/log4j/core/config/plugins/Log4j2Plugins.dat")) {
        fs.rmSync("jars/tmp/META-INF/org/apache/logging/log4j/core/config/plugins/Log4j2Plugins.dat");
    }

    if (fs.existsSync("assets/icons")) {
        fs.rmdirSync("assets/icons", { recursive: true, force: true });
    }

    if (fs.existsSync("assets/minecraft/resourcepacks")) {
        fs.rmdirSync("assets/minecraft/resourcepacks", { recursive: true, force: true });
    }

    if (fs.existsSync("assets/minecraft")) {
        fs.renameSync("assets/minecraft", "jars/tmp/assets/minecraft", { replace: true });
    }

    if (fs.existsSync("assets/pack.mcmeta")) {
        fs.renameSync("assets/pack.mcmeta", "jars/tmp/pack.mcmeta");
    }

    if (fs.existsSync("assets/realms")) {
        fs.rmdirSync("assets/realms", { recursive: true, force: true });
    }

    if (fs.existsSync("jars/tmp/assets/realms")) {
        fs.rmdirSync("jars/tmp/assets/realms", { recursive: true, force: true });
    }

    // Use jlink.cmd instead
    /* if (fs.existsSync("jdk.zip")) {
        fs.rmSync("jdk.zip");
    }

    fs.rmdirSync("jdk/include", { recursive: true, force: true });
    fs.rmdirSync("jdk/jmods", { recursive: true, force: true });
    fs.rmdirSync("jdk/legal", { recursive: true, force: true });
    fs.rmSync("jdk/LICENSE");
    fs.rmSync("jdk/README");
    fs.rmSync("jdk/release");
    fs.rmdirSync("jdk/conf/management", { recursive: true, force: true });
    fs.rmSync("jdk/conf/logging.properties");
    fs.rmSync("jdk/conf/net.properties");
    fs.rmSync("jdk/conf/sound.properties");
    fs.rmSync("jdk/conf/security/policy/README.txt");
    fs.rmSync("jdk/lib/src.zip");
    fs.rmSync("jdk/lib/classlist");
    fs.rmSync("jdk/lib/fontconfig.bfc");
    fs.rmSync("jdk/lib/psfont.properties.ja");
    fs.rmSync("jdk/lib/psfontj2d.properties");
    fs.rmSync("jdk/lib/fontconfig.properties.src");
    fs.rmSync("jdk/lib/ct.sym");
    fs.rmSync("jdk/lib/jawt.lib");
    fs.rmSync("jdk/lib/jvm.lib");
    fs.rmSync("jdk/lib/jrt-fs.jar");
    // const dlls = ["attach.dll", "awt.dll", "dt_shmem.dll", "dt_socket.dll", "fontmanager.dll", "freetype.dll", "instrument.dll", "j2gss.dll", "j2pcsc.dll", "j2pkcs11.dll", "jaas.dll", "javaaccessbridge.dll", "javajpeg.dll", "jawt.dll", "jdwp.dll", "jpackage.dll", "jsound.dll", "jsvml.dll", "lcms.dll", "le.dll", "management_agent.dll", "mlib_image.dll", "msvcp140.dll", "prefs.dll", "rmi.dll", "saproc.dll", "splashscreen.dll", "sspi_bridge.dll", "sunmscapi.dll", "syslookup.dll", "ucrtbase.dll", "vcruntime140.dll", "vcruntime140_1.dll", "verify.dll", "w2k_lsa_auth.dll", "windowsaccessbridge-64.dll"];
    for (const file of fs.readdirSync("jdk/bin")) {
        if ((file.endsWith(".exe") && file != "java.exe") || dlls.includes(file)) {
        fs.rmSync("jdk/bin/" + file);
    } */
})();