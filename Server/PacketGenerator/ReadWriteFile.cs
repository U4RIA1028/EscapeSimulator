using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Security.Cryptography.X509Certificates;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace PacketGenerator
{
    internal class ReadWriteFile
    {
        private string filePath = "";
        public ReadWriteFile(string filePath)
        {
            this.filePath = filePath;
        }

        public void MakeOnlyCppHandler(string serverPath, string clientPath)
        {
            MakeCppHandle();
            MakeCppHandle(false);

            // 만들어진 파일을 IOCP 서버로 옮기는 코드
            System.IO.File.Copy("ServerPacketHandler.h", serverPath, true);
            System.IO.File.Copy("ClientPacketHandler.h", clientPath, true);
        }

        public void MakeMultiHandler(string serverPath, string clientPath)
        {
            MakeCppHandle();
            MakeCSharpHandle();

            string packetHandler = clientPath + "PacketHandler.cs";
            string clientPacketHandler = clientPath + "ClientPacketHandler.cs";

            System.IO.File.Copy("ServerPacketHandler.h", serverPath, true);
            System.IO.File.Copy("PacketHandler.cs", packetHandler, true);
            System.IO.File.Copy("ClientPacketHandler.cs", clientPacketHandler, true);
        }

        private void MakeCppHandle(bool isServer = true)
        {
            List<string> types = ReadFile(filePath);
            string[] handlers = new string[4];

            foreach (string type in types)
            {
                if (isServer)
                {
                    // 핸들러 코드 추가
                    handlers[0] += string.Format(CppHandlerFormat.handlerFormat, type, "C");

                    // 초기화 코드 추가
                    handlers[1] += string.Format(CppHandlerFormat.initFormat, type, "C");

                    // 생성 코드 (MakeSendBuffer) 추가
                    handlers[2] += string.Format(CppHandlerFormat.makeFormat, type, "S");
                }
                else
                {
                    // 핸들러 코드 추가
                    handlers[0] += string.Format(CppHandlerFormat.handlerFormat, type, "S");

                    // 초기화 코드 추가
                    handlers[1] += string.Format(CppHandlerFormat.initFormat, type, "S");

                    // 생성 코드 (MakeSendBuffer) 추가
                    handlers[2] += string.Format(CppHandlerFormat.makeFormat, type, "C");
                }
            }
            if(isServer)
                handlers[3] = "ServerPacketHandler";
            else
                handlers[3] = "ClientPacketHandler";

            string cppPacketHandler = "";

            foreach (string line in File.ReadAllLines("../../../Templates/PacketHandler.h"))
            {
                if (Regex.IsMatch(line, @"{[0-9]}"))
                {
                    int n = line.IndexOf('{');
                    char c = line[n + 1];
                    int i = int.Parse(c.ToString());
                    
                    if (handlers.Length > i)
                    {
                        string newLine = line.Replace("{" + c + "}", handlers[i]);
                        cppPacketHandler += newLine;
                    }
                }
                else
                    cppPacketHandler += line;
                cppPacketHandler += "\n";
            }
            if(isServer)
            {
                File.WriteAllText("ServerPacketHandler.h", cppPacketHandler);
            }
            else
            {
                File.WriteAllText("ClientPacketHandler.h", cppPacketHandler);
            }
        }

        private void MakeCSharpHandle()
        {
            List<string> types = ReadFile(filePath);
            string[] packetHandlers = new string[1];
            string[] clientPacketHandlers = new string[1];

            foreach (string type in types)
            {
                string[] words = type.Split("_");

                string msgName = "";
                foreach (string word in words)
                    msgName += FirstCharToUpper(word);

                // handler 포멧팅
                string packetName = $"S_{type}";
                packetHandlers[0] += string.Format(CSharpHandleFormat.handlerFormat, msgName, packetName);

                // 수행할 함수 포맷팅
                clientPacketHandlers[0] += string.Format(CSharpHandleFormat.clienthandleFormat, packetName);
            }

            string cSharpPacketHandler = "";

            foreach (string line in File.ReadAllLines("../../../Templates/PacketHandler.cs"))
            {
                if (Regex.IsMatch(line, @"{[0-9]}"))
                {
                    int n = line.IndexOf('{');
                    char c = line[n + 1];
                    int i = int.Parse(c.ToString());

                    if (packetHandlers.Length > i)
                    {
                        cSharpPacketHandler += packetHandlers[i];
                    }
                }
                else
                    cSharpPacketHandler += line;
                cSharpPacketHandler += "\n";
            }

            File.WriteAllText("PacketHandler.cs", cSharpPacketHandler);

            // Client PacketHandler 만들기
            cSharpPacketHandler = "";

            foreach (string line in File.ReadAllLines("../../../Templates/ClientPacketHandler.cs"))
            {
                if (Regex.IsMatch(line, @"{[0-9]}"))
                {
                    int n = line.IndexOf('{');
                    char c = line[n + 1];
                    int i = int.Parse(c.ToString());

                    if (clientPacketHandlers.Length > i)
                    {
                        cSharpPacketHandler += clientPacketHandlers[i];
                    }
                }
                else
                    cSharpPacketHandler += line;
                cSharpPacketHandler += "\n";
            }

            File.WriteAllText("ClientPacketHandler.cs", cSharpPacketHandler);
        }

        public void MakeProto(string destPath, bool isOnlyCpp)
        {
            List<string> types = ReadFile(filePath);
            string handle = "";
            Dictionary<string, string> clientInside = new Dictionary<string, string>();
            Dictionary<string, string> serverInside = new Dictionary<string, string>();

            bool isClient = false;
            bool isServer = false;

            if (File.Exists(destPath))
            {
                string subStr = "";
                foreach (string line in File.ReadAllLines(destPath))
                {
                    if (isClient)
                    {
                        if (line.Contains("{"))
                            continue;
                        else if (line.Contains("}"))
                        {
                            isClient = false;
                            continue;
                        }
                        else
                        {
                            clientInside[subStr] += line + "\n";
                        }
                    }

                    if (isServer)
                    {
                        if (line.Contains("{"))
                            continue;
                        else if (line.Contains("}"))
                        {
                            isServer = false;
                            continue;
                        }
                        else
                        {
                            serverInside[subStr] += line + "\n";
                        }

                    }

                    if (line.Contains("C_"))
                    {
                        isClient = true;
                        int index = line.IndexOf("C_");
                        subStr = line.Substring(index + 2);
                        if (types.Contains(subStr))
                        {
                            clientInside[subStr] = "";
                        }

                        continue;
                    }
                    else if (line.Contains("S_"))
                    {
                        isServer = true;
                        int index = line.IndexOf("S_");
                        subStr = line.Substring(index + 2);
                        if (types.Contains(subStr))
                        {
                            serverInside[subStr] = "";
                        }

                        continue;
                    }
                }
            }

            foreach (string type in types)
            {
                string client = "";
                string server = "";
                if (clientInside.ContainsKey(type))
                {
                    if (clientInside[type].Length > 2)
                        client = clientInside[type].Substring(0, clientInside[type].Length - 1);
                }
                if (serverInside.ContainsKey(type))
                {
                    if (serverInside[type].Length > 2)
                        server = serverInside[type].Substring(0, serverInside[type].Length - 1); ;
                }

                handle += String.Format(ProtoFormat.handleFormat, type, server, client);
            }

            string cppText = string.Format(ProtoFormat.cppFormat, handle);
            File.WriteAllText("Protocol.proto", cppText);
            System.IO.File.Copy("Protocol.proto", destPath, true);

            if (!isOnlyCpp)
            {
                string paths = Path.GetDirectoryName(destPath);

                string csharpText = string.Format(ProtoFormat.csharpFormat, handle);
                File.WriteAllText("ProtocolC.proto", csharpText);
                System.IO.File.Copy("ProtocolC.proto", paths + "\\ProtocolC.proto", true);
            }
        }
        private List<string> ReadFile(string filePath)
        {
            List<string> types = new List<string>();

            bool startParsing = false;
            foreach (string line in File.ReadAllLines(filePath))
            {
                if (!startParsing && line.Contains("enum INGAME"))
                {
                    startParsing = true;
                    continue;
                }

                if (!startParsing)
                    continue;

                if (line.Contains("{"))
                    continue;

                if (line.Contains("NULL"))
                    continue;

                if (line.Contains("}"))
                    break;

                string[] type = line.Trim().Split(" =");
                if (type.Length == 0)
                    continue;

                types.Add(type[0]);
                //handle += String.Format(ProtoFormat.handleFormat, names[0]);
            }

            return types;
        }
        private static string FirstCharToUpper(string input)
        {
            if (string.IsNullOrEmpty(input))
                return "";
            return input[0].ToString().ToUpper() + input.Substring(1).ToLower();
        }
    }
}
