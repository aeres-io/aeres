/*
  MIT License

  Copyright (c) 2018 aeres.io

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include <aeres/Util.h>
#include <aeres/HandshakeInstruction.h>


namespace aeres
{
  namespace
  {
    static const uint16_t PROTOCOL_VERSION = 1;
    static const uint16_t OPCODE_HANDSHAKE = 1;
  }

  bool HandshakeInstruction::Serialize(IOutputStream & stream) const
  {
    return_false_if(!stream.WriteUInt16(PROTOCOL_VERSION));
    return_false_if(!stream.WriteUInt16(OPCODE_HANDSHAKE));

    return_false_if(!stream.WriteString(this->hostname));
    return_false_if(!stream.WriteUInt16(this->port));
    return_false_if(!stream.WriteUInt8(static_cast<uint8_t>(this->protocol)));
    return_false_if(!stream.WriteString(this->data));

    return true;
  }


  bool HandshakeInstruction::Deserialize(IInputStream & stream)
  {
    return_false_if(stream.Remainder() < sizeof(uint16_t) * 2 + sizeof(uint32_t));
    return_false_if(stream.ReadUInt16() != PROTOCOL_VERSION);
    return_false_if(stream.ReadUInt16() != OPCODE_HANDSHAKE);
    return_false_if(stream.ReadString(this->hostname) <= sizeof(uint32_t));

    return_false_if(stream.Remainder() < sizeof(uint16_t) + sizeof(uint8_t));
    this->port = stream.ReadUInt16();

    uint8_t protoVal = stream.ReadUInt8();
    return_false_if(protoVal >= static_cast<uint8_t>(Protocol::__MAX__));
    this->protocol = static_cast<Protocol>(protoVal);

    return_false_if(stream.Remainder() < sizeof(uint32_t));
    return_false_if(stream.ReadString(this->data) < sizeof(uint32_t));

    return true;
  }
}