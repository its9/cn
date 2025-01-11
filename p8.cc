#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
//#include "ns3/tcp-westwood.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("TcpExampleWithAnimation");

int main(int argc, char *argv[]) {
  CommandLine cmd;
  cmd.Parse(argc, argv);

  // Create nodes
  NodeContainer nodes;
  nodes.Create(2);

  // Create a point-to-point link between nodes
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
  pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

  NetDeviceContainer devices;
  devices = pointToPoint.Install(nodes);

  // Install the internet stack on the nodes
  InternetStackHelper stack;
  stack.Install(nodes);

  // Assign IP addresses
  Ipv4AddressHelper address;
  address.SetBase("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces = address.Assign(devices);

  // Install TCP applications
  uint16_t port = 9;

  // Create a TCP server on node 1
  Address serverAddress = InetSocketAddress(interfaces.GetAddress(1), port);
  PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory", serverAddress);
  ApplicationContainer serverApps = packetSinkHelper.Install(nodes.Get(1));
  serverApps.Start(Seconds(1.0));
  serverApps.Stop(Seconds(10.0));

  // Create a TCP client on node 0
  OnOffHelper client("ns3::TcpSocketFactory", serverAddress);
  client.SetAttribute("DataRate", StringValue("1Mbps"));
  client.SetAttribute("PacketSize", UintegerValue(2000));
  ApplicationContainer clientApps = client.Install(nodes.Get(0));
  clientApps.Start(Seconds(2.0));
  clientApps.Stop(Seconds(10.0));

  // Enable tracing to analyze the congestion window
  AsciiTraceHelper ascii;
  pointToPoint.EnableAsciiAll(ascii.CreateFileStream("tcp-example.tr"));

  // Enable animation
  AnimationInterface anim("tcp-animation.xml");

  // Run the simulation
  Simulator::Run();
  Simulator::Destroy();

  return 0;
}

