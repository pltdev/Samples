/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.plantronics.UC.JavaSocketWrapper;



/**
 * AttachedArgs
 * @author lcollins
 */
public class AttachedArgs extends EventArgs 
{
    public String m_device;
    public String m_deviceid;
    
    public AttachedArgs(String aDevice, String deviceId)
    {
        m_device = aDevice;
        m_deviceid = deviceId;
    }
}
