//
//  ConfigController.swift
//  PlantCO2Meter
//
//  Created by Sam Bower on 5/22/20.
//  Copyright © 2020 Sam Bower. All rights reserved.
//

import UIKit


class ConfigController: UIViewController {
    weak var sendDelegate: sendDataDelegate?
    private var datePicker: UIDatePicker?
    
    @IBOutlet weak var dateAndTimeField: UITextField!
    @IBOutlet weak var measurementPeriodField: UITextField!
    @IBOutlet weak var measurementNumberField: UITextField!
    @IBOutlet weak var shutDownField: UITextField!
    
    
    override func viewDidLoad() {
        super.viewDidLoad()
        self.navigationItem.title = "Configuration";
        datePicker = UIDatePicker()
        datePicker?.datePickerMode = .dateAndTime
        datePicker?.addTarget(self, action: #selector(ConfigController.dateChanged(datePicker:)), for: .valueChanged)
        dateAndTimeField.inputView = datePicker
        let tapGesture = UITapGestureRecognizer(target: self, action: #selector(ConfigController.viewTapped(gestureRecognizer:)))
        view.addGestureRecognizer(tapGesture)
    }
    
    
    @objc func viewTapped(gestureRecognizer: UITapGestureRecognizer){
        view.endEditing(true)
    }
    @objc func dateChanged(datePicker: UIDatePicker){
        let dateFormatter = DateFormatter()
        dateFormatter.dateFormat = "MM/dd/yy H:mm"
        dateAndTimeField.text = dateFormatter.string(from: datePicker.date)
        view.endEditing(true)
    }
    
    
    //protocol method is called on Configure button press
    @IBAction func buttonTimeConfigure(_ sender: Any){
        let stringForParsing = dateAndTimeField.text
        let separatorSet = CharacterSet(charactersIn: "/ :")
        let comps = stringForParsing!.components(separatedBy: separatorSet)
        var UInt8Array = [UInt8]()
        for i in 0...4{
            UInt8Array.append(UInt8(comps[i]) ?? 0)
        }
        print(comps)
        UInt8Array.insert(messageID.CONFIGURE_DATE_TIME.rawValue, at: 0)
        print(UInt8Array)
        let data = Data(bytes: UInt8Array)
        print(data)
        sendDelegate?.Transmit(transmissionData: data)
    }
    @IBAction func buttonNumberConfigure(_ sender: Any) {
        let numberString = UInt8(measurementNumberField.text ?? "0")
        var payloadArray = [UInt8]()
        payloadArray.append(numberString ?? 0)
        payloadArray.insert(messageID.CONFIGURE_TOTAL_MEASUREMENTS.rawValue, at: 0)
        //print(payloadArray)
        let payload = Data(bytes: payloadArray)
        sendDelegate?.Transmit(transmissionData: payload)
    }
    @IBAction func buttonPeriodConfigure(_ sender: Any) {
        let periodTime = UInt16(measurementPeriodField.text ?? "0")
        var bigEndian = periodTime?.bigEndian
        var bytes = withUnsafeBytes(of: &bigEndian) { Array($0) }
        bytes.removeLast()
        bytes.insert(messageID.CONFIGURE_CYCLE_PERIOD.rawValue, at: 0) //message ID inserted at beginning of byte array
        let data1 = Data(bytes: bytes)
        sendDelegate?.Transmit(transmissionData: data1)
    }
    @IBAction func buttonPowerConfigure(_ sender: Any) {
        let shutDownTime = UInt16(shutDownField.text ?? "0")
        var bigEndian = shutDownTime?.bigEndian
        var bytes = withUnsafeBytes(of: &bigEndian) { Array($0) }
        bytes.removeLast()
        bytes.insert(messageID.POWER_OFF_REQUEST.rawValue, at: 0) //message ID inserted at beginning of byte array
        let data1 = Data(bytes: bytes)
        sendDelegate?.Transmit(transmissionData: data1)
    }
    
}
