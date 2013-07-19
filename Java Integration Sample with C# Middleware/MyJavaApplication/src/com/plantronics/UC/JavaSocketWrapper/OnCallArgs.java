/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.plantronics.UC.JavaSocketWrapper;



/**
 * Event arguments for OnCall event handler
 * @author lcollins
 */
public class OnCallArgs extends EventArgs 
{
    public String CallSource;
    public Boolean Incoming;
    public OnCallCallState State;

    public OnCallArgs(String source, Boolean isIncoming, OnCallCallState state)
    {
        CallSource = source;
        Incoming = isIncoming;
        State = state;
    }
}
