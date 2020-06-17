//
//  ViewController.swift
//  PlantCO2Meter
//
//  Created by Sam Bower on 5/22/20.
//  Copyright © 2020 Sam Bower. All rights reserved.
//

import UIKit
import CoreBluetooth

class Model{
    var batteryMinutes: Int?
    var numMeasurements: Int?
    var hourFirst: Int?
    var minuteFirst: Int?
}


class RootController: UIViewController, CBCentralManagerDelegate, CBPeripheralDelegate, sendDataDelegate {
    //Bluetooth Variables
    var manager:CBCentralManager? = nil
    var mainPeripheral:CBPeripheral? = nil
    var mainCharacteristic:CBCharacteristic? = nil
    let BLEService = "FFE0"
    let BLECharacteristic = "FFE1"
    //instantiating classes
    let model1 = Model()
    var statusVC = StatusController()
    //Outlet Variables
    @IBOutlet weak var printField: UILabel!
    @IBOutlet weak var FieldUnitButton: UIButton!
    
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        self.navigationItem.title = "Home"
        manager = CBCentralManager(delegate: self, queue: nil);
        customiseNavigationBar()
    }
    
    //Setups the button to the Scan Controller View
    func customiseNavigationBar () {
        self.navigationItem.rightBarButtonItem = nil
        let rightButton = UIButton()
        if (mainPeripheral == nil) {
            rightButton.setTitle("Scan", for: [])
            rightButton.setTitleColor(UIColor.blue, for: [])
            rightButton.frame = CGRect(origin: CGPoint(x: 0,y :0), size: CGSize(width: 60, height: 30))
            rightButton.addTarget(self, action: #selector(self.scanButtonPressed), for: .touchUpInside)
        } else {
            rightButton.setTitle("Disconnect", for: [])
            rightButton.setTitleColor(UIColor.blue, for: [])
            rightButton.frame = CGRect(origin: CGPoint(x: 0,y :0), size: CGSize(width: 100, height: 30))
            rightButton.addTarget(self, action: #selector(self.disconnectButtonPressed), for: .touchUpInside)
        }
        let rightBarButton = UIBarButtonItem()
        rightBarButton.customView = rightButton
        self.navigationItem.rightBarButtonItem = rightBarButton
    }

    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        //rootToScanTable
        if (segue.identifier == "scanSegue") {
            let scanController: ScanViewController = segue.destination as! ScanViewController
            //set the manager's delegate to the scan view so it can call relevant connection methods
            manager?.delegate = scanController
            scanController.manager = manager
            scanController.parentView = self
        }
        if (segue.identifier == "rootToConfig") {
            if let configVC = segue.destination as? ConfigController {
                configVC.sendDelegate = self
            }
        }
        if (segue.identifier == "rootToStatus") {
            statusVC = segue.destination as! StatusController
            statusVC.sendDelegate = self
            statusVC.model = model1
        }
    }
    
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        print(central.state)
    }
    // MARK: - CBCentralManagerDelegate Methods, updates root view after disconnecting
    func centralManager(_ central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: Error?) {
        mainPeripheral = nil
        customiseNavigationBar()
        print("Disconnected" + peripheral.name!)
    }

    
    
    
    //Button Press Segues
    @IBAction func buttonPressShowDetails(_ sender: UIButton) {
        performSegue(withIdentifier: "rootToConfig", sender: self)
    }
    @IBAction func buttonMeasAndStat(_ sender: Any) {
        performSegue(withIdentifier: "rootToStatus", sender: self)
    }
    @objc func scanButtonPressed() {
        performSegue(withIdentifier: "scanSegue", sender: nil)
    }
    @objc func disconnectButtonPressed() {
        //this will call didDisconnectPeripheral, but if any other apps are using the device it will not immediately disconnect
        manager?.cancelPeripheralConnection(mainPeripheral!)
    }
    
    //Other Button Actions
    @IBAction func buttonConnectToField(_ sender: Any) {
        struct S { static var v = "not connected" }
        var payloadArray = [UInt8]()
        payloadArray.append(0)
        if (S.v == "not connected") {
            FieldUnitButton.setTitle("Connect To Field Unit", for: [])
                S.v = "connected"
            payloadArray.insert(messageID.SET_SETUP_MODE.rawValue, at: 0)
        }
        else if (S.v == "connected") {
            FieldUnitButton.setTitle("Disconnect from Field Unit", for: [])
                S.v = "not connected"
            payloadArray.insert(messageID.END_SETUP_MODE.rawValue, at: 0)
        }
        print(payloadArray)
        let payload = Data(bytes: payloadArray)
        Transmit(transmissionData: payload)
    }
    
    
    
    // MARK: CBPeripheralDelegate Methods
    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        for service in peripheral.services! {
            print("Service found with UUID: " + service.uuid.uuidString)
            //device information service
            if (service.uuid.uuidString == "180A") {
                peripheral.discoverCharacteristics(nil, for: service)
            }
            //GAP (Generic Access Profile) for Device Name
            // This replaces the deprecated CBUUIDGenericAccessProfileString
            if (service.uuid.uuidString == "1800") {
                peripheral.discoverCharacteristics(nil, for: service)
            }
            //HM-10 Service
            if (service.uuid.uuidString == BLEService) {
                peripheral.discoverCharacteristics(nil, for: service)
            }
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        //get device name
//        if (service.uuid.uuidString == "1800") {
//            for characteristic in service.characteristics! {
//                if (characteristic.uuid.uuidString == "2A00") {
//                    peripheral.readValue(for: characteristic)
//                    print("Found Device Name Characteristic")
//                }
//            }
//        }
//
//        if (service.uuid.uuidString == "180A") {
//            for characteristic in service.characteristics! {
//                if (characteristic.uuid.uuidString == "2A29") {
//                    peripheral.readValue(for: characteristic)
//                    print("Found a Device Manufacturer Name Characteristic")
//                } else if (characteristic.uuid.uuidString == "2A23") {
//                    peripheral.readValue(for: characteristic)
//                    print("Found System ID")
//                }
//            }
//        }
        if (service.uuid.uuidString == BLEService) {
            for characteristic in service.characteristics! {
                if (characteristic.uuid.uuidString == BLECharacteristic) {
                    //we'll save the reference, we need it to write data
                    mainCharacteristic = characteristic
                    //Set Notify is useful to read incoming data async
                    peripheral.setNotifyValue(true, for: characteristic)
                    print("Found HM-10 Data Characteristic")
                }
            }
        }
    }
    //reading happens below i think...
    func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?) {
        if (characteristic.uuid.uuidString == BLECharacteristic) {
            //data recieved
            if(characteristic.value != nil) {
                //let stringValue = String(data: (characteristic.value)!, encoding: String.Encoding.utf8)
                //var values = [UInt8]()
                var data = characteristic.value
                //data.copyBytes(&values, length:data?.count)
                var values = [UInt8](repeating:0, count:data!.count) //creates array
                data?.copyBytes(to: &values, count: data!.count) //fills array with stuff from data
                
                //identifies what message is being sent from REMOTE to GUI
                switch values[0] {
                    case messageID.ID_INVALID.rawValue:
                        print("ID_INVALID")
                    case messageID.CONFIGURE_DATE_TIME_ACK.rawValue:
                        print("CONFIGURE_DATE_TIME_ACK")
                        printField.text = "CONFIGURE_DATE_TIME_ACK"
                    case messageID.MEASUREMENT_REQUEST_ACK.rawValue:
                        print("MEASUREMENT_REQUEST_ACK")
                        printField.text = "MEASUREMENT_REQUEST_ACK"
                    case messageID.CONFIGURE_TOTAL_MEASUREMENTS_ACK.rawValue:
                        print("CONFIGURE_TOTAL_MEASUREMENTS_ACK")
                        printField.text = "CONFIGURE_TOTAL_MEASUREMENTS_ACK"
                    case messageID.CONFIGURE_CYCLE_PERIOD_ACK.rawValue:
                        print("CONFIGURE_CYCLE_PERIOD_ACK")
                        printField.text = "CONFIGURE_CYCLE_PERIOD_ACK"
                    case messageID.CONFIGURE_CYCLE_PERIOD_ACK.rawValue:
                        print("CONFIGURE_CYCLE_PERIOD_ACK")
                        printField.text = "CONFIGURE_CYCLE_PERIOD_ACK"
                    case messageID.POWER_OFF_REQUEST_ACK.rawValue:
                        print("POWER_OFF_REQUEST_ACK")
                    case messageID.UPDATE_STATUS_REQUEST_ACK.rawValue:
                        print("UPDATE_STATUS_REQUEST_ACK")
                        model1.batteryMinutes = Int(values[2])
                        model1.numMeasurements = Int(values[3])
                        model1.hourFirst = Int(values[4])
                        model1.minuteFirst = Int(values[5])
                        statusVC.viewDidLoad()
                    case messageID.DATA_TRANSMISSION_ACK.rawValue:
                        print("DATA_TRANSMISSION_ACK")
                    case messageID.COMMUNICATION_FAILURE.rawValue:
                        print("COMMUNICATION_FAILURE")
                    case messageID.SET_SETUP_MODE_ACK.rawValue:
                        print("SET_SETUP_MODE_ACK")
                    case messageID.END_SETUP_MODE_ACK.rawValue:
                        print("END_SETUP_MODE_ACK")
                    default:
                        print("Unregistered Message ID")
                    break
                }
                
                
                
                
                
//
//
//                let array : [UInt8] = [values[0],values[1]]
//                let bigEndianValue = array.withUnsafeBufferPointer {
//                    ($0.baseAddress!.withMemoryRebound(to: UInt16.self, capacity: 1) { $0 })
//                    }.pointee
//                let value = UInt16(bigEndian: bigEndianValue)
//
//                print(values)
//                print(values)
//
//                let data1 = Data(bytes: values)
//                let data2 = data1[2...3]
//                let value = UInt16(bigEndian: data2.withUnsafeBytes { $0.pointee })
//                print(value)
                
                
                
//                printField.text = String(value)
                //recievedMessageText.text = stringValue
            }
        }
    }
    
    
}//end of root controller


//data sender, part of sendDataDelegate protocol
extension RootController{
    func Transmit(transmissionData: Data?) {
        print("Transmit Protocol Called")
        if (mainPeripheral != nil) {
            mainPeripheral?.writeValue(transmissionData!, for: mainCharacteristic!, type: CBCharacteristicWriteType.withoutResponse)
        } else {
            print("haven't discovered device yet")
        }
    }
}
