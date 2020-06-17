//
//  Protocols.swift
//  PlantCO2Meter
//
//  Created by Sam Bower on 5/29/20.
//  Copyright © 2020 Sam Bower. All rights reserved.
//
import Foundation

protocol sendDataDelegate: class{
    func Transmit(transmissionData: Data?)
}

