#include <hal/debug.h>
#include <hal/video.h>
#include <windows.h>
#include <assert.h>
#include <usbh_lib.h>
#include <xid_driver.h>
#include <windows.h>

#define ROM_SIZE (12 * 1024) // is this right
#define CHUNK_SIZE (1024)
#define UAC_355xB_MEM_READ 0x05 // Not confirmed, just works

int sha1digest(uint8_t *digest, char *hexdigest, const uint8_t *data, size_t databytes);

static uint8_t rom_data[ROM_SIZE] = {0};

void device_connection_callback(UDEV_T *udev, int status)
{
    uint32_t rom_length;
    uint32_t transferred_bytes = 0;
    int ret;

    if (udev->descriptor.idVendor != 0x045E || udev->descriptor.idProduct != 0x0283)
    {
        debugPrint("%04x %04x is not a Xbox Live Communicator.\n", udev->descriptor.idVendor, udev->descriptor.idProduct);
        return;
    }

    debugPrint("Xbox Live Communicator Connected. Reading data...\n");

    memset(rom_data, 0, ROM_SIZE);
    rom_length = 0;
    uint8_t *dma_mem = usbh_alloc_mem(CHUNK_SIZE);
    assert(dma_mem);

    for (int i = 0; i < ROM_SIZE; i += CHUNK_SIZE)
    {
        transferred_bytes = 0;
        ret = usbh_ctrl_xfer(udev,                                                // Device
                             REQ_TYPE_IN | REQ_TYPE_VENDOR_DEV | REQ_TYPE_TO_DEV, // bmRequestType
                             UAC_355xB_MEM_READ,                                  // bRequest
                             0x0000,                                              // wValue
                             i,                                                   // wIndex
                             CHUNK_SIZE,                                          // wLength
                             dma_mem,                                             // data],
                             &transferred_bytes,
                             10 // Timeout (in 10ms blocks)
        );

        if (ret != USBH_OK)
        {
            debugPrint("Error: Failed Transfer of %d bytes at offset %d\n", CHUNK_SIZE, i);
            assert(0);
        }

        if (transferred_bytes != CHUNK_SIZE)
        {
            debugPrint("Error: Byte read mismatch. Wanted %d, Got %d bytes\n", CHUNK_SIZE, transferred_bytes);
            assert(0);
        }

        memcpy(rom_data + i, dma_mem, CHUNK_SIZE);
        rom_length += transferred_bytes;
    };

    uint8_t digest[20];
    char hex_digest[41];
    sha1digest(digest, hex_digest, rom_data, rom_length);
    debugPrint("Read %d bytes okay\n", rom_length);
    debugPrint("SHA1: %s\n", hex_digest);

    SYSTEMTIME st;
    GetLocalTime(&st);
    static char path[MAX_PATH];
    snprintf(path, sizeof(path), "D:\\xblc_%04d%02d%02d %02d%02d%02d.bin",
             st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
    FILE *fp = fopen(path, "wb");
    if (fp)
    {
        fwrite(rom_data, 1, rom_length, fp);
        fclose(fp);
        debugPrint("Wrote %d bytes to %s\n", rom_length, path);
    }
    else
    {
        debugPrint("Failed to open %s for writing\n", path);
    }
}

void device_disconnect_callback(UDEV_T *udev, int status)
{
    debugPrint("Device disconnected\n");
}

int main(void)
{
    XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);

    usbh_core_init();
    usbh_install_conn_callback(device_connection_callback, device_disconnect_callback);
    debugPrint("Insert your Xbox Live Communicator into the slot\n");

    while (1)
    {
        usbh_pooling_hubs();
    }

    usbh_core_deinit();
    return 0;
}
