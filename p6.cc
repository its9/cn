#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("P2P_CSMA");

int main(int argc, char *argv[]) {
    LogComponentEnable("P2P_CSMA", LOG_LEVEL_INFO);

    NodeContainer p2pNodes;
    p2pNodes.Create(1);  
    NodeContainer lanNodes;
    lanNodes.Create(4);  

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("100Mbps"));
    csma.SetChannelAttribute("Delay", TimeValue(NanoSeconds(6560)));

    NetDeviceContainer p2pDevices;
    p2pDevices = pointToPoint.Install(p2pNodes.Get(0), lanNodes.Get(0));

    NetDeviceContainer csmaDevices;
    csmaDevices = csma.Install(lanNodes);

    InternetStackHelper stack;
    stack.Install(p2pNodes);
    stack.Install(lanNodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces;
    p2pInterfaces = address.Assign(p2pDevices);

    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer csmaInterfaces;
    csmaInterfaces = address.Assign(csmaDevices);

    uint16_t port = 9;
    UdpEchoServerHelper echoServer(port);
    ApplicationContainer serverApps = echoServer.Install(lanNodes.Get(3));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    UdpEchoClientHelper echoClient(csmaInterfaces.GetAddress(3), port);
    echoClient.SetAttribute("MaxPackets", UintegerValue(10));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient.Install(p2pNodes.Get(0));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    AnimationInterface anim("mysixth.xml");
    
    anim.SetConstantPosition(p2pNodes.Get(0), 10.0, 50.0);  
    
    anim.SetConstantPosition(lanNodes.Get(0), 50.0, 70.0);  
    anim.SetConstantPosition(lanNodes.Get(1), 40.0, 50.0); 
    anim.SetConstantPosition(lanNodes.Get(2), 60.0, 50.0); 
    anim.SetConstantPosition(lanNodes.Get(3), 50.0, 30.0); 

    AsciiTraceHelper ascii;
    pointToPoint.EnableAsciiAll(ascii.CreateFileStream("mysixth.tr"));
    csma.EnableAsciiAll(ascii.CreateFileStream("csma-trace.tr"));

    pointToPoint.EnablePcapAll("p2p");
    csma.EnablePcapAll("csma");

    Simulator::Stop(Seconds(11.0));
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}

