#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("PeerToPeerExample");

int main (int argc, char *argv[])
{
  CommandLine cmd (__FILE__);
  cmd.Parse (argc, argv);

  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  // Create a container for three nodes
  NodeContainer nodes;
  nodes.Create (3);

  // Setup point-to-point links for all nodes to connect to the server (node 0)
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer devices1, devices2;
  devices1 = pointToPoint.Install (nodes.Get(0), nodes.Get(1)); // Link between server and client 1
  devices2 = pointToPoint.Install (nodes.Get(0), nodes.Get(2)); // Link between server and client 2

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");

  Ipv4InterfaceContainer interfaces1 = address.Assign (devices1);
  Ipv4InterfaceContainer interfaces2 = address.Assign (devices2);

  // Create a UDP Echo Server on node 0
  UdpEchoServerHelper echoServer (9);
  ApplicationContainer serverApps = echoServer.Install (nodes.Get (0));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  // Create UDP Echo Clients on node 1 and node 2
  UdpEchoClientHelper echoClient1 (interfaces1.GetAddress (0), 9); // Client 1
  echoClient1.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient1.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient1.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps1 = echoClient1.Install (nodes.Get (1));
  clientApps1.Start (Seconds (2.0));
  clientApps1.Stop (Seconds (10.0));

  UdpEchoClientHelper echoClient2 (interfaces2.GetAddress (0), 9); // Client 2
  echoClient2.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient2.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient2.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps2 = echoClient2.Install (nodes.Get (2));
  clientApps2.Start (Seconds (3.0));
  clientApps2.Stop (Seconds (10.0));

  // Animation interface for visualization
  AnimationInterface anim("second.xml");
  anim.SetConstantPosition(nodes.Get(0), 100, 400); // Server
  anim.SetConstantPosition(nodes.Get(1), 400, 300); // Client 1
  anim.SetConstantPosition(nodes.Get(2), 400, 500); // Client 2

  // Optional: ASCII trace for packet metrics
  AsciiTraceHelper ascii;
  pointToPoint.EnableAsciiAll(ascii.CreateFileStream("trace.tr"));

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}

