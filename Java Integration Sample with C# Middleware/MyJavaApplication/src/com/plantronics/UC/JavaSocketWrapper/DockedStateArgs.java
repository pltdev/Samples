/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.plantronics.UC.JavaSocketWrapper;

/**
 * Event arguments for Docked/Un-docked event handlers
 * @author lcollins
 */
public class DockedStateArgs extends EventArgs 
{
    public boolean m_docked = false;
    public boolean m_isInitialStateEvent = false;

    public DockedStateArgs(boolean docked, boolean isInitialStateEvent)
    {
        m_docked = docked;
        m_isInitialStateEvent = isInitialStateEvent;
    }
}
