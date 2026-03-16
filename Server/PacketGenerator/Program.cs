
using System;
using System.IO;
using System.Reflection;
using System.Xml;

namespace PacketGenerator
{
    class Program
    {
        static void Main()
        {
            // ../../../ = PacketGenerator
            ReadWriteFile readWriteFile = new ReadWriteFile("../../../../Common/protoc-21.12-win64/bin/Enum.proto");

            bool isOnlyCpp = true;
            if (isOnlyCpp) // C++ Client - Server
            {
                string serverPath = "../../../../GameServer/ServerPacketHandler.h";
                string clientPath = "../../../../../Capstone/Source/Capstone/ClientPacketHandler.h";

                readWriteFile.MakeOnlyCppHandler(serverPath, clientPath);
            }
            else // C# Client - Server
            {
                string serverPath = "../../../../Server/ServerPacketHandler.h";
                string clientPath = "../../../../../CsharpClient/GameServer/Packet/";

                readWriteFile.MakeMultiHandler(serverPath, clientPath);
            }

            {
                string destPath = "../../../../Common/protoc-21.12-win64/bin/Protocol.proto";

                readWriteFile.MakeProto(destPath, isOnlyCpp);
            }

            Console.WriteLine("Success");
        }
    }
}
