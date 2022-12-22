import fs from 'fs';
import fetch from 'node-fetch';

const version = "1.19.2";

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

    console.log("Download assets");
    const assetIndexResponse = await fetch(versionJSON.assetIndex.url);
    const assetIndexJSON = await assetIndexResponse.json();
    for (const path in assetIndexJSON.objects) {
        const hash = assetIndexJSON.objects[path].hash;
        if (path.includes("/")) {
            fs.mkdirSync("assets/" + path.slice(0, path.lastIndexOf("/")), { recursive: true });
        }

        if (!fs.existsSync("assets/" + path)) {
            const response = await fetch(`https://resources.download.minecraft.net/${hash.slice(0, 2)}/${hash}`);
            fs.writeFileSync("assets/" + path, Buffer.from(await response.arrayBuffer()));
        }
    }

    // Use jlink.cmd instead
    /* console.log("Download jdk");
    if (!fs.existsSync("jdk.zip")) {
        const jdkResponse = await fetch("https://download.oracle.com/java/19/latest/jdk-19_windows-x64_bin.zip");
        fs.writeFileSync("jdk.zip", Buffer.from(await jdkResponse.arrayBuffer()));
    } */
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