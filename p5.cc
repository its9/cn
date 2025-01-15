#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/ipv4-global-routing-helper.h" // Updated inclusion


// n0 -------------- r0 -------------- r1 -------------- r2 -------------- r3 -------------- n1


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("PointToPointPPPExample");

int main (int argc, char *argv[])
{
CommandLine cmd (__FILE__);
cmd.Parse (argc, argv);

Time::SetResolution (Time::NS);
LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

// Create nodes
NodeContainer nodes;
nodes.Create (2); 

NodeContainer routers;
routers.Create (4); 


PointToPointHelper pointToPoint;
pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

// Point-to-point links between nodes and routers
NetDeviceContainer devices0, devices1, devices2, devices3, devices4, devices5;

// (n0, r0)
devices0 = pointToPoint.Install (nodes.Get(0), routers.Get(0));
// (r0, r1)
devices1 = pointToPoint.Install (routers.Get(0), routers.Get(1));
// (r1, r2)
devices2 = pointToPoint.Install (routers.Get(1), routers.Get(2));
// (r2, r3)
devices3 = pointToPoint.Install (routers.Get(2), routers.Get(3));
// (r3, n1)
devices4 = pointToPoint.Install (routers.Get(3), nodes.Get(1));


InternetStackHelper stack;
stack.Install (nodes);
stack.Install (routers);

Ipv4AddressHelper address;


address.SetBase ("10.1.1.0", "255.255.255.0");
Ipv4InterfaceContainer interfaces0 = address.Assign (devices0);


address.SetBase ("10.1.2.0", "255.255.255.0");
Ipv4InterfaceContainer interfaces1 = address.Assign (devices1);


address.SetBase ("10.1.3.0", "255.255.255.0");
Ipv4InterfaceContainer interfaces2 = address.Assign (devices2);


address.SetBase ("10.1.4.0", "255.255.255.0");
Ipv4InterfaceContainer interfaces3 = address.Assign (devices3);

// Assign addresses to r3-n1 link
address.SetBase ("10.1.5.0", "255.255.255.0");
Ipv4InterfaceContainer interfaces4 = address.Assign (devices4);


Ipv4GlobalRoutingHelper::PopulateRoutingTables ();


UdpEchoServerHelper echoServer (9);
ApplicationContainer serverApps = echoServer.Install (nodes.Get (1));
serverApps.Start (Seconds (1.0));
serverApps.Stop (Seconds (10.0));


UdpEchoClientHelper echoClient (interfaces4.GetAddress (1), 9);
echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
clientApps.Start (Seconds (2.0));
clientApps.Stop (Seconds (10.0));

Ptr n0 = nodes.Get(0);
Ptr n1 = nodes.Get(1);
Ptr r0 = routers.Get(0);
Ptr r1 = routers.Get(1);
Ptr r2 = routers.Get(2);
Ptr r3 = routers.Get(3);

AnimationInterface anim ("fifth.xml");
anim.SetConstantPosition (n0, 100, 400);
anim.SetConstantPosition (n1, 400, 400);
anim.SetConstantPosition (r0, 200, 300);
anim.SetConstantPosition (r1, 300, 300);
anim.SetConstantPosition (r2, 400, 300);
anim.SetConstantPosition (r3, 500, 300);
AsciiTraceHelper ascii;
pointToPoint.EnableAsciiAll (ascii.CreateFileStream ("5th.tr"));
Simulator::Run();
Simulator::Destroy();
return 0;
}
