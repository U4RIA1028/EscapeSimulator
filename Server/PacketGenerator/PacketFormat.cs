using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
class CppHandlerFormat
{
    public static string handlerFormat = @"bool Handle_{0}(PacketSessionRef& ref, Protocol::{1}_{0} pkt);
";
    public static string initFormat = @"        _packetHandler[Protocol::INGAME::{0}] = [](PacketSessionRef& ref, BYTE* buf, uint16 size) {{ return ParsingPacket<Protocol::{1}_{0}>(Handle_{0}, ref, buf, size); }};
";
    public static string makeFormat = @"    static SendBufferRef MakeSendBuffer(Protocol::{1}_{0} pkt) {{ return _MakeSendBuffer(pkt, Protocol::INGAME::{0}); }}
";
}

class CSharpHandleFormat
{
    public static string handlerFormat = @"            _onRecv.Add((UInt16)INGAME.{0}, MakePacket<{1}>);
            _handle.Add((UInt16)INGAME.{0}, ClientPacketHandler.Handle_{1});
";

    public static string clienthandleFormat = @"        public static void Handle_{0}(IMessage packet)
        {{

		}}
";
}

class ProtoFormat
{
    public static string cppFormat = @"syntax = ""proto3"";
package Protocol;

import ""Enum.proto"";
import ""Struct.proto"";

{0}
";
    public static string csharpFormat = @"syntax = ""proto3"";
package Protocol;
import ""Enum.proto"";
import ""Struct.proto"";

option csharp_namespace = ""Google.Protobuf.Protocol"";

{0}
";
    public static string handleFormat = @"message S_{0}
{{
{1}
}}

message C_{0}
{{
{2}
}}

";
}
