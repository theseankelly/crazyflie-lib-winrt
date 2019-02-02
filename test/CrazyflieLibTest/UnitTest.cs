
using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading.Tasks;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Windows.Devices.Enumeration;
using Windows.Devices.Radios;
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
        public void InitializeTest()
        {
            IList<string> devices = null;
            System.Threading.Tasks.Task.Run(async () =>
            {
                devices = await Crazyflie.ScanInterfacesAsync();
                Assert.IsTrue(devices.Count >= 1);
                Crazyflie c = new Crazyflie(devices[0]);
                CrazyflieStatus status = await c.InitializeAsync();
                Assert.IsTrue(status == CrazyflieStatus.Success);
            }).GetAwaiter().GetResult();
        }

        [TestMethod]
        public void SendSetpointTest()
        {
            IList<string> devices = null;
            System.Threading.Tasks.Task.Run(async () =>
            {
                devices = await Crazyflie.ScanInterfacesAsync();
                Assert.IsTrue(devices.Count >= 1);
                Crazyflie c = new Crazyflie(devices[0]);
                CrazyflieStatus status = await c.InitializeAsync();
                Assert.IsTrue(status == CrazyflieStatus.Success);

                status = await c.SendCommanderPacketAsync(0, 0, 0, 0);
                Assert.IsTrue(status == CrazyflieStatus.Success);
                status = await c.SendCommanderPacketAsync(0, 0, 0, 0.1f);
                Assert.IsTrue(status == CrazyflieStatus.Success);
            }).GetAwaiter().GetResult();
        }
    }

    [TestClass]
    public class NegativeTests
    {
        [TestMethod]
        public void ConnectToInvalidBluetoothID()
        {
            System.Threading.Tasks.Task.Run(async () =>
            {
                Crazyflie c = new Crazyflie("BluetoothLE#BluetoothLE00:00:00:00:00:00-00:00:00:00:00:00");
                CrazyflieStatus status = await c.InitializeAsync();
                Assert.IsTrue(status == CrazyflieStatus.InvalidDeviceId);
            }).GetAwaiter().GetResult();
        }
    }
 }
