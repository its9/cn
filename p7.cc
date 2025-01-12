#include <fstream>
#include "ns3/core-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"  // Include the NetAnim module
#include "ns3/point-to-point-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("UdpClientServerExample");

int main (int argc, char *argv[])
{
  // Declare variables used in command-line arguments
  bool useV6 = false;
  bool logging = true;
  Address serverAddress;

  CommandLine cmd (__FILE__);
  cmd.AddValue ("useIpv6", "Use Ipv6", useV6);
  cmd.AddValue ("logging", "Enable logging", logging);
  cmd.Parse (argc, argv);

  if (logging)
    {
      LogComponentEnable ("UdpClient", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpServer", LOG_LEVEL_INFO);
    }

  NS_LOG_INFO ("Create nodes in the topology.");
  NodeContainer n;
  n.Create (2);

  // Install internet stack
  InternetStackHelper internet;
  internet.Install (n);

  NS_LOG_INFO ("Create point-to-point link between the two nodes.");
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
  NetDeviceContainer devices = p2p.Install (n);

  NS_LOG_INFO ("Assign IP Addresses.");
  if (useV6 == false)
    {
      Ipv4AddressHelper ipv4;
      ipv4.SetBase ("10.1.1.0", "255.255.255.0");
      Ipv4InterfaceContainer i = ipv4.Assign (devices);
      serverAddress = Address (i.GetAddress (1));
    }
  else
    {
      Ipv6AddressHelper ipv6;
      ipv6.SetBase ("2001:0000:f00d:cafe::", Ipv6Prefix (64));
      Ipv6InterfaceContainer i6 = ipv6.Assign (devices);
      serverAddress = Address(i6.GetAddress (1,1));
    }

  NS_LOG_INFO ("Create UDP server application on node 1.");
  uint16_t port = 4000;
  UdpServerHelper server (port);
  ApplicationContainer apps = server.Install (n.Get (1));
  apps.Start (Seconds (1.0));
  apps.Stop (Seconds (10.0));

  NS_LOG_INFO ("Create UDP client application on node 0 to send to node 1.");
  uint32_t MaxPacketSize = 1024;
  Time interPacketInterval = Seconds (0.05);
  uint32_t maxPacketCount = 320;
  UdpClientHelper client (serverAddress, port);
  client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  client.SetAttribute ("Interval", TimeValue (interPacketInterval));
  client.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));
  apps = client.Install (n.Get (0));
  apps.Start (Seconds (2.0));
  apps.Stop (Seconds (10.0));

  // Create animation object before running the simulation
  AnimationInterface anim ("UdpClientServerExample.xml"); // This will create the XML file

  // Set positions for the nodes in the animation with larger distance
  anim.SetConstantPosition (n.Get(0), 2.0, 2.0);  // Position node 0 at (2, 2)
  anim.SetConstantPosition (n.Get(1), 50.0, 50.0); // Position node 1 at (50, 50) to keep them far apart

  // Set the tracing for the UDP packets
  AsciiTraceHelper ascii;
  p2p.EnableAsciiAll(ascii.CreateFileStream("udp-trace.tr"));
  p2p.EnablePcapAll("udp-pcap");

  NS_LOG_INFO ("Run the simulation.");
  Simulator::Run ();

  // The XML file will be generated once the simulation finishes running
  Simulator::Destroy ();

  NS_LOG_INFO ("Done.");
}

