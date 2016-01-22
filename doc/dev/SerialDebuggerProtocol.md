# Zebu 3 Serial Debugger Protocol
## Introduction

The Serial Debugger Protocol (SDP) has multiple purposes. It first purpose is to
load the operating system on to the device using a serial connection, which
means the SD card does not need to be modified for every build. The second
purpose is to provide access to the operating system kernel externally for
debugging purposes.

## Usage

The Serial Debugger Protocol will be used by a small bootloader on the SD card
of the device, then the kernel. The bootloader will wait for a connection from
the SDP client running on an attached computer. Once a connection is
established, the protocol will be used to send the operating system to the
device. The bootloader will copy the kernel to the appropriate location and
start the kernel as if it had booted directly.

Once the basics of the kernel are running, it will check that the SDP client is
still connected and use the connection to send and receive data. The kernel will
detect if the SDP client connects or disconnects at any time so debugging can be
done after-the-fact as well.

## Protocol Overview

The SDP protocol is a binary message-based protocol. Some messages will only be
sent by the server (the kernel or bootloader), others will only be sent by the
client. This will be indicated in each message definition below, though it
should generally be obvious which messages don't apply to each end of the
connection.

Each message begins with a signature: the bytes for 'Zebu' in ASCII (5A 65 62
75). This is to ensure that some other device isn't sending data on the serial
port, which may be interpreted incorrectly.

Following the signature, one byte indicates the message type. The message type
bytes are described in each message definition below.

Each message has its own definition for what data is included within it. If a
variable-length datum is included (such as a string of text), it will start with
two bytes in big-endian order to indicate the length of that data.

Finally, the end of each message is a CRC to ensure the message was properly
received and parsed. The CRC is computed using all of the data bytes from the
message, including the signature and type. The algorithm used is CRC-16-CCITT:

```c
uint16_t crc16(const uint8_t* data, uint16_t length) {
    uint8_t x;
    uint16_t crc = 0xFFFF;

    while (length--) {
        x = crc >> 8 ^ *data++;
        x ^= x >> 4;
        crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x << 5)) ^ ((uint16_t)x);
    }
    return crc;
}
```

If a the checksum at the end of a message does not match the expected value, the
recipient MUST discard the message. It MAY watch for the message-starting
signature on the channel to indicate the start of another message.

## Flow

The general flow of messages between a server (kernel or bootloader) and a
client looks like this:

* S: Ping
* (client isn't connected yet, nothing happens)
* S: Ping
* C: Ping Response
* S: Get Version (`1`)
* C: Version Response (`1`)
* S: Log Message (`2`, ``"Loader ready"``)
* S: Request Kernel
* C: File Info (`"kernel.bin"`, `83721`)
* C: File Data
* C: File Data
* etc.

## Messages

### Ping (`00`)

The Ping message is sent to check that the other end of the connection is alive.
It contains no additional data.

### Ping Response (`01`)

The Ping Response message is sent immediately in response to a Ping message. It
contains no additional data.

### Get Version (`02`)

```
uint16_t version
```

Queries the client for its version, and supplies the server's own `version`.
The only supported version currently is `1`.

### Version Response (`03`)

```
uint16_t version
```

A response to a Get Version message. Supplies the `version` of the client to the
server.

### Log Message (`04`)

```
uint8_t level
string  module
string  message
```

Used by the server to send a log message to the client. The log `level` is one of:

* 0 = Fatal
* 1 = Error
* 2 = Warning
* 3 = Info
* 4 = Debug

The `module` is the name of the module that emitted the message. These names are
outside of the scope of this document, but would likely be things like `"Mem
Mgr"` and `"USB"`.

### Request Kernel (`05`)

This message is sent by the server to request the client send it the kernel. The
client SHOULD respond with a File Info followed by File Data messages.

### File Info (`06`)

```
string   filename
uint32_t size
```

Returns information about a file (including the kernel) to the server from the
client. Note that future versions of this protocol will likely include
additional information about a file.

### File Data (`07`)

```
uint8_t[] data
```

Sends some `data` for a file. The recipient must already know the full size of
the file from a File Info packet. Any amount of data, up to the limit of the
size of a message, might be included here, but a reasonable size (say, 4096
bytes) is suggested.

### Error (`08`)

```
string message
```

Indicates that some error related to this protocol's operations has occurred.
For example, if the client was sending a file to the server and the file became
inaccessible to the client, it would have to stop sending the file and report
an error using this message.

The recipient of this message must assume that whatever operation was currently
taking place has been terminated.

###### Copyright &copy; 2016 Kulshan Concepts. All rights reserved.
