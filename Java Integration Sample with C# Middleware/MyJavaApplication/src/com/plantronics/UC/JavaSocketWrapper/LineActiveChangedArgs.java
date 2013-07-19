/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.plantronics.UC.JavaSocketWrapper;



/**
 * Event arguments for LineActiveChanged event handler
 * @author lcollins
 */
public class LineActiveChangedArgs extends EventArgs 
{
    public Boolean m_lineactive = false;

    public LineActiveChangedArgs(Boolean isLineActive)
    {
        m_lineactive = isLineActive;
    }
}
