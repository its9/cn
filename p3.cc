#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("StarTopologyBroadcastExample");


void BroadcastPacket(Ptr<Socket> broadcastSocket) {
    Ptr<Packet> packet = Create<Packet>(1024); 
    broadcastSocket->SendTo(packet, 0, InetSocketAddress(Ipv4Address("255.255.255.255"), 9)); 
}

int main (int argc, char *argv[])
{
    CommandLine cmd (__FILE__);
    cmd.Parse (argc, argv);

    Time::SetResolution (Time::NS);
    LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

   
    NodeContainer nodes;
    nodes.Create (8); 

    
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    
    NetDeviceContainer devices[7];
    for (int i = 0; i < 7; ++i) {
        devices[i] = pointToPoint.Install(nodes.Get(0), nodes.Get(i + 1)); 
    }

    InternetStackHelper stack;
    stack.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");

   
    Ipv4InterfaceContainer interfaces[7];
    for (int i = 0; i < 7; ++i) {
        interfaces[i] = address.Assign(devices[i]);
    }

    
    Ptr<Socket> broadcastSocket = Socket::CreateSocket(nodes.Get(0), UdpSocketFactory::GetTypeId());
    broadcastSocket->Bind(InetSocketAddress(Ipv4Address::GetAny(), 9)); 
    broadcastSocket->SetAllowBroadcast(true); 

  
    Simulator::Schedule(Seconds(2.0), &BroadcastPacket, broadcastSocket);

 
    for (int i = 0; i < 7; ++i) {
        UdpEchoServerHelper echoServer(9);
        ApplicationContainer serverApps = echoServer.Install(nodes.Get(i + 1));
        serverApps.Start(Seconds(1.0));
        serverApps.Stop(Seconds(10.0));
    }

   
    AnimationInterface anim("output/star_topology_broadcast.xml");
    anim.SetConstantPosition(nodes.Get(0), 300, 300);
   
    anim.SetConstantPosition(nodes.Get(1), 100, 200);
    anim.SetConstantPosition(nodes.Get(2), 100, 300); 
    anim.SetConstantPosition(nodes.Get(3), 100, 400);
    anim.SetConstantPosition(nodes.Get(4), 100, 500); 
    anim.SetConstantPosition(nodes.Get(5), 100, 600); 
    anim.SetConstantPosition(nodes.Get(6), 100, 700); 
    anim.SetConstantPosition(nodes.Get(7), 100, 800); 

   
    AsciiTraceHelper ascii;
    pointToPoint.EnableAsciiAll(ascii.CreateFileStream("trace.tr"));

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}

