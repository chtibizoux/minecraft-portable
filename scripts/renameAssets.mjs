import fs from 'fs';
const version = "1.19";
const json = JSON.parse(fs.readFileSync(`indexes/${version}.json`));
for (const path in json.objects) {
    const hash = json.objects[path].hash;
    if (fs.existsSync(`objects/${hash.slice(0, 2)}/${hash}`)) {
        if (path.includes("/")) {
            fs.mkdirSync(path.slice(0, path.lastIndexOf("/")), { recursive: true });
        }
        fs.renameSync(`objects/${hash.slice(0, 2)}/${hash}`, path);
    }
}