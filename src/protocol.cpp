
#include "main.h"

size_t ReceiveParsing(void *receivedata, size_t receive_len, uint8_t *sendDataBuff, MB_FRAME_TYPE type)
{

    int MessageLength = 0;
    int offset = 0;

    if (type == RTU)
    {
        if (RK.readShort(slave_id) != ((uint8_t *)receivedata)[MB_UID])
            return 0;
    }

    if (type == TCP)
    {
        offset = 6;
    }

    for (size_t i = 0; i <= receive_len; i++)
    {
        sendDataBuff[i] = ((uint8_t *)receivedata)[i];
    }

    byte byteFN = sendDataBuff[MB_FUNC + offset];
    int Start = word(sendDataBuff[MB_REGISTER_START + offset], sendDataBuff[MB_REGISTER_START + offset + 1]);
    int WordDataLength = word(sendDataBuff[MB_REGISTER_NUMBER + offset], sendDataBuff[MB_REGISTER_NUMBER + offset + 1]);
    int ByteDataLength = WordDataLength * 2;

    switch (byteFN)
    {
    case MB_FC_NONE:
        break;

    case MB_FC_READ_REGISTERS: // 03 Read Holding Registers
        if (type == TCP)
        {
            sendDataBuff[MB_TCP_LEN] = highByte(ByteDataLength + 3);
            sendDataBuff[MB_TCP_LEN + 1] = lowByte(ByteDataLength + 3);
            ; // Number of bytes after this one.
        }

        sendDataBuff[MB_RES_BYTE_LEN + offset] = ByteDataLength; // Number of bytes after this one (or number of bytes of data).
        for (int i = 0; i < WordDataLength; i++)
        {
            sendDataBuff[MB_RES_DATA + offset + (i * 2)] = RK.readByte(((Start + i) * 2) + 1); // high byte
            sendDataBuff[MB_RES_DATA + offset + 1 + (i * 2)] = RK.readByte((Start + i) * 2);   // low byte
        }
        MessageLength = ByteDataLength + offset + 3;

        break;

    case MB_FC_READ_INPUT_REGISTERS: // 02 Read Input Registers
        if (type == TCP)
        {
            sendDataBuff[MB_TCP_LEN] = highByte(ByteDataLength + 3);
            sendDataBuff[MB_TCP_LEN + 1] = lowByte(ByteDataLength + 3);
            ; // Number of bytes after this one.
        }
        sendDataBuff[MB_RES_BYTE_LEN + offset] = ByteDataLength;
        for (int i = 0; i < WordDataLength; i++)
        {
            sendDataBuff[MB_RES_DATA + offset + (i * 2)] = RA.readByte(((Start + i) * 2) + 1); // high byte
            sendDataBuff[MB_RES_DATA + offset + 1 + (i * 2)] = RA.readByte((Start + i) * 2);   // low byte
        }
        MessageLength = ByteDataLength + offset + 3;

        break;

    case MB_FC_WRITE_REGISTER: // 06 Write Holding Register

        RK.writeByte(Start * 2, sendDataBuff[MB_WRITE_DATA_06 + offset + 1]);
        RK.writeByte(Start * 2 + 1, sendDataBuff[MB_WRITE_DATA_06 + offset]);
        MessageLength = 6 + offset;

        break;

    case MB_FC_WRITE_MULTIPLE_REGISTERS: // 16 Write Holding Registers
        for (int i = 0; i < WordDataLength; i++)
        {

            RK.writeByte((Start + i) * 2, sendDataBuff[MB_WRITE_DATA_16 + offset + 1 + (i * 2)]);
            RK.writeByte(((Start + i) * 2) + 1, sendDataBuff[MB_WRITE_DATA_16 + offset + (i * 2)]);
        }
        MessageLength = 6 + offset;

        break;

    case MB_FC_USER: // 80 User Function

        break;
    }

    if (type == RTU)
    {
        word crc = calculateCRC(sendDataBuff, MessageLength);
        sendDataBuff[MessageLength++] = highByte(crc);
        sendDataBuff[MessageLength++] = lowByte(crc);
    }

    return MessageLength;
}

unsigned int calculateCRC(uint8_t *frame, size_t bufferSize)
{
    unsigned int temp, temp2, flag;
    temp = 0xFFFF;
    for (size_t i = 0; i < bufferSize; i++)
    {
        temp = temp ^ frame[i];
        for (size_t j = 1; j <= 8; j++)
        {
            flag = temp & 0x0001;
            temp >>= 1;
            if (flag)
                temp ^= 0xA001;
        }
    }
    // Reverse byte order.
    temp2 = temp >> 8;
    temp = (temp << 8) | temp2;
    temp &= 0xFFFF;
    // the returned value is already swapped
    // crcLo byte is first & crcHi byte is last
    return temp;
}