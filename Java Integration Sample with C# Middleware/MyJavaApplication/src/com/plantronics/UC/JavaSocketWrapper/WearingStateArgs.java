/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.plantronics.UC.JavaSocketWrapper;

/**
 * Event arguments for TakenOff/PutOn events (wearing state) event handlers
 * @author lcollins
 */
public class WearingStateArgs extends EventArgs 
{
    public boolean m_worn = false;
    public boolean m_isInitialStateEvent = false;

    public WearingStateArgs(boolean worn, boolean isInitialStateEvent)
    {
        m_worn = worn;
        m_isInitialStateEvent = isInitialStateEvent;
    }
}
