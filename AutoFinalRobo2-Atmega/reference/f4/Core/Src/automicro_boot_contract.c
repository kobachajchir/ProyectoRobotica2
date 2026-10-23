#include "automicro_boot_contract.h"

#include <string.h>

uint32_t AutoMicroBoot_Crc32(const void *data, size_t length)
{
    const uint8_t *bytes = (const uint8_t *)data;
    uint32_t crc = UINT32_C(0xFFFFFFFF);

    for (size_t index = 0U; index < length; ++index)
    {
        crc ^= bytes[index];
        for (uint32_t bit = 0U; bit < 8U; ++bit)
        {
            const uint32_t lsb_mask = (uint32_t)(0U - (crc & 1U));
            crc = (crc >> 1) ^ (UINT32_C(0xEDB88320) & lsb_mask);
        }
    }

    return ~crc;
}

const AutoMicroBootMailbox *AutoMicroBootMailbox_Get(void)
{
    return (const AutoMicroBootMailbox *)(uintptr_t)AUTOMICRO_BOOT_MAILBOX_ADDR;
}

bool AutoMicroBootMailbox_Validate(const AutoMicroBootMailbox *mailbox)
{
    if ((mailbox == NULL) ||
        (mailbox->magic != AUTOMICRO_BOOT_MAILBOX_MAGIC) ||
        (mailbox->schema_version != AUTOMICRO_BOOT_MAILBOX_SCHEMA) ||
        (mailbox->struct_size != sizeof(*mailbox)))
    {
        return false;
    }

    return AutoMicroBoot_Crc32(mailbox, offsetof(AutoMicroBootMailbox, mailbox_crc32)) ==
           mailbox->mailbox_crc32;
}

bool AutoMicroBootMailbox_Consume(AutoMicroBootMailbox *out)
{
    const AutoMicroBootMailbox *mailbox = AutoMicroBootMailbox_Get();
    volatile AutoMicroBootMailbox *mutable_mailbox =
        (volatile AutoMicroBootMailbox *)(uintptr_t)AUTOMICRO_BOOT_MAILBOX_ADDR;

    if (out == NULL || !AutoMicroBootMailbox_Validate(mailbox))
    {
        if (out != NULL)
        {
            memset(out, 0, sizeof(*out));
        }
        return false;
    }

    memcpy(out, mailbox, sizeof(*out));
    mutable_mailbox->magic = 0U;
    mutable_mailbox->mailbox_crc32 = 0U;
    return true;
}

bool AutoMicroBootMailbox_WriteRequest(AutoMicroBootRequest request)
{
    AutoMicroBootMailbox request_mailbox;
    volatile uint32_t *destination =
        (volatile uint32_t *)(uintptr_t)AUTOMICRO_BOOT_MAILBOX_ADDR;
    const uint32_t *source = (const uint32_t *)(const void *)&request_mailbox;

    if (request != AUTOMICRO_BOOT_REQUEST_VISUAL_MENU &&
        request != AUTOMICRO_BOOT_REQUEST_ROM_BOOTLOADER)
    {
        return false;
    }

    memset(&request_mailbox, 0, sizeof(request_mailbox));
    request_mailbox.magic = AUTOMICRO_BOOT_MAILBOX_MAGIC;
    request_mailbox.schema_version = AUTOMICRO_BOOT_MAILBOX_SCHEMA;
    request_mailbox.struct_size = sizeof(request_mailbox);
    request_mailbox.request = (uint32_t)request;
    request_mailbox.mailbox_crc32 = AutoMicroBoot_Crc32(
        &request_mailbox,
        offsetof(AutoMicroBootMailbox, mailbox_crc32));

    destination[0] = 0U;
    for (size_t index = 1U;
         index < (sizeof(request_mailbox) / sizeof(uint32_t));
         ++index)
    {
        destination[index] = source[index];
    }
    destination[0] = source[0];

    return AutoMicroBootMailbox_Validate(AutoMicroBootMailbox_Get());
}

AutoMicroBootRequest AutoMicroBootMailbox_ConsumeRequest(void)
{
    const AutoMicroBootMailbox *mailbox = AutoMicroBootMailbox_Get();
    volatile AutoMicroBootMailbox *mutable_mailbox =
        (volatile AutoMicroBootMailbox *)(uintptr_t)AUTOMICRO_BOOT_MAILBOX_ADDR;
    AutoMicroBootRequest request = AUTOMICRO_BOOT_REQUEST_NONE;

    if (AutoMicroBootMailbox_Validate(mailbox))
    {
        if (mailbox->request == (uint32_t)AUTOMICRO_BOOT_REQUEST_VISUAL_MENU)
        {
            request = AUTOMICRO_BOOT_REQUEST_VISUAL_MENU;
        }
        else if (mailbox->request == (uint32_t)AUTOMICRO_BOOT_REQUEST_ROM_BOOTLOADER)
        {
            request = AUTOMICRO_BOOT_REQUEST_ROM_BOOTLOADER;
        }
    }

    mutable_mailbox->magic = 0U;
    mutable_mailbox->request = (uint32_t)AUTOMICRO_BOOT_REQUEST_NONE;
    mutable_mailbox->mailbox_crc32 = 0U;
    return request;
}
