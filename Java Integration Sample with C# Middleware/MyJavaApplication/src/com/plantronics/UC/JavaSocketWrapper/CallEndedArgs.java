/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.plantronics.UC.JavaSocketWrapper;



/**
 * Event arguments for CallEnded event handler
 * @author lcollins
 */
public class CallEndedArgs extends EventArgs 
{
    public int CallId;
    public String CallSource;

    public CallEndedArgs(int callid, String callsource)
    {
        CallId = callid;
        CallSource = callsource;
    }
}
