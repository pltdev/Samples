/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.plantronics.UC.JavaSocketWrapper;



/**
 * Event arguments for MuteChanged event handler
 * @author lcollins
 */
public class MuteChangedArgs extends EventArgs 
{
    public Boolean m_muteon = false;

    public MuteChangedArgs(Boolean isMuteOn)
    {
        m_muteon = isMuteOn;
    }
}
