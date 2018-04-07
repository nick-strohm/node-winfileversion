#ifdef UNICODE
#undef UNICODE
#endif

#pragma comment(lib, "Version")

#include <nan.h>
#include <Windows.h>
#include <strsafe.h>
#include <TlHelp32.h>
#include <iostream>

void ErrorExit(LPTSTR lpszFunction) 
{ 
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(dw); 
}

class VersionResponse : public Nan::ObjectWrap {
    public:

        static NAN_MODULE_INIT(Init) {
            v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
            tpl->SetClassName(Nan::New("VersionResponse").ToLocalChecked());
            tpl->InstanceTemplate()->SetInternalFieldCount(4);

            Nan::SetPrototypeMethod(tpl, "getMajor", GetMajor);
            Nan::SetPrototypeMethod(tpl, "getMinor", GetMinor);
            Nan::SetPrototypeMethod(tpl, "getBuild", GetBuild);
            Nan::SetPrototypeMethod(tpl, "getRevision", GetRevision);

            constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
            Nan::Set(target, Nan::New("VersionResponse").ToLocalChecked(), Nan::GetFunction(tpl).ToLocalChecked());
        }

        static NAN_METHOD(NewInstance) {
            if (info.Length() != 4) {
                Nan::ThrowRangeError("Expected 4 arguments. major, minor, build, revision.");
                return;
            }

            if (!(info[0]->IsNumber() && info[1]->IsNumber() && info[2]->IsNumber() && info[3]->IsNumber())) {
                Nan::ThrowTypeError("An argument is not a number.");
                return;
            }

            int32_t major = Nan::To<int32_t>(info[0]).FromJust();
            int32_t minor = Nan::To<int32_t>(info[1]).FromJust();
            int32_t build = Nan::To<int32_t>(info[2]).FromJust();
            int32_t revis = Nan::To<int32_t>(info[3]).FromJust();

            VersionResponse* obj = new VersionResponse(major, minor, build, revis);
            obj->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
        }

        static v8::Local<v8::Object> IntNewInstance(int32_t t_major = 0, int32_t t_minor = 0, int32_t t_build = 0, int32_t t_revis = 0) {
            v8::Local<v8::Function> cons = Nan::New(constructor());
            v8::Local<v8::Number> major = Nan::New<v8::Number>(t_major);
            v8::Local<v8::Number> minor = Nan::New<v8::Number>(t_minor);
            v8::Local<v8::Number> build = Nan::New<v8::Number>(t_build);
            v8::Local<v8::Number> revis = Nan::New<v8::Number>(t_revis);
            const int argc = 4;
            v8::Local<v8::Value> argv[argc] = {major, minor, build, revis};
            return Nan::NewInstance(cons, argc, argv).ToLocalChecked();
        }

        inline double major() const {
            return major_;
        }

        inline double minor() const {
            return minor_;
        }

        inline double build() const {
            return build_;
        }

        inline double revision() const {
            return revision_;
        }
    private:
        explicit VersionResponse(int32_t major = 0, int32_t minor = 0, int32_t build = 0, int32_t revision = 0) {
            major_ = major;
            minor_ = minor;
            build_ = build;
            revision_ = revision;
        }

        ~VersionResponse() {}

        static NAN_METHOD(New) {
            if (info.IsConstructCall()) {
                int32_t major = Nan::To<int32_t>(info[0]).FromJust();
                int32_t minor = Nan::To<int32_t>(info[1]).FromJust();
                int32_t build = Nan::To<int32_t>(info[2]).FromJust();
                int32_t revis = Nan::To<int32_t>(info[3]).FromJust();
                VersionResponse* obj = new VersionResponse(major, minor, build, revis);
                obj->Wrap(info.This());
                info.GetReturnValue().Set(info.This());
                return;
            }

            v8::Local<v8::Function> cons = Nan::New(constructor());
            const int argc = 4;
            v8::Local<v8::Value> argv[argc] = {info[0],info[1],info[2],info[3]};
            info.GetReturnValue().Set(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
        }

        static NAN_METHOD(GetMajor) {
            VersionResponse* obj = ObjectWrap::Unwrap<VersionResponse>(info.Holder());
            info.GetReturnValue().Set(obj->major_);
        }

        static NAN_METHOD(GetMinor) {
            VersionResponse* obj = ObjectWrap::Unwrap<VersionResponse>(info.Holder());
            info.GetReturnValue().Set(obj->minor_);
        }

        static NAN_METHOD(GetBuild) {
            VersionResponse* obj = ObjectWrap::Unwrap<VersionResponse>(info.Holder());
            info.GetReturnValue().Set(obj->build_);
        }

        static NAN_METHOD(GetRevision) {
            VersionResponse* obj = ObjectWrap::Unwrap<VersionResponse>(info.Holder());
            info.GetReturnValue().Set(obj->revision_);
        }

        static inline Nan::Persistent<v8::Function> & constructor() {
            static Nan::Persistent<v8::Function> my_constructor;
            return my_constructor;
        }

        int32_t major_;
        int32_t minor_;
        int32_t build_;
        int32_t revision_;
};

NAN_METHOD(getFileVersion)
{
    Nan::Utf8String pathNan(info[0]->ToString());
    std::string pathStd = std::string(*pathNan);
    LPCSTR path = (LPCSTR)pathStd.c_str();

    DWORD  verHandle = 0;
    UINT   size      = 0;
    LPBYTE lpBuffer  = NULL;
    DWORD  verSize   = GetFileVersionInfoSize(path, &verHandle);

    if (verSize != NULL)
    {
        LPSTR verData = new char[verSize];

        if (GetFileVersionInfo(path, verHandle, verSize, verData))
        {
            if (VerQueryValue(verData, "\\", (LPVOID*)&lpBuffer, (unsigned int*)&size))
            {
                if (size)
                {
                    VS_FIXEDFILEINFO* verInfo = (VS_FIXEDFILEINFO*)lpBuffer;
                    if (verInfo->dwSignature == 0xfeef04bd) {
                        int32_t major = HIWORD(verInfo->dwFileVersionMS);
                        int32_t minor = LOWORD(verInfo->dwFileVersionMS);
                        int32_t build = HIWORD(verInfo->dwFileVersionLS);
                        int32_t revis = LOWORD(verInfo->dwFileVersionLS);

                        v8::Local<v8::Object> response = VersionResponse::IntNewInstance(major, minor, build, revis);
                        info.GetReturnValue().Set(response);
                        delete[] verData;
                        return;
                    }
                }
            }

            ErrorExit(TEXT("VerQueryValue"));
            return;
        }

        delete[] verData;
        ErrorExit(TEXT("GetFileVersionInfo"));
        return;
    }

    ErrorExit(TEXT("GetFileVersionInfoSize"));
    return;
}

NAN_MODULE_INIT(Initialize)
{
    NAN_EXPORT(target, getFileVersion);
    
    VersionResponse::Init(target);
}

NODE_MODULE(processutils, Initialize)