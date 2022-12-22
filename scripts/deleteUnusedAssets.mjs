import fs from 'fs';
const version = "1.19";
const json = JSON.parse(fs.readFileSync(`indexes/${version}.json`));
const hashList = Object.values(json.objects).map((o) => o.hash);
console.log(hashList);
for (const dir of fs.readdirSync("objects")) {
    for (const file of fs.readdirSync("objects/" + dir)) {
        if (!hashList.includes(file)) {
            console.log(file);
            fs.rmSync(`objects/${dir}/${file}`);
        }
    }
    if (fs.readdirSync("objects/" + dir).length == 0) {
        fs.rmdirSync("objects/" + dir);
    }
}