/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.plantronics.UC.JavaSocketWrapper;



/**
 * Event arguments for CallAnswered event handler
 * @author lcollins
 */
public class CallAnsweredArgs extends EventArgs 
{
    public int CallId;
    public String CallSource;

    public CallAnsweredArgs(int callid, String callsource)
    {
        CallId = callid;
        CallSource = callsource;
    }
}
