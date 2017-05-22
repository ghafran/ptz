#include <node.h>
#include <string>
#include "libuvc/libuvc.h"

namespace ptz {

using v8::Function;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Null;
using v8::Object;
using v8::String;
using v8::Value;
using v8::Undefined;
using v8::Array;
using v8::Integer;
using v8::Context;
using v8::Boolean;

// open close device operations
struct UVCDevice {
        uvc_context_t *ctx;
        uvc_device_t *device;
        uvc_device_handle_t *devicehandle;
        uvc_error_t result;
        const char * error;
        int vendorId;
        int productId;
};

void openDevice(UVCDevice *uvcDevice){

        // initalize uvc
        uvcDevice->result = uvc_init(&uvcDevice->ctx, NULL);
        if (uvcDevice->result != 0) {
                uvc_exit(uvcDevice->ctx);
                std::string error(uvc_strerror(uvcDevice->result));
                uvcDevice->error = error.c_str();
        }

        // get device
        uvcDevice->result = uvc_find_device(uvcDevice->ctx, &uvcDevice->device, uvcDevice->vendorId, uvcDevice->productId, NULL);
        if (uvcDevice->result != 0) {
                uvc_unref_device(uvcDevice->device);
                uvc_exit(uvcDevice->ctx);
                std::string error(uvc_strerror(uvcDevice->result));
                uvcDevice->error = error.c_str();
        }

        // open device
        uvcDevice->result = uvc_open(uvcDevice->device, &uvcDevice->devicehandle);
        if (uvcDevice->result != 0) {
                uvc_close(uvcDevice->devicehandle);
                uvc_unref_device(uvcDevice->device);
                uvc_exit(uvcDevice->ctx);
                std::string error(uvc_strerror(uvcDevice->result));
                uvcDevice->error = error.c_str();
        }
}

void closeDevice(UVCDevice *uvcDevice){
        uvc_close(uvcDevice->devicehandle);
        uvc_unref_device(uvcDevice->device);
        uvc_exit(uvcDevice->ctx);
}


// device operation support check
struct DeviceCapability {
        int absolute_zoom;
        int relative_zoom;
        int absolute_pan_tilt;
        int relative_pan_tilt;
        int absolute_roll;
        int relative_roll;
};

struct DeviceCapability getDeviceCapability(struct UVCDevice *uvcDevice){

        struct DeviceCapability deviceCapability;
        enum uvc_req_code requestCode;
        uvc_error_t res;
        int32_t v1;
        int32_t v2;
        uint16_t v3;
        int8_t v4;
        uint8_t v5;
        uint8_t v6;
        uint8_t v7;
        int16_t v8;
        int8_t v10;

        requestCode = UVC_GET_DEF;
        res = uvc_get_zoom_abs(uvcDevice->devicehandle, &v3, requestCode);
        deviceCapability.absolute_zoom = res == 0 ? 1 : 0;

        requestCode = UVC_GET_DEF;
        res = uvc_get_zoom_rel(uvcDevice->devicehandle, &v4, &v5, &v6, requestCode);
        deviceCapability.relative_zoom = res == 0 ? 1 : 0;

        requestCode = UVC_GET_DEF;
        res = uvc_get_pantilt_abs(uvcDevice->devicehandle, &v1, &v2, requestCode);
        deviceCapability.absolute_pan_tilt = res == 0 ? 1 : 0;

        requestCode = UVC_GET_DEF;
        res = uvc_get_pantilt_rel(uvcDevice->devicehandle, &v4, &v5, &v10, &v7, requestCode);
        deviceCapability.relative_pan_tilt = res == 0 ? 1 : 0;

        requestCode = UVC_GET_DEF;
        res = uvc_get_roll_abs(uvcDevice->devicehandle, &v8, requestCode);
        deviceCapability.absolute_roll = res == 0 ? 1 : 0;

        requestCode = UVC_GET_DEF;
        res = uvc_get_roll_rel(uvcDevice->devicehandle, &v4, &v5, requestCode);
        deviceCapability.relative_roll = res == 0 ? 1 : 0;

        return deviceCapability;
}

// absolute zoom operations
struct AbsoluteZoomInfo {
        uint16_t min;
        uint16_t max;
        uint16_t resolution;
        uint16_t current;
        uint16_t def;
        uvc_error_t result;
        const char * error;
};

void getAbsoluteZoomInfo(struct UVCDevice *uvcDevice, struct AbsoluteZoomInfo *absoluteZoomInfo){
        enum uvc_req_code requestCode;

        requestCode = UVC_GET_MIN;
        absoluteZoomInfo->result = uvc_get_zoom_abs(uvcDevice->devicehandle, &absoluteZoomInfo->min, requestCode);
        if (absoluteZoomInfo->result != 0) {
                std::string error(uvc_strerror(absoluteZoomInfo->result));
                absoluteZoomInfo->error = error.c_str();
                return;
        }

        requestCode = UVC_GET_MAX;
        absoluteZoomInfo->result = uvc_get_zoom_abs(uvcDevice->devicehandle, &absoluteZoomInfo->max, requestCode);
        if (absoluteZoomInfo->result != 0) {
                std::string error(uvc_strerror(absoluteZoomInfo->result));
                absoluteZoomInfo->error = error.c_str();
                return;
        }

        requestCode = UVC_GET_RES;
        absoluteZoomInfo->result = uvc_get_zoom_abs(uvcDevice->devicehandle, &absoluteZoomInfo->resolution, requestCode);
        if (absoluteZoomInfo->result != 0) {
                std::string error(uvc_strerror(absoluteZoomInfo->result));
                absoluteZoomInfo->error = error.c_str();
                return;
        }

        requestCode = UVC_GET_DEF;
        absoluteZoomInfo->result = uvc_get_zoom_abs(uvcDevice->devicehandle, &absoluteZoomInfo->def, requestCode);
        if (absoluteZoomInfo->result != 0) {
                std::string error(uvc_strerror(absoluteZoomInfo->result));
                absoluteZoomInfo->error = error.c_str();
                return;
        }

        requestCode = UVC_GET_CUR;
        absoluteZoomInfo->result = uvc_get_zoom_abs(uvcDevice->devicehandle, &absoluteZoomInfo->current, requestCode);
        if (absoluteZoomInfo->result != 0) {
                std::string error(uvc_strerror(absoluteZoomInfo->result));
                absoluteZoomInfo->error = error.c_str();
                return;
        }
}

struct AbsoluteZoom {
        uint16_t zoom;
        uvc_error_t result;
        const char * error;
};

void setAbsoluteZoom(struct UVCDevice *uvcDevice, struct AbsoluteZoom *absoluteZoom){
        absoluteZoom->result = uvc_set_zoom_abs(uvcDevice->devicehandle, absoluteZoom->zoom);
        if (absoluteZoom->result != 0) {
                std::string error(uvc_strerror(absoluteZoom->result));
                absoluteZoom->error = error.c_str();
                return;
        }
}

// relative zoom operations
struct RelativeZoomInfo {
        int8_t direction;
        uint8_t digital_zoom;
        uint8_t min_speed;
        uint8_t max_speed;
        uint8_t resolution_speed;
        uint8_t current_speed;
        uint8_t default_speed;
        uvc_error_t result;
        const char * error;
};

void getRelativeZoomInfo(struct UVCDevice *uvcDevice, struct RelativeZoomInfo *relativeZoomInfo){
        enum uvc_req_code requestCode;

        requestCode = UVC_GET_MIN;
        relativeZoomInfo->result = uvc_get_zoom_rel(uvcDevice->devicehandle, &relativeZoomInfo->direction, &relativeZoomInfo->digital_zoom, &relativeZoomInfo->min_speed, requestCode);
        if (relativeZoomInfo->result != 0) {
                std::string error(uvc_strerror(relativeZoomInfo->result));
                relativeZoomInfo->error = error.c_str();
                return;
        }

        requestCode = UVC_GET_MAX;
        relativeZoomInfo->result = uvc_get_zoom_rel(uvcDevice->devicehandle, &relativeZoomInfo->direction, &relativeZoomInfo->digital_zoom, &relativeZoomInfo->max_speed, requestCode);
        if (relativeZoomInfo->result != 0) {
                std::string error(uvc_strerror(relativeZoomInfo->result));
                relativeZoomInfo->error = error.c_str();
                return;
        }

        requestCode = UVC_GET_RES;
        relativeZoomInfo->result = uvc_get_zoom_rel(uvcDevice->devicehandle, &relativeZoomInfo->direction, &relativeZoomInfo->digital_zoom, &relativeZoomInfo->resolution_speed, requestCode);
        if (relativeZoomInfo->result != 0) {
                std::string error(uvc_strerror(relativeZoomInfo->result));
                relativeZoomInfo->error = error.c_str();
                return;
        }

        requestCode = UVC_GET_DEF;
        relativeZoomInfo->result = uvc_get_zoom_rel(uvcDevice->devicehandle, &relativeZoomInfo->direction, &relativeZoomInfo->digital_zoom, &relativeZoomInfo->default_speed, requestCode);
        if (relativeZoomInfo->result != 0) {
                std::string error(uvc_strerror(relativeZoomInfo->result));
                relativeZoomInfo->error = error.c_str();
                return;
        }

        requestCode = UVC_GET_CUR;
        relativeZoomInfo->result = uvc_get_zoom_rel(uvcDevice->devicehandle, &relativeZoomInfo->direction, &relativeZoomInfo->digital_zoom, &relativeZoomInfo->current_speed, requestCode);
        if (relativeZoomInfo->result != 0) {
                std::string error(uvc_strerror(relativeZoomInfo->result));
                relativeZoomInfo->error = error.c_str();
                return;
        }
}

struct RelativeZoom {
        int8_t direction;
        int8_t speed;
        uvc_error_t result;
        const char * error;
};

void setRelativeZoom(struct UVCDevice *uvcDevice, struct RelativeZoom *relativeZoom){
        relativeZoom->result = uvc_set_zoom_rel(uvcDevice->devicehandle, relativeZoom->direction, 1, relativeZoom->speed);
        if (relativeZoom->result != 0) {
                std::string error(uvc_strerror(relativeZoom->result));
                relativeZoom->error = error.c_str();
                return;
        }
}

// absolute pan tilt operations
struct AbsolutePanTiltInfo {
        int32_t min_pan;
        int32_t min_tilt;
        int32_t max_pan;
        int32_t max_tilt;
        int32_t resolution_pan;
        int32_t resolution_tilt;
        int32_t current_pan;
        int32_t current_tilt;
        int32_t default_pan;
        int32_t default_tilt;
        uvc_error_t result;
        const char * error;
};

void getAbsolutePanTiltInfo(struct UVCDevice *uvcDevice, struct AbsolutePanTiltInfo *absolutePanTiltInfo){
        enum uvc_req_code requestCode;

        requestCode = UVC_GET_MIN;
        absolutePanTiltInfo->result = uvc_get_pantilt_abs(uvcDevice->devicehandle, &absolutePanTiltInfo->min_pan, &absolutePanTiltInfo->min_tilt, requestCode);
        if (absolutePanTiltInfo->result != 0) {
                std::string error(uvc_strerror(absolutePanTiltInfo->result));
                absolutePanTiltInfo->error = error.c_str();
                return;
        }

        requestCode = UVC_GET_MAX;
        absolutePanTiltInfo->result  = uvc_get_pantilt_abs(uvcDevice->devicehandle, &absolutePanTiltInfo->max_pan, &absolutePanTiltInfo->max_pan, requestCode);
        if (absolutePanTiltInfo->result != 0) {
                std::string error(uvc_strerror(absolutePanTiltInfo->result));
                absolutePanTiltInfo->error = error.c_str();
                return;
        }

        requestCode = UVC_GET_RES;
        absolutePanTiltInfo->result  = uvc_get_pantilt_abs(uvcDevice->devicehandle, &absolutePanTiltInfo->resolution_pan, &absolutePanTiltInfo->resolution_tilt, requestCode);
        if (absolutePanTiltInfo->result != 0) {
                std::string error(uvc_strerror(absolutePanTiltInfo->result));
                absolutePanTiltInfo->error = error.c_str();
                return;
        }

        requestCode = UVC_GET_DEF;
        absolutePanTiltInfo->result  = uvc_get_pantilt_abs(uvcDevice->devicehandle, &absolutePanTiltInfo->default_pan, &absolutePanTiltInfo->default_tilt, requestCode);
        if (absolutePanTiltInfo->result != 0) {
                std::string error(uvc_strerror(absolutePanTiltInfo->result));
                absolutePanTiltInfo->error = error.c_str();
                return;
        }

        requestCode = UVC_GET_CUR;
        absolutePanTiltInfo->result  = uvc_get_pantilt_abs(uvcDevice->devicehandle, &absolutePanTiltInfo->current_pan, &absolutePanTiltInfo->current_tilt, requestCode);
        if (absolutePanTiltInfo->result != 0) {
                std::string error(uvc_strerror(absolutePanTiltInfo->result));
                absolutePanTiltInfo->error = error.c_str();
                return;
        }
}

struct AbsolutePanTilt {
        int32_t pan;
        int32_t tilt;
        uvc_error_t result;
        const char * error;
};

void setAbsolutePanTilt(struct UVCDevice *uvcDevice, struct AbsolutePanTilt *absolutePanTilt){
        absolutePanTilt->result = uvc_set_pantilt_abs(uvcDevice->devicehandle, absolutePanTilt->pan, absolutePanTilt->tilt);
        if (absolutePanTilt->result != 0) {
                std::string error(uvc_strerror(absolutePanTilt->result));
                absolutePanTilt->error = error.c_str();
                return;
        }
}

// relative pan tilt operations
struct RelativePanTiltInfo {
        int8_t pan_direction;
        int8_t tilt_direction;
        uint8_t min_pan_speed;
        uint8_t min_tilt_speed;
        uint8_t max_pan_speed;
        uint8_t max_tilt_speed;
        uint8_t resolution_pan_speed;
        uint8_t resolution_tilt_speed;
        uint8_t default_pan_speed;
        uint8_t default_tilt_speed;
        uint8_t current_pan_speed;
        uint8_t current_tilt_speed;
        uvc_error_t result;
        const char * error;
};

void getRelativePanTiltInfo(struct UVCDevice *uvcDevice, struct RelativePanTiltInfo *relativePanTiltInfo){
        enum uvc_req_code requestCode;

        requestCode = UVC_GET_MIN;
        relativePanTiltInfo->result = uvc_get_pantilt_rel(uvcDevice->devicehandle, &relativePanTiltInfo->pan_direction, &relativePanTiltInfo->min_pan_speed, &relativePanTiltInfo->tilt_direction, &relativePanTiltInfo->min_tilt_speed, requestCode);
        if (relativePanTiltInfo->result != 0) {
                std::string error(uvc_strerror(relativePanTiltInfo->result));
                relativePanTiltInfo->error = error.c_str();
                return;
        }

        requestCode = UVC_GET_MAX;
        relativePanTiltInfo->result = uvc_get_pantilt_rel(uvcDevice->devicehandle, &relativePanTiltInfo->pan_direction, &relativePanTiltInfo->max_pan_speed, &relativePanTiltInfo->tilt_direction, &relativePanTiltInfo->max_tilt_speed, requestCode);
        if (relativePanTiltInfo->result != 0) {
                std::string error(uvc_strerror(relativePanTiltInfo->result));
                relativePanTiltInfo->error = error.c_str();
                return;
        }

        requestCode = UVC_GET_RES;
        relativePanTiltInfo->result = uvc_get_pantilt_rel(uvcDevice->devicehandle, &relativePanTiltInfo->pan_direction, &relativePanTiltInfo->resolution_pan_speed, &relativePanTiltInfo->tilt_direction, &relativePanTiltInfo->resolution_tilt_speed, requestCode);
        if (relativePanTiltInfo->result != 0) {
                std::string error(uvc_strerror(relativePanTiltInfo->result));
                relativePanTiltInfo->error = error.c_str();
                return;
        }

        requestCode = UVC_GET_DEF;
        relativePanTiltInfo->result = uvc_get_pantilt_rel(uvcDevice->devicehandle, &relativePanTiltInfo->pan_direction, &relativePanTiltInfo->default_pan_speed, &relativePanTiltInfo->tilt_direction, &relativePanTiltInfo->default_tilt_speed, requestCode);
        if (relativePanTiltInfo->result != 0) {
                std::string error(uvc_strerror(relativePanTiltInfo->result));
                relativePanTiltInfo->error = error.c_str();
                return;
        }

        requestCode = UVC_GET_CUR;
        relativePanTiltInfo->result = uvc_get_pantilt_rel(uvcDevice->devicehandle, &relativePanTiltInfo->pan_direction, &relativePanTiltInfo->current_pan_speed, &relativePanTiltInfo->tilt_direction, &relativePanTiltInfo->current_tilt_speed, requestCode);
        if (relativePanTiltInfo->result != 0) {
                std::string error(uvc_strerror(relativePanTiltInfo->result));
                relativePanTiltInfo->error = error.c_str();
                return;
        }
}

struct RelativePanTillt {
        int8_t pan_direction;
        uint8_t pan_speed;
        int8_t tilt_direction;
        uint8_t tilt_speed;
        uvc_error_t result;
        const char * error;
};

void setRelativePanTilt(struct UVCDevice *uvcDevice, struct RelativePanTillt *relativePanTillt){
        relativePanTillt->result = uvc_set_pantilt_rel(uvcDevice->devicehandle, relativePanTillt->pan_direction, relativePanTillt->pan_speed, relativePanTillt->tilt_direction, relativePanTillt->tilt_speed);
        if (relativePanTillt->result != 0) {
                std::string error(uvc_strerror(relativePanTillt->result));
                relativePanTillt->error = error.c_str();
                return;
        }
}

// node binding functions
void listDevices(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        Local<Function> cb = Local<Function>::Cast(args[0]);

        // get device list
        uvc_context_t *ctx;
        uvc_device_t **devices;
        uvc_device_descriptor_t *descriptor;
        uvc_error_t res;

        // initalize uvc
        res = uvc_init(&ctx, NULL);
        if (res != 0) {
                uvc_exit(ctx);
                std::string error(uvc_strerror(res));
                Local<Value> argv[2] = { String::NewFromUtf8(isolate, error.c_str()), Undefined(isolate) };
                cb->Call(Null(isolate), 2, argv);
                return;
        }

        // get devices list
        res = uvc_get_device_list(ctx, &devices);
        if (res != 0) {
                uvc_exit(ctx);
                std::string error(uvc_strerror(res));
                Local<Value> argv[2] = { String::NewFromUtf8(isolate, error.c_str()), Undefined(isolate) };
                cb->Call(Null(isolate), 2, argv);
                return;
        }

        // create output result
        Local<Array> jsDevices = Array::New(isolate);
        int devicesCount = sizeof(devices) / sizeof(devices[0]);
        for(int i = 0; i < devicesCount; i++) {
                res = uvc_get_device_descriptor(devices[i], &descriptor);
                if (res != 0) {
                        uvc_free_device_list(devices, 1);
                        uvc_exit(ctx);
                        std::string error(uvc_strerror(res));
                        Local<Value> argv[2] = { String::NewFromUtf8(isolate, error.c_str()), Undefined(isolate) };
                        cb->Call(Null(isolate), 2, argv);
                        return;
                }
                Local<Object> jsDevice = Object::New(isolate);
                jsDevice->Set(String::NewFromUtf8(isolate, "vendorId"), Integer::New(isolate, descriptor->idVendor));
                jsDevice->Set(String::NewFromUtf8(isolate, "productId"), Integer::New(isolate, descriptor->idProduct));
                jsDevice->Set(String::NewFromUtf8(isolate, "serialNumber"), String::NewFromUtf8(isolate, descriptor->serialNumber));
                jsDevice->Set(String::NewFromUtf8(isolate, "manufacturer"), String::NewFromUtf8(isolate, descriptor->manufacturer));
                jsDevice->Set(String::NewFromUtf8(isolate, "product"), String::NewFromUtf8(isolate, descriptor->product));
                jsDevices->Set(i, jsDevice);

                uvc_free_device_descriptor(descriptor);
        }
        uvc_free_device_list(devices, 1);
        uvc_exit(ctx);

        const unsigned argc = 2;
        Local<Value> argv[argc] = { Undefined(isolate), jsDevices };
        cb->Call(Null(isolate), argc, argv);
}

void getCapabilities(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        Local<Object> input = Local<Object>::Cast(args[0]);
        Local<Function> cb = Local<Function>::Cast(args[1]);
        Local<Context> context = isolate->GetCurrentContext();

        // get options
        Local<Value> vendorId = input->Get(context, String::NewFromUtf8(isolate, "vendorId")).ToLocalChecked();
        Local<Value> productId = input->Get(context, String::NewFromUtf8(isolate, "productId")).ToLocalChecked();

        struct UVCDevice uvcDevice;
        uvcDevice.vendorId = vendorId->Int32Value();
        uvcDevice.productId = productId->Int32Value();

        // open device
        openDevice(&uvcDevice);
        if (uvcDevice.result != 0) {
                Local<Value> argv[2] = { String::NewFromUtf8(isolate, uvcDevice.error), Undefined(isolate) };
                cb->Call(Null(isolate), 2, argv);
                return;
        }
        // printf("==original2===%p====", uvcDevice.devicehandle);

        // get capabilities
        struct DeviceCapability deviceCapability;
        deviceCapability = getDeviceCapability(&uvcDevice);

        // create output result
        Local<Object> jsDevice = Object::New(isolate);
        jsDevice->Set(String::NewFromUtf8(isolate, "absoluteZoom"), Boolean::New(isolate, deviceCapability.absolute_zoom));
        jsDevice->Set(String::NewFromUtf8(isolate, "relativeZoom"), Boolean::New(isolate, deviceCapability.relative_zoom));
        jsDevice->Set(String::NewFromUtf8(isolate, "absolutePanTilt"), Boolean::New(isolate, deviceCapability.absolute_pan_tilt));
        jsDevice->Set(String::NewFromUtf8(isolate, "relativePanTilt"), Boolean::New(isolate, deviceCapability.relative_pan_tilt));
        jsDevice->Set(String::NewFromUtf8(isolate, "absoluteRoll"), Boolean::New(isolate, deviceCapability.absolute_roll));
        jsDevice->Set(String::NewFromUtf8(isolate, "relativeRoll"), Boolean::New(isolate, deviceCapability.relative_roll));

        // cleanup
        closeDevice(&uvcDevice);

        // trigger callback
        Local<Value> argv[2] = { Undefined(isolate), jsDevice };
        cb->Call(Null(isolate), 2, argv);
}

void getAbsoluteZoom(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        Local<Object> input = Local<Object>::Cast(args[0]);
        Local<Function> cb = Local<Function>::Cast(args[1]);
        Local<Context> context = isolate->GetCurrentContext();

        // get options
        Local<Value> vendorId = input->Get(context, String::NewFromUtf8(isolate, "vendorId")).ToLocalChecked();
        Local<Value> productId = input->Get(context, String::NewFromUtf8(isolate, "productId")).ToLocalChecked();

        struct UVCDevice uvcDevice;
        uvcDevice.vendorId = vendorId->Int32Value();
        uvcDevice.productId = productId->Int32Value();

        // open device
        openDevice(&uvcDevice);
        if (uvcDevice.result != 0) {
                Local<Value> argv[2] = { String::NewFromUtf8(isolate, uvcDevice.error), Undefined(isolate) };
                cb->Call(Null(isolate), 2, argv);
                return;
        }

        // get zoom info
        struct AbsoluteZoomInfo absoluteZoomInfo;
        getAbsoluteZoomInfo(&uvcDevice, &absoluteZoomInfo);
        if (absoluteZoomInfo.result != 0) {
                closeDevice(&uvcDevice);
                Local<Value> argv[2] = { String::NewFromUtf8(isolate, absoluteZoomInfo.error), Undefined(isolate) };
                cb->Call(Null(isolate), 2, argv);
                return;
        }

        // create output result
        Local<Object> jsDevice = Object::New(isolate);
        jsDevice->Set(String::NewFromUtf8(isolate, "min"), Boolean::New(isolate, absoluteZoomInfo.min));
        jsDevice->Set(String::NewFromUtf8(isolate, "max"), Boolean::New(isolate, absoluteZoomInfo.max));
        jsDevice->Set(String::NewFromUtf8(isolate, "resolution"), Boolean::New(isolate, absoluteZoomInfo.resolution));
        jsDevice->Set(String::NewFromUtf8(isolate, "current"), Boolean::New(isolate, absoluteZoomInfo.current));
        jsDevice->Set(String::NewFromUtf8(isolate, "default"), Boolean::New(isolate, absoluteZoomInfo.def));

        // cleanup
        closeDevice(&uvcDevice);

        // trigger callback
        Local<Value> argv[2] = { Undefined(isolate), jsDevice };
        cb->Call(Null(isolate), 2, argv);
}

void absoluteZoom(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        Local<Object> input = Local<Object>::Cast(args[0]);
        Local<Function> cb = Local<Function>::Cast(args[1]);
        Local<Context> context = isolate->GetCurrentContext();

        // get options
        Local<Value> vendorId = input->Get(context, String::NewFromUtf8(isolate, "vendorId")).ToLocalChecked();
        Local<Value> productId = input->Get(context, String::NewFromUtf8(isolate, "productId")).ToLocalChecked();
        Local<Value> zoom = input->Get(context, String::NewFromUtf8(isolate, "zoom")).ToLocalChecked();

        struct UVCDevice uvcDevice;
        uvcDevice.vendorId = vendorId->Int32Value();
        uvcDevice.productId = productId->Int32Value();

        // open device
        openDevice(&uvcDevice);
        if (uvcDevice.result != 0) {
                Local<Value> argv[2] = { String::NewFromUtf8(isolate, uvcDevice.error), Undefined(isolate) };
                cb->Call(Null(isolate), 2, argv);
                return;
        }

        // get zoom info
        struct AbsoluteZoom absoluteZoom;
        absoluteZoom.zoom = zoom->Int32Value();
        setAbsoluteZoom(&uvcDevice, &absoluteZoom);
        if (absoluteZoom.result != 0) {
                closeDevice(&uvcDevice);
                Local<Value> argv[2] = { String::NewFromUtf8(isolate, absoluteZoom.error), Undefined(isolate) };
                cb->Call(Null(isolate), 2, argv);
                return;
        }

        // cleanup
        closeDevice(&uvcDevice);

        // trigger callback
        Local<Value> argv[1] = { Undefined(isolate) };
        cb->Call(Null(isolate), 1, argv);
}

void getRelativeZoom(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        Local<Object> input = Local<Object>::Cast(args[0]);
        Local<Function> cb = Local<Function>::Cast(args[1]);
        Local<Context> context = isolate->GetCurrentContext();

        // get options
        Local<Value> vendorId = input->Get(context, String::NewFromUtf8(isolate, "vendorId")).ToLocalChecked();
        Local<Value> productId = input->Get(context, String::NewFromUtf8(isolate, "productId")).ToLocalChecked();

        struct UVCDevice uvcDevice;
        uvcDevice.vendorId = vendorId->Int32Value();
        uvcDevice.productId = productId->Int32Value();

        // open device
        openDevice(&uvcDevice);
        if (uvcDevice.result != 0) {
                Local<Value> argv[2] = { String::NewFromUtf8(isolate, uvcDevice.error), Undefined(isolate) };
                cb->Call(Null(isolate), 2, argv);
                return;
        }

        // get zoom info
        struct RelativeZoomInfo relativeZoomInfo;
        getRelativeZoomInfo(&uvcDevice, &relativeZoomInfo);
        if (relativeZoomInfo.result != 0) {
                closeDevice(&uvcDevice);
                Local<Value> argv[2] = { String::NewFromUtf8(isolate, relativeZoomInfo.error), Undefined(isolate) };
                cb->Call(Null(isolate), 2, argv);
                return;
        }

        // create output result
        Local<Object> result = Object::New(isolate);
        result->Set(String::NewFromUtf8(isolate, "direction"), Integer::New(isolate, relativeZoomInfo.direction));
        result->Set(String::NewFromUtf8(isolate, "digitalZoom"), Boolean::New(isolate, relativeZoomInfo.digital_zoom));
        result->Set(String::NewFromUtf8(isolate, "minSpeed"), Integer::New(isolate, relativeZoomInfo.min_speed));
        result->Set(String::NewFromUtf8(isolate, "maxSpeed"), Integer::New(isolate, relativeZoomInfo.max_speed));
        result->Set(String::NewFromUtf8(isolate, "resolutionSpeed"), Integer::New(isolate, relativeZoomInfo.resolution_speed));
        result->Set(String::NewFromUtf8(isolate, "currentSpeed"), Integer::New(isolate, relativeZoomInfo.current_speed));
        result->Set(String::NewFromUtf8(isolate, "defaultSpeed"), Integer::New(isolate, relativeZoomInfo.default_speed));

        // cleanup
        closeDevice(&uvcDevice);

        // trigger callback
        Local<Value> argv[2] = { Undefined(isolate), result };
        cb->Call(Null(isolate), 2, argv);
}

void relativeZoom(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        Local<Object> input = Local<Object>::Cast(args[0]);
        Local<Function> cb = Local<Function>::Cast(args[1]);
        Local<Context> context = isolate->GetCurrentContext();

        // get options
        Local<Value> vendorId = input->Get(context, String::NewFromUtf8(isolate, "vendorId")).ToLocalChecked();
        Local<Value> productId = input->Get(context, String::NewFromUtf8(isolate, "productId")).ToLocalChecked();
        Local<Value> direction = input->Get(context, String::NewFromUtf8(isolate, "direction")).ToLocalChecked();
        Local<Value> speed = input->Get(context, String::NewFromUtf8(isolate, "speed")).ToLocalChecked();

        struct UVCDevice uvcDevice;
        uvcDevice.vendorId = vendorId->Int32Value();
        uvcDevice.productId = productId->Int32Value();

        // open device
        openDevice(&uvcDevice);
        if (uvcDevice.result != 0) {
                Local<Value> argv[2] = { String::NewFromUtf8(isolate, uvcDevice.error), Undefined(isolate) };
                cb->Call(Null(isolate), 2, argv);
                return;
        }

        // get zoom info
        struct RelativeZoom relativeZoom;
        relativeZoom.direction = direction->Int32Value();
        relativeZoom.speed = speed->Int32Value();
        setRelativeZoom(&uvcDevice, &relativeZoom);
        if (relativeZoom.result != 0) {
                closeDevice(&uvcDevice);
                Local<Value> argv[2] = { String::NewFromUtf8(isolate, relativeZoom.error), Undefined(isolate) };
                cb->Call(Null(isolate), 2, argv);
                return;
        }

        // cleanup
        closeDevice(&uvcDevice);

        // trigger callback
        Local<Value> argv[1] = { Undefined(isolate) };
        cb->Call(Null(isolate), 1, argv);
}

void getAbsolutePanTilt(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        Local<Object> input = Local<Object>::Cast(args[0]);
        Local<Function> cb = Local<Function>::Cast(args[1]);
        Local<Context> context = isolate->GetCurrentContext();

        // get options
        Local<Value> vendorId = input->Get(context, String::NewFromUtf8(isolate, "vendorId")).ToLocalChecked();
        Local<Value> productId = input->Get(context, String::NewFromUtf8(isolate, "productId")).ToLocalChecked();

        struct UVCDevice uvcDevice;
        uvcDevice.vendorId = vendorId->Int32Value();
        uvcDevice.productId = productId->Int32Value();

        // open device
        openDevice(&uvcDevice);
        if (uvcDevice.result != 0) {
                Local<Value> argv[2] = { String::NewFromUtf8(isolate, uvcDevice.error), Undefined(isolate) };
                cb->Call(Null(isolate), 2, argv);
                return;
        }

        // get pan tilt info
        struct AbsolutePanTiltInfo absolutePanTiltInfo;
        getAbsolutePanTiltInfo(&uvcDevice, &absolutePanTiltInfo);
        if (absolutePanTiltInfo.result != 0) {
                closeDevice(&uvcDevice);
                Local<Value> argv[2] = { String::NewFromUtf8(isolate, absolutePanTiltInfo.error), Undefined(isolate) };
                cb->Call(Null(isolate), 2, argv);
                return;
        }

        // create output result
        Local<Object> result = Object::New(isolate);
        result->Set(String::NewFromUtf8(isolate, "minPan"), Integer::New(isolate, absolutePanTiltInfo.min_pan));
        result->Set(String::NewFromUtf8(isolate, "minTilt"), Integer::New(isolate, absolutePanTiltInfo.min_tilt));
        result->Set(String::NewFromUtf8(isolate, "maxPan"), Integer::New(isolate, absolutePanTiltInfo.max_pan));
        result->Set(String::NewFromUtf8(isolate, "maxTilt"), Integer::New(isolate, absolutePanTiltInfo.max_tilt));
        result->Set(String::NewFromUtf8(isolate, "resolutionPan"), Integer::New(isolate, absolutePanTiltInfo.resolution_pan));
        result->Set(String::NewFromUtf8(isolate, "resolutionTilt"), Integer::New(isolate, absolutePanTiltInfo.resolution_tilt));
        result->Set(String::NewFromUtf8(isolate, "currentPan"), Integer::New(isolate, absolutePanTiltInfo.current_pan));
        result->Set(String::NewFromUtf8(isolate, "currentTilt"), Integer::New(isolate, absolutePanTiltInfo.current_tilt));
        result->Set(String::NewFromUtf8(isolate, "defaultPan"), Integer::New(isolate, absolutePanTiltInfo.default_pan));
        result->Set(String::NewFromUtf8(isolate, "defaultTilt"), Integer::New(isolate, absolutePanTiltInfo.default_tilt));

        // cleanup
        closeDevice(&uvcDevice);

        // trigger callback
        Local<Value> argv[2] = { Undefined(isolate), result };
        cb->Call(Null(isolate), 2, argv);
}

void absolutePanTilt(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        Local<Object> input = Local<Object>::Cast(args[0]);
        Local<Function> cb = Local<Function>::Cast(args[1]);
        Local<Context> context = isolate->GetCurrentContext();

        // get options
        Local<Value> vendorId = input->Get(context, String::NewFromUtf8(isolate, "vendorId")).ToLocalChecked();
        Local<Value> productId = input->Get(context, String::NewFromUtf8(isolate, "productId")).ToLocalChecked();
        Local<Value> pan = input->Get(context, String::NewFromUtf8(isolate, "pan")).ToLocalChecked();
        Local<Value> tilt = input->Get(context, String::NewFromUtf8(isolate, "tilt")).ToLocalChecked();

        struct UVCDevice uvcDevice;
        uvcDevice.vendorId = vendorId->Int32Value();
        uvcDevice.productId = productId->Int32Value();

        // open device
        openDevice(&uvcDevice);
        if (uvcDevice.result != 0) {
                Local<Value> argv[2] = { String::NewFromUtf8(isolate, uvcDevice.error), Undefined(isolate) };
                cb->Call(Null(isolate), 2, argv);
                return;
        }

        // get zoom info
        struct AbsolutePanTilt absolutePanTilt;
        absolutePanTilt.pan = pan->Int32Value();
        absolutePanTilt.tilt = tilt->Int32Value();
        setAbsolutePanTilt(&uvcDevice, &absolutePanTilt);
        if (absolutePanTilt.result != 0) {
                closeDevice(&uvcDevice);
                Local<Value> argv[2] = { String::NewFromUtf8(isolate, absolutePanTilt.error), Undefined(isolate) };
                cb->Call(Null(isolate), 2, argv);
                return;
        }

        // cleanup
        closeDevice(&uvcDevice);

        // trigger callback
        Local<Value> argv[1] = { Undefined(isolate) };
        cb->Call(Null(isolate), 1, argv);
}

void getRelativePanTilt(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        Local<Object> input = Local<Object>::Cast(args[0]);
        Local<Function> cb = Local<Function>::Cast(args[1]);
        Local<Context> context = isolate->GetCurrentContext();

        // get options
        Local<Value> vendorId = input->Get(context, String::NewFromUtf8(isolate, "vendorId")).ToLocalChecked();
        Local<Value> productId = input->Get(context, String::NewFromUtf8(isolate, "productId")).ToLocalChecked();

        struct UVCDevice uvcDevice;
        uvcDevice.vendorId = vendorId->Int32Value();
        uvcDevice.productId = productId->Int32Value();

        // open device
        openDevice(&uvcDevice);
        if (uvcDevice.result != 0) {
                Local<Value> argv[2] = { String::NewFromUtf8(isolate, uvcDevice.error), Undefined(isolate) };
                cb->Call(Null(isolate), 2, argv);
                return;
        }

        // get zoom info
        struct RelativePanTiltInfo relativePanTiltInfo;
        getRelativePanTiltInfo(&uvcDevice, &relativePanTiltInfo);
        if (relativePanTiltInfo.result != 0) {
                closeDevice(&uvcDevice);
                Local<Value> argv[2] = { String::NewFromUtf8(isolate, relativePanTiltInfo.error), Undefined(isolate) };
                cb->Call(Null(isolate), 2, argv);
                return;
        }

        // create output result
        Local<Object> result = Object::New(isolate);
        result->Set(String::NewFromUtf8(isolate, "panDirection"), Integer::New(isolate, relativePanTiltInfo.pan_direction));
        result->Set(String::NewFromUtf8(isolate, "tiltDirection"), Integer::New(isolate, relativePanTiltInfo.tilt_direction));
        result->Set(String::NewFromUtf8(isolate, "minPanSpeed"), Integer::New(isolate, relativePanTiltInfo.min_pan_speed));
        result->Set(String::NewFromUtf8(isolate, "minTiltSpeed"), Integer::New(isolate, relativePanTiltInfo.min_tilt_speed));
        result->Set(String::NewFromUtf8(isolate, "maxPanSpeed"), Integer::New(isolate, relativePanTiltInfo.max_pan_speed));
        result->Set(String::NewFromUtf8(isolate, "maxTiltSpeed"), Integer::New(isolate, relativePanTiltInfo.max_tilt_speed));
        result->Set(String::NewFromUtf8(isolate, "resolutionPanSpeed"), Integer::New(isolate, relativePanTiltInfo.resolution_pan_speed));
        result->Set(String::NewFromUtf8(isolate, "resolutionTiltSpeed"), Integer::New(isolate, relativePanTiltInfo.resolution_tilt_speed));
        result->Set(String::NewFromUtf8(isolate, "defaultPanSpeed"), Integer::New(isolate, relativePanTiltInfo.default_pan_speed));
        result->Set(String::NewFromUtf8(isolate, "defaultTiltSpeed"), Integer::New(isolate, relativePanTiltInfo.default_tilt_speed));
        result->Set(String::NewFromUtf8(isolate, "currentPanSpeed"), Integer::New(isolate, relativePanTiltInfo.current_pan_speed));
        result->Set(String::NewFromUtf8(isolate, "currentTiltSpeed"), Integer::New(isolate, relativePanTiltInfo.current_tilt_speed));

        // cleanup
        closeDevice(&uvcDevice);

        // trigger callback
        Local<Value> argv[2] = { Undefined(isolate), result };
        cb->Call(Null(isolate), 2, argv);
}

void relativePanTilt(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        Local<Object> input = Local<Object>::Cast(args[0]);
        Local<Function> cb = Local<Function>::Cast(args[1]);
        Local<Context> context = isolate->GetCurrentContext();

        // get options
        Local<Value> vendorId = input->Get(context, String::NewFromUtf8(isolate, "vendorId")).ToLocalChecked();
        Local<Value> productId = input->Get(context, String::NewFromUtf8(isolate, "productId")).ToLocalChecked();
        Local<Value> panDirection = input->Get(context, String::NewFromUtf8(isolate, "panDirection")).ToLocalChecked();
        Local<Value> panSpeed = input->Get(context, String::NewFromUtf8(isolate, "panSpeed")).ToLocalChecked();
        Local<Value> tiltDirection = input->Get(context, String::NewFromUtf8(isolate, "tiltDirection")).ToLocalChecked();
        Local<Value> tiltSpeed = input->Get(context, String::NewFromUtf8(isolate, "tiltSpeed")).ToLocalChecked();

        struct UVCDevice uvcDevice;
        uvcDevice.vendorId = vendorId->Int32Value();
        uvcDevice.productId = productId->Int32Value();

        // open device
        openDevice(&uvcDevice);
        if (uvcDevice.result != 0) {
                Local<Value> argv[2] = { String::NewFromUtf8(isolate, uvcDevice.error), Undefined(isolate) };
                cb->Call(Null(isolate), 2, argv);
                return;
        }

        // get zoom info
        struct RelativePanTillt relativePanTillt;
        relativePanTillt.pan_direction = panDirection->Int32Value();
        relativePanTillt.pan_speed = panSpeed->Int32Value();
        relativePanTillt.tilt_direction = tiltDirection->Int32Value();
        relativePanTillt.tilt_speed = tiltSpeed->Int32Value();
        setRelativePanTilt(&uvcDevice, &relativePanTillt);
        if (relativePanTillt.result != 0) {
                closeDevice(&uvcDevice);
                Local<Value> argv[2] = { String::NewFromUtf8(isolate, relativePanTillt.error), Undefined(isolate) };
                cb->Call(Null(isolate), 2, argv);
                return;
        }

        // cleanup
        closeDevice(&uvcDevice);

        // trigger callback
        Local<Value> argv[1] = { Undefined(isolate) };
        cb->Call(Null(isolate), 1, argv);
}

void Init(Local<Object> exports, Local<Object> module) {
        NODE_SET_METHOD(exports, "listDevices", listDevices);
        NODE_SET_METHOD(exports, "getCapabilities", getCapabilities);
        NODE_SET_METHOD(exports, "getAbsoluteZoom", getAbsoluteZoom);
        NODE_SET_METHOD(exports, "absoluteZoom", absoluteZoom);
        NODE_SET_METHOD(exports, "getRelativeZoom", getRelativeZoom);
        NODE_SET_METHOD(exports, "relativeZoom", relativeZoom);
        NODE_SET_METHOD(exports, "getAbsolutePanTilt", getAbsolutePanTilt);
        NODE_SET_METHOD(exports, "absolutePanTilt", absolutePanTilt);
        NODE_SET_METHOD(exports, "getRelativePanTilt", getRelativePanTilt);
        NODE_SET_METHOD(exports, "relativePanTilt", relativePanTilt);
}

NODE_MODULE(ptz, Init)

}  // namespace ptz
