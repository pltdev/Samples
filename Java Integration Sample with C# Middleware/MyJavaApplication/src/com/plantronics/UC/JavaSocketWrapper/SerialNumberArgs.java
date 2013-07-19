/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.plantronics.UC.JavaSocketWrapper;



/**
 * Event arguments for SerialNumber event handler
 * @author lcollins
 */
public class SerialNumberArgs extends EventArgs 
{
    public String SerialNumber;
    public SerialNumberTypes SerialNumberType;

    public SerialNumberArgs(String serialnumber, SerialNumberTypes serialnumtype)
    {
        SerialNumber = serialnumber;
        SerialNumberType = serialnumtype;
    }
}
