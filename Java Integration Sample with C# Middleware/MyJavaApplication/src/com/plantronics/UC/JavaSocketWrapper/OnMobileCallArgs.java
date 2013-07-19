/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.plantronics.UC.JavaSocketWrapper;



/**
 * Event arguments for OnMobileCall event handler
 * @author lcollins
 */
public class OnMobileCallArgs extends EventArgs 
{
    public boolean Incoming;
    public MobileCallState State;

    public OnMobileCallArgs(boolean isIncoming, MobileCallState state)
    {
        Incoming = isIncoming;
        State = state;
    }
}
