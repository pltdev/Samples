/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.plantronics.UC.JavaSocketWrapper;



/**
 * EventArgs used with MultiLineStateChanged event handler to receive status of multiple lines (PC, Mobile, Deskphone) 
 * when the state of any of these lines changes.
 * @author lcollins
 */
public class MultiLineStateArgs extends EventArgs 
{
    public MultiLineStateFlags MultiLineState;

    public MultiLineStateArgs(MultiLineStateFlags multilinestate)
    {
        MultiLineState = multilinestate;
    }
}
