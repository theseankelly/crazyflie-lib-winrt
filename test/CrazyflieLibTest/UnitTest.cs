
using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading.Tasks;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Windows.Devices.Enumeration;
using bitcraze.crazyflielib;

namespace CrazyflieLibTest
{
    [TestClass]
    public class FunctionalTests
    {
        [TestMethod]
        public void ScanInterfacesTest()
        {
            // Assuming at least once device is paired, verify it is found.
            IList<string> devices = null;
            System.Threading.Tasks.Task.Run(async () =>
            {
                devices = await Crazyflie.ScanInterfacesAsync();
            }).GetAwaiter().GetResult();
            Assert.IsTrue(devices.Count >= 1);
            Debug.WriteLine("Found devices: ");
            foreach (string device in devices)
            {
                Debug.WriteLine("ID:   " + device);
            }
        }

        [TestMethod]
        public void ConnectTest()
        {
            // Connect to the first available device. Should not throw.
            IList<string> devices = null;
            System.Threading.Tasks.Task.Run(async () =>
            {
                devices = await Crazyflie.ScanInterfacesAsync();
                Crazyflie c = new Crazyflie(devices[0]);
                await c.ConnectAsync();
            }).GetAwaiter().GetResult();
        }

        [TestMethod]
        public void SendSetpointTest()
        {
            // Connect to the first available device. Should not throw.
            IList<string> devices = null;
            System.Threading.Tasks.Task.Run(async () =>
            {
                devices = await Crazyflie.ScanInterfacesAsync();
                Crazyflie c = new Crazyflie(devices[0]);
                await c.ConnectAsync();

                bool ret = await c.SendCommanderPacketAsync(0, 0, 0, 0);
                Assert.IsTrue(ret);
                ret = await c.SendCommanderPacketAsync(0, 0, 0, 0.1f);
                Assert.IsTrue(ret);
            }).GetAwaiter().GetResult();
        }
    }
 }
