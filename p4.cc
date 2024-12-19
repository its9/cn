#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("BusTopologyExample");

int main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);


  NodeContainer nodes;
  nodes.Create (5);


  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
  pointToPoint.SetChannelAttribute("Delay", TimeValue(NanoSeconds(100)));


  NetDeviceContainer devices01;
  devices01 = pointToPoint.Install (nodes.Get(0), nodes.Get(1));


  NetDeviceContainer devices12;
  devices12 = pointToPoint.Install (nodes.Get(1), nodes.Get(2));

  NetDeviceContainer devices23;
  devices23 = pointToPoint.Install (nodes.Get(2), nodes.Get(3));


  NetDeviceContainer devices34;
  devices34 = pointToPoint.Install (nodes.Get(3), nodes.Get(4));


  InternetStackHelper stack;
  stack.Install (nodes);


  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces01 = address.Assign (devices01);
  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces12 = address.Assign (devices12);
  address.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces23 = address.Assign (devices23);
  address.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces34 = address.Assign (devices34);


  UdpServerHelper udpServer (9);
  ApplicationContainer serverApp = udpServer.Install (nodes.Get (1));
  serverApp.Start (Seconds (1.0));
  serverApp.Stop (Seconds (10.0));


  Ipv4Address remoteAddress = Ipv4Address("10.1.1.2"); 
  UdpClientHelper udpClient (InetSocketAddress(remoteAddress, 9));
  udpClient.SetAttribute ("MaxPackets", UintegerValue (320));
  udpClient.SetAttribute ("Interval", TimeValue (MilliSeconds (50)));
  udpClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApp = udpClient.Install (nodes.Get (0));
  clientApp.Start (Seconds (2.0));
  clientApp.Stop (Seconds (10.0));


  pointToPoint.EnablePcapAll ("bus-topology");


  LogComponentEnable ("UdpClient", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpServer", LOG_LEVEL_INFO);


  AnimationInterface anim ("bus-topology.xml");


  anim.SetConstantPosition (nodes.Get(0), 100, 100); 
  anim.SetConstantPosition (nodes.Get(1), 200, 100); 
  anim.SetConstantPosition (nodes.Get(2), 300, 100); 
  anim.SetConstantPosition (nodes.Get(3), 400, 100); 
  anim.SetConstantPosition (nodes.Get(4), 500, 100); 


  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}

