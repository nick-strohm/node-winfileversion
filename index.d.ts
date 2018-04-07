declare namespace winfileversion {
    interface VersionResponse
    {
        VersionResponse(major: Number, minor: Number, build: Number, revision: Number);

        getMajor(): Number;
        getMinor(): Number;
        getBuild(): Number;
        getRevision(): Number;
    }

    interface WinFileVersion {
        VersionResponse: VersionResponse;

        getFileVersion(path: String): VersionResponse;
    }
}

declare const winfileversion: winfileversion.WinFileVersion;
module.export = winfileversion;
exports = winfileversion;