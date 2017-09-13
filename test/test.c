/*
   cd test
   gcc test.c -o test -luvc;./test;
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libuvc/libuvc.h"

struct PanTilt {
        int32_t pan;
        int32_t tilt;
};

struct PanTiltRelative {
        int8_t panRelative;
        uint8_t panSpeed;
        int8_t tiltRelative;
        uint8_t tiltSpeed;
};

// absolute pan tilt operations
int getPTMin(uvc_device_handle_t *devicehandle, struct PanTilt *value){
        enum uvc_req_code requestCode = UVC_GET_MIN;
        uvc_error_t res;
        res = uvc_get_pantilt_abs(devicehandle, &value->pan, &value->tilt, requestCode);
        if (res != 0) {
                uvc_perror(res, "getPTMin:uvc_get_pantilt_abs");
                return -1;
        }
        return 0;
}

int getPTMax(uvc_device_handle_t *devicehandle, struct PanTilt *value){
        enum uvc_req_code requestCode = UVC_GET_MAX;
        uvc_error_t res;
        res = uvc_get_pantilt_abs(devicehandle, &value->pan, &value->tilt, requestCode);
        if (res != 0) {
                uvc_perror(res, "getPTMax:uvc_get_pantilt_abs");
                return -1;
        }
        return 0;
}

int getPTCurrent(uvc_device_handle_t *devicehandle, struct PanTilt *value){
        enum uvc_req_code requestCode = UVC_GET_CUR;
        uvc_error_t res;
        res = uvc_get_pantilt_abs(devicehandle, &value->pan, &value->tilt, requestCode);
        if (res != 0) {
                uvc_perror(res, "getPTCurrent:uvc_get_pantilt_abs");
                return -1;
        }
        return 0;
}

int setPT(uvc_device_handle_t *devicehandle, struct PanTilt value){
        uvc_error_t res;
        res = uvc_set_pantilt_abs(devicehandle, value.pan, value.tilt);
        if (res != 0) {
                uvc_perror(res, "setPT:uvc_set_pantilt_abs");
                return -1;
        }
        return 1;
}

// relative pan tilt operations
int getPTRCurrent(uvc_device_handle_t *devicehandle, struct PanTiltRelative *value){
        enum uvc_req_code requestCode = UVC_GET_CUR;
        uvc_error_t res;
        res = uvc_get_pantilt_rel(devicehandle, &value->panRelative, &value->panSpeed, &value->tiltRelative, &value->tiltSpeed, requestCode);
        if (res != 0) {
                uvc_perror(res, "getPTRCurrent:uvc_get_pantilt_rel");
                return -1;
        }
        return 0;
}

int setPTR(uvc_device_handle_t *devicehandle, struct PanTiltRelative value){
        uvc_error_t res;
        res = uvc_set_pantilt_rel(devicehandle, value.panRelative, value.panSpeed, value.tiltRelative, value.tiltSpeed);
        if (res != 0) {
                uvc_perror(res, "setPTR:uvc_set_pantilt_rel");
                return -1;
        }
        return 1;
}

// absolute zoom operations
int getZoomMin(uvc_device_handle_t *devicehandle, uint16_t *value){
        enum uvc_req_code requestCode = UVC_GET_MIN;
        uvc_error_t res;
        res = uvc_get_zoom_abs(devicehandle, value, requestCode);
        if (res != 0) {
                uvc_perror(res, "getZoomMin:uvc_get_zoom_abs");
                return -1;
        }
        return 0;
}

int getZoomMax(uvc_device_handle_t *devicehandle, uint16_t *value){
        enum uvc_req_code requestCode = UVC_GET_MAX;
        uvc_error_t res;
        res = uvc_get_zoom_abs(devicehandle, value, requestCode);
        if (res != 0) {
                uvc_perror(res, "getZoomMax:uvc_get_zoom_abs");
                return -1;
        }
        return 0;
}

int getZoomCurrent(uvc_device_handle_t *devicehandle, uint16_t *value){
        enum uvc_req_code requestCode = UVC_GET_CUR;
        uvc_error_t res;
        res = uvc_get_zoom_abs(devicehandle, value, requestCode);
        if (res != 0) {
                uvc_perror(res, "getZoomCurrent:uvc_get_zoom_abs");
                return -1;
        }
        return 0;
}

int setZoom(uvc_device_handle_t *devicehandle, uint16_t value){
        uvc_error_t res;
        res = uvc_set_zoom_abs(devicehandle, value);
        if (res != 0) {
                uvc_perror(res, "setZoom:uvc_set_zoom_abs");
                return -1;
        }
        return 0;
}

int main(int argc, char **argv) {

        uvc_context_t *ctx;
        uvc_device_t **devices;
        uvc_device_t *device;
        uvc_device_handle_t *devicehandle;
        uvc_stream_ctrl_t ctrl;
        uvc_device_descriptor_t *descriptor;
        uvc_error_t res;

        res = uvc_init(&ctx, NULL);
        if (res != 0) {
                uvc_perror(res, "uvc_init");
                return res;
        }

        res = uvc_get_device_list(ctx, &devices);
        if (res != 0) {
                uvc_perror(res, "uvc_get_device_list");
                uvc_exit(ctx);
                return res;
        }

        int devicesCount = sizeof(devices) / sizeof(devices[0]);
        printf("%d devices found\n", devicesCount);
        for(int i = 0; i < devicesCount; i++) {
                res = uvc_get_device_descriptor(devices[i], &descriptor);
                if (res != 0) {
                        uvc_perror(res, "uvc_get_device_descriptor");
                        uvc_free_device_list(devices, 1);
                        uvc_exit(ctx);
                        return res;
                }
                printf("%d vendorid, %d productid, %s serialNumber, %s manufacturer, %s product\n", descriptor->idVendor, descriptor->idProduct, descriptor->serialNumber, descriptor->manufacturer, descriptor->product);
                uvc_free_device_descriptor(descriptor);
        }
        uvc_free_device_list(devices, 1);

        // /* Locates the first attached UVC device, stores in dev */
        // res = uvc_find_device(ctx, &device, 0x46d, 0x848, NULL); /* filter devices: vendor_id, product_id, "serial_num" */
        // Sony SRG-120DU, 1356, 2943
        // Logitech CC3000e 1133, 2120
        res = uvc_find_device(ctx, &device, 1356, 2943, NULL);
        if (res != 0) {
                uvc_perror(res, "uvc_find_device");
                uvc_unref_device(device);
                uvc_exit(ctx);
                return res;
        }

        /* Try to open the device: requires exclusive access */
        res = uvc_open(device, &devicehandle);
        if (res != 0) {
                uvc_perror(res, "uvc_open");
                uvc_unref_device(device);
                uvc_exit(ctx);
                return res;
        }

        int result;

        while(1) {
                char input[50];
                printf ("Enter command (d, q, z, a, r): ");
                scanf ("%s", input);
                // quit
                if(strcmp(input, "q") == 0) {
                        break;
                }

                // device descriptor
                if(strcmp(input, "d") == 0) {
                        uvc_print_diag(devicehandle, stderr);
                }

                // zoom
                if(strcmp(input, "z") == 0) {

                        uint16_t zoomMin;
                        result = getZoomMin(devicehandle, &zoomMin);
                        if(result != 0) {
                                printf("failed to get min zoom\n");
                        } else {
                                printf("min zoom is %d\n", zoomMin);
                        }

                        uint16_t zoomMax;
                        result = getZoomMax(devicehandle, &zoomMax);
                        if(result != 0) {
                                printf("failed to get max zoom\n");
                        } else {
                                printf("max zoom is %d\n", zoomMax);
                        }

                        uint16_t zoomCurrent;
                        result = getZoomCurrent(devicehandle, &zoomCurrent);
                        if(result != 0) {
                                printf("failed to get current zoom\n");
                        } else {
                                printf("current zoom is %d\n", zoomCurrent);
                        }
                        printf ("Enter new zoom value (%d - %d): ", zoomMin, zoomMax);
                        scanf ("%s", input);
                        uint16_t newzoom = atoi(input);
                        result = setZoom(devicehandle, newzoom);
                        if(result != 0) {
                                printf("failed to set zoom\n");
                        } else {
                                printf("set zoom to %d\n", newzoom);
                        }
                }
                // absolute pan tilt
                if(strcmp(input, "a") == 0) {

                        struct PanTilt ptMin;
                        result = getPTMin(devicehandle, &ptMin);
                        if(result != 0) {
                                printf("failed to get min pan tilt\n");
                        } else {
                                printf("min pan is %d min tilt is %d\n", ptMin.pan, ptMin.tilt);
                        }

                        struct PanTilt ptMax;
                        result = getPTMax(devicehandle, &ptMax);
                        if(result != 0) {
                                printf("failed to get max pan tilt\n");
                        } else {
                                printf("max pan is %d max tilt is %d\n", ptMax.pan, ptMax.tilt);
                        }

                        struct PanTilt ptCurrent;
                        result = getPTCurrent(devicehandle, &ptCurrent);
                        if(result != 0) {
                                printf("failed to get current pan tilt\n");
                        } else {
                                printf("current pan is %d current tilt is %d\n", ptCurrent.pan, ptCurrent.tilt);
                        }
                        printf ("Enter new pan value (%d - %d): ", ptMin.pan, ptMax.pan);
                        scanf ("%s", input);
                        uint16_t newpan = atoi(input);
                        printf ("Enter new tilt value (%d - %d): ", ptMin.tilt, ptMax.tilt);
                        scanf ("%s", input);
                        uint16_t newtilt = atoi(input);
                        struct PanTilt newpt;
                        newpt.pan = newpan;
                        newpt.tilt = newtilt;
                        result = setPT(devicehandle, newpt);
                        if(result != 0) {
                                printf("failed to set pan tilt\n");
                        } else {
                                printf("set pan tilt to pan %d tilt %d\n", newpt.pan, newpt.tilt);
                        }
                }

                // absolute pan tilt
                if(strcmp(input, "r") == 0) {
                        struct PanTiltRelative ptrCurrent;
                        result = getPTRCurrent(devicehandle, &ptrCurrent);
                        if(result != 0) {
                                printf("failed to get current pan tilt relative\n");
                        } else {
                                printf("current pan relative %d speed %d current tilt relative %d speed %d\n", ptrCurrent.panRelative, ptrCurrent.panSpeed, ptrCurrent.tiltRelative, ptrCurrent.tiltSpeed);
                        }
                        printf ("Enter new pan relative: ");
                        scanf ("%s", input);
                        int8_t newpanrelative = atoi(input);
                        printf ("Enter new pan speed: ");
                        scanf ("%s", input);
                        uint8_t newpanspeed = atoi(input);
                        printf ("Enter new tilt relative: ");
                        scanf ("%s", input);
                        int8_t newtiltrelative = atoi(input);
                        printf ("Enter new tilt speed: ");
                        scanf ("%s", input);
                        uint8_t newtiltspeed = atoi(input);
                        struct PanTiltRelative newptr;
                        newptr.panRelative = newpanrelative;
                        newptr.panSpeed = newpanspeed;
                        newptr.tiltRelative = newtiltrelative;
                        newptr.tiltSpeed = newtiltspeed;
                        result = setPTR(devicehandle, newptr);
                        if(result != 0) {
                                printf("failed to set relative pan tilt\n");
                        } else {
                                printf("set pan relative %d speed %d current tilt relative %d speed %d\n", newptr.panRelative, newptr.panSpeed, newptr.tiltRelative, newptr.tiltSpeed);
                        }
                }
        }

        // cleanup
        uvc_close(devicehandle);
        uvc_unref_device(device);
        uvc_exit(ctx);

        return 0;
}
