import fs from 'fs';
import fetch from 'node-fetch';
import { execSync } from 'child_process';

const version = "1.19.2";
const JARPath = "D:/Logiciels/java/jdk-19/bin/jar.exe";

(async () => {
    // TODO: select version
    const versionsResponse = await fetch("https://launchermeta.mojang.com/mc/game/version_manifest.json");
    const versionsJSON = await versionsResponse.json();

    const desiredVersion = versionsJSON.versions.filter(v => v.id === version)[0] || null;
    if (!desiredVersion) throw "Error when getting version";

    const versionResponse = await fetch(desiredVersion.url);
    const versionJSON = await versionResponse.json();

    if (!fs.existsSync("jars/lib/", { recursive: true })) {
        fs.mkdirSync("jars/lib/", { recursive: true });
    }

    console.log("Download client.jar");
    if (!fs.existsSync("jars/client.jar")) {
        const clientResponse = await fetch(versionJSON.downloads.client.url);
        fs.writeFileSync("jars/client.jar", Buffer.from(await clientResponse.arrayBuffer()));
    }

    console.log("Download libraries");
    for (const lib of versionJSON.libraries) {
        const name = lib.downloads.artifact.path.slice(lib.downloads.artifact.path.lastIndexOf("/") + 1);
        if (!fs.existsSync("jars/lib/" + name)) {
            if (parseRule(lib)) {
                console.log(name + " not downloaded");
                continue;
            }
            const response = await fetch(lib.downloads.artifact.url);
            fs.writeFileSync("jars/lib/" + name, Buffer.from(await response.arrayBuffer()));
        }
    }

    if (!fs.existsSync("jars/tmp/")) {
        fs.mkdirSync("jars/tmp/");
    }

    console.log("unzip .jar files in tmp");
    for (const file of fs.readdirSync("jars/lib")) {
        // unzip(`jars/lib/${file}`, "jars/tmp", { replace: true });
        execSync(`cd jars/tmp && ${JARPath} xf ../lib/${file}`);
    }
    // unzip("jars/client.jar", "jars/tmp", { replace: true });
    execSync(`cd jars/tmp && ${JARPath} xf ../client.jar`);

    console.log("use good Main.java file");
    var file = fs.readFileSync("jars/tmp/META-INF/MANIFEST.MF").toString();
    file = file.replace("net.minecraft.client.Main", "net.minecraft.client.main.Main\nMulti-Release: True");
    fs.writeFileSync("jars/tmp/META-INF/MANIFEST.MF", file);

    console.log("Remove MOJANGCS encryption files");
    if (fs.existsSync("jars/tmp/META-INF/MOJANGCS.RSA")) {
        fs.rmSync("jars/tmp/META-INF/MOJANGCS.RSA");
    }
    if (fs.existsSync("jars/tmp/META-INF/MOJANGCS.SF")) {
        fs.rmSync("jars/tmp/META-INF/MOJANGCS.SF");
    }
    console.log("Remove log4j plugins cache");
    if (fs.existsSync("jars/tmp/META-INF/org/apache/logging/log4j/core/config/plugins/Log4j2Plugins.dat")) {
        fs.rmSync("jars/tmp/META-INF/org/apache/logging/log4j/core/config/plugins/Log4j2Plugins.dat");
    }

    console.log("Download assets");
    const assetIndexResponse = await fetch(versionJSON.assetIndex.url);
    const assetIndexJSON = await assetIndexResponse.json();
    for (const path in assetIndexJSON.objects) {
        const hash = assetIndexJSON.objects[path].hash;
        if (path.includes("/")) {
            fs.mkdirSync("jars/tmp/assets/" + path.slice(0, path.lastIndexOf("/")), { recursive: true });
        }

        const response = await fetch(`https://resources.download.minecraft.net/${hash.slice(0, 2)}/${hash}`);
        fs.writeFileSync("jars/tmp/assets/" + path, Buffer.from(await response.arrayBuffer()));
    }

    console.log("Remove unused icons");
    if (fs.existsSync("jars/tmp/assets/icons")) {
        fs.rmdirSync("jars/tmp/assets/icons", { recursive: true, force: true });
    }

    console.log("Remove programmer_art resourcepack");
    if (fs.existsSync("jars/tmp/assets/minecraft/resourcepacks")) {
        fs.rmdirSync("jars/tmp/assets/minecraft/resourcepacks", { recursive: true, force: true });
    }

    console.log("Move pack.mcmeta");
    if (fs.existsSync("jars/tmp/assets/pack.mcmeta")) {
        fs.renameSync("jars/tmp/assets/pack.mcmeta", "jars/tmp/pack.mcmeta");
    }

    console.log("Remove realms assets");
    if (fs.existsSync("jars/tmp/assets/realms")) {
        fs.rmdirSync("jars/tmp/assets/realms", { recursive: true, force: true });
    }

    console.log("Create minecraft.jar");
    // zip("jars/tmp", "minecraft.jar");
    execSync(`${JARPath} cfm minecraft.jar jars/tmp/META-INF/MANIFEST.MF -C jars/tmp .`);

    // Use jlink.cmd instead
    /* console.log("Download jdk");
    if (!fs.existsSync("jdk.zip")) {
        const jdkResponse = await fetch("https://download.oracle.com/java/19/latest/jdk-19_windows-x64_bin.zip");
        fs.writeFileSync("jdk.zip", Buffer.from(await jdkResponse.arrayBuffer()));
    }
    // unzip("jdk.zip", "./");
    console.log("Extract jdk");
    execSync(`${JARPath} xf jdk.zip`);
    console.log("Rename jdk");
    fs.renameSync("jdk-19", "jdk");
    console.log("Delete jdk.zip");
    fs.rmSync("jdk.zip");

    console.log("Delete unesed jdk files");
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
    for (const file of fs.readdirSync("jdk/bin")) {
        if (file.endsWith(".exe") && file != "java.exe") {
            fs.rmSync("jdk/bin/" + file);
        }
    }
    // console.log("Compress full-jdk.zip");
    // // zip("jdk", "full-jdk.zip");
    // execSync(`${JARPath} cfM full-jdk.zip -C jdk .`);
    // const dlls = ["attach.dll", "awt.dll", "dt_shmem.dll", "dt_socket.dll", "fontmanager.dll", "freetype.dll", "instrument.dll", "j2gss.dll", "j2pcsc.dll", "j2pkcs11.dll", "jaas.dll", "javaaccessbridge.dll", "javajpeg.dll", "jawt.dll", "jdwp.dll", "jpackage.dll", "jsound.dll", "jsvml.dll", "lcms.dll", "le.dll", "management_agent.dll", "mlib_image.dll", "msvcp140.dll", "prefs.dll", "rmi.dll", "saproc.dll", "splashscreen.dll", "sspi_bridge.dll", "sunmscapi.dll", "syslookup.dll", "ucrtbase.dll", "vcruntime140.dll", "vcruntime140_1.dll", "verify.dll", "w2k_lsa_auth.dll", "windowsaccessbridge-64.dll"];
    // for (const file of fs.readdirSync("jdk/bin")) {
    //     if (dlls.includes(file)) {
    //         fs.rmSync("jdk/bin/" + file);
    //     }
    // }
    console.log("Compress jdk.zip");
    // zip("jdk", "jdk.zip");
    execSync(`${JARPath} cfM jdk.zip -C jdk .`); */
})();

function parseRule(lib) {
    if (lib.rules) {
        if (lib.rules.length > 1) {
            return !(lib.rules[0].action === 'allow' && lib.rules[1].action === 'disallow' && lib.rules[1].os.name === 'osx');
        } else {
            if (lib.rules[0].action === 'allow' && lib.rules[0].os) return lib.rules[0].os.name !== "windows";
        }
    } else {
        return false;
    }
}