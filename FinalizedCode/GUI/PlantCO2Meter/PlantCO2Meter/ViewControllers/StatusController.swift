//
//  StatusController.swift
//  PlantCO2Meter
//
//  Created by Sam Bower on 5/29/20.
//  Copyright © 2020 Sam Bower. All rights reserved.
//

import UIKit


class StatusController: UIViewController {
    weak var sendDelegate: sendDataDelegate?
    var model: Model?
    
    @IBOutlet weak var statusNotification: UILabel?
    
    override func viewDidLoad() {
        super.viewDidLoad()
        self.navigationItem.title = "Field Unit Status"
        if let batteryCharge = model!.batteryMinutes{
            statusNotification?.text = """
            battery charge at: \(batteryCharge) %
            measurements taken: \(String(describing: model!.numMeasurements!))
            first measurement occurred:\(String(describing: model!.hourFirst!)):\(String(describing: model!.minuteFirst!))
            """
        }
        else{
            statusNotification?.text = "Field Unit status is unknown."
        }
        print("viewDidLoad called in Status")

    }
    
    
    @IBAction func buttonMeasurementRequest(_ sender: Any) {
        var payloadArray = [UInt8]()
        payloadArray.append(0)
        payloadArray.insert(messageID.MEASUREMENT_REQUEST.rawValue, at: 0)
        let payload = Data(bytes: payloadArray)
        sendDelegate?.Transmit(transmissionData: payload)
    }
    @IBAction func buttonFieldUpdate(_ sender: Any) {
        var payloadArray = [UInt8]()
        payloadArray.append(messageID.UPDATE_STATUS_REQUEST.rawValue)
        let payload = Data(bytes: payloadArray)
        sendDelegate?.Transmit(transmissionData: payload)
    }
    

}//end of StatusController class
