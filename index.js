'use strict';

const path = require('path');
const winfileversion = require('./build/Release/winfileversion.node');

console.log(`native addon VersionResponse(1,3,3,7)->getMajor(): ${winfileversion.VersionResponse(1,3,3,7).getMajor()}`)

const absolutePath = path.join(process.cwd(), 'Test.dll');
console.log(absolutePath);

console.log(`native addon getFileVersion->Test.dll->getRevision(): ${winfileversion.getFileVersion(absolutePath).getRevision()}`);

console.dir(winfileversion);