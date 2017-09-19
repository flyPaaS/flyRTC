
#pragma once

const int COMM_OK                  = 0;              // success 
const int COMM_ERR_GENERAL         = -1;             // unknown error
const int COMM_ERR_PARA            = -2;             // invalid arguments
const int COMM_ERR_NOMEM           = -3;             // not enough mem
const int COMM_ERR_FILEIO          = -4;             // file io error (read/write)
const int COMM_ERR_SQLDB           = -5;             // sql server error 
const int COMM_ERR_DATA            = -6;             // data verify error, invalid data
const int COMM_ERR_NETIO           = -7;             // network io error
const int COMM_ERR_SERVERBUSY      = -8;             // server busy, server lock time out, etc 
const int COMM_ERR_SIGABORT        = -9;             // receive abort signal
const int COMM_ERR_OVERSIZE        = -10;            // data block size over limit
const int COMM_ERR_SERVERMASKED    = -11;            // server has been masked
const int COMM_ERR_SERVERREADONLY  = -12;            // server set to readonly
const int COMM_ERR_OUTOFRANGE      = -13;            // uin is out of range

// Socket Error (-201 ~ -300)
const int COMM_ERR_SOCKETOPEN      = -201;           // Open Socket Error
const int COMM_ERR_SOCKETREAD      = -202;           // Read From Socket Error
const int COMM_ERR_SOCKETWRITE     = -203;           // Write to Socket Error
const int COMM_ERR_SOCKETCLOSE     = -204;           // Close Socket Error
const int COMM_ERR_SOCKETINVALID   = -205;           // Socket is Being Invalid

// Protocol Error(-301 ~ -400)
const int COMM_PROTOCOLINVALID     = -301;           //Protocol Invalid


